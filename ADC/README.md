# ADC

PA9 핀에 들어오는 아날로그 전압(쓰로틀/가변저항 등)을 **ADC5 채널 2(ADC5_IN2)** 로 읽어 전압으로 환산하는 실습.
HAL 없이 레지스터를 직접 설정하며, 메인 루프에서 소프트웨어 트리거 단일 변환(polling)을 반복한다.

모든 코드는 `Core/Src/main.c`에 있다.

## 동작 흐름

```
main()
 ├─ Initialize_MCU()          // SYSCLK 170MHz 설정 (Clock 프로젝트와 동일)
 ├─ GPIOA 클록 ON, PA9 Analog 모드
 ├─ ADC345 클록 설정          // 버스 클록 ON, 커널 클록 = SYSCLK / 10 = 17MHz
 ├─ ADC5 초기화               // DEEPPWD 해제 → 레귤레이터 ON → 캘리브레이션 → ADEN
 ├─ 채널 설정                 // SMP2 = 24.5 cycles, SQ1 = 채널 2
 └─ while(1)
     ├─ ADSTART = 1           // 변환 시작
     ├─ EOC 대기
     ├─ adc_raw = ADC5->DR    // 12-bit 결과 (DR 읽으면 EOC 자동 클리어)
     └─ throttle_V = adc_raw * 3.3 / 4095
```

결과는 전역 변수 `adc_raw`, `throttle_V`(`volatile`)에 저장되므로 디버거의 Live Expressions로 확인한다.

## 1. 클록 설정 — `Initialize_MCU()`

[Clock](../Clock) 프로젝트와 동일하게 HSE(24MHz) + PLL로 SYSCLK = **170MHz**, AHB = APB1 = APB2 = 170MHz로 설정한다.
(FLASH 4 wait state, PLLM=/6, PLLN=×85, PLLR=/2, boost 모드, CSS 활성화 등 상세 내용은 Clock 프로젝트 README 참고)

## 2. GPIO 설정

| 레지스터 | 설정 | 설명 |
|---|---|---|
| `RCC->AHB2ENR` | bit0 GPIOAEN = 1 | GPIOA 클록 ON |
| `GPIOA->MODER` | MODER9[19:18] = 11 | PA9 Analog 모드 (ADC5_IN2) |

## 3. ADC345 클록 설정

ADC는 **버스 클록**(레지스터 접근용)과 **커널 클록**(실제 변환용) 두 개가 필요하다.

| 레지스터 | 설정 | 설명 |
|---|---|---|
| `RCC->AHB2ENR` | bit14 ADC345EN = 1 | ADC3/4/5 버스 클록 ON |
| `RCC->CCIPR` | ADC345SEL[31:30] = 10 | 커널 클록 소스 = SYSCLK (리셋값 00 = No clock) |
| `ADC345_COMMON->CCR` | PRESC[21:18] = 0101 | /10 → 170MHz / 10 = **17MHz** (허용 0.14~60MHz) |

- PRESC는 `CKMODE[17:16] = 00`(비동기 모드, 리셋값)일 때만 적용된다.
- CCR은 ADC가 꺼져 있을 때(ADEN = 0)만 쓸 수 있으므로 ADC enable 전에 설정한다.

## 4. ADC5 초기화 순서

| 순서 | 레지스터 | 설정 | 설명 |
|---|---|---|---|
| 1 | `ADC5->CR` | DEEPPWD(bit29) = 0 | Deep power-down 해제 |
| 2 | `ADC5->CR` | ADVREGEN(bit28) = 1 | 내부 전압 레귤레이터 ON |
| - | - | 소프트웨어 지연 | tADCVREG_STUP ≥ 20µs 대기 |
| 3 | `ADC5->CR` | ADCALDIF(bit30) = 0, ADCAL(bit31) = 1 | Single-ended 캘리브레이션 시작, 완료(ADCAL = 0)까지 대기 |
| - | - | 짧은 지연 | 캘리브레이션 후 4 ADC 클록 동안 ADEN 금지 |
| 4 | `ADC5->CR` | ADEN(bit0) = 1 | ADC enable, `ISR.ADRDY`(bit0) = 1 대기 |
| 5 | `ADC5->SMPR1` | SMP2[8:6] = 011 | 채널 2 샘플링 시간 24.5 cycles (≒ 1.44µs @ 17MHz) |
| 6 | `ADC5->SQR1` | SQ1[10:6] = 2, L = 0 | 정규 시퀀스 1번째 = 채널 2, 총 1개 변환 |

`CFGR`은 리셋값 그대로 사용한다 (CONT = 0 단일 변환, ALIGN = 0 우측 정렬, RES = 00 12-bit).

## 5. 변환 (메인 루프)

1. `ADC5->CR` ADSTART(bit2) = 1 → 정규 채널 변환 시작
2. `ADC5->ISR` EOC(bit2) = 1 될 때까지 대기
3. `ADC5->DR` 읽기 → EOC 자동 클리어
4. `throttle_V = adc_raw * VREF / 4095` (VREF = 3.3V)

## 개선할 점 / 주의사항

- **지연 루프**: `for (volatile ...)` 소프트웨어 지연은 컴파일 최적화/클록에 따라 실제 시간이 달라진다. 현재 값(4000회)은 170MHz에서 20µs를 충분히 넘지만, SysTick/타이머 기반 지연으로 바꾸면 더 명확하다.
- **SMPR1 설정**: `|=`로 OR만 하므로 SMP2가 리셋값(000)이라는 전제에 의존한다. 다른 값으로 바꿀 땐 필드를 먼저 클리어해야 한다.
- **VREF**: 전압 환산은 VDDA = 3.3V를 가정한다. 정확한 값이 필요하면 VREFINT 채널로 실제 VDDA를 보정할 수 있다.
- **폴링 방식**: CPU가 EOC를 계속 기다린다. 연속 변환 + 인터럽트/DMA 방식으로 확장 가능.
- `ADC.ioc`는 CubeMX 기본값 상태이며 실제 설정은 코드에서 레지스터로 직접 처리한다.
- `Initialize_MCU()`의 (6) SYSCFG 클록 설정은 이 실습에서 사용하지 않는다.

## 하드웨어

- 보드: NUCLEO-G474RE
- 아날로그 입력: PA9 (ADC5_IN2) — 가변저항 와이퍼 등 0~3.3V 전압 연결
