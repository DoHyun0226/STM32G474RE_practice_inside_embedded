# TimerCounter

**TIM1 업데이트(오버플로우) 인터럽트**로 1초마다 LD2 LED(PA5)를 토글하는 실습.
HAL 없이 레지스터를 직접 설정하며, 메인 루프는 비워 두고 LED 토글은 인터럽트 서비스 루틴(ISR)에서 처리한다.

모든 코드는 `Core/Src/main.c`에 있다.

## 동작 흐름

```
main()
 ├─ Initialize_MCU()          // SYSCLK 170MHz 설정 (Clock 프로젝트와 동일)
 ├─ GPIOA 클록 ON, PA5 출력 / medium speed
 ├─ TIM1 클록 ON
 ├─ PSC = 17000-1, ARR = 10000-1, UG로 즉시 로드
 ├─ DIER.UIE = 1, NVIC TIM1_UP_TIM16 활성화
 ├─ CR1.CEN = 1               // 카운터 시작
 └─ while(1) { }              // 대기만 함

1초마다 카운터 오버플로우
 └─ TIM1_UP_TIM16_IRQHandler()
     ├─ SR.UIF 확인 및 클리어
     └─ GPIOA->ODR ^= (1<<5)  // LED 토글
```

## 1. 클록 설정 — `Initialize_MCU()`

[Clock](../Clock) 프로젝트와 동일하게 HSE(24MHz) + PLL로 SYSCLK = **170MHz**, AHB = APB1 = APB2 = 170MHz로 설정한다.
TIM1은 APB2에 연결되어 있고 APB2 프리스케일러가 1이므로 **타이머 클록 = 170MHz**이다.

## 2. GPIO 설정

| 레지스터 | 설정 | 설명 |
|---|---|---|
| `RCC->AHB2ENR` | bit0 GPIOAEN = 1 | GPIOA 클록 ON |
| `GPIOA->MODER` | MODER5 = 01 | PA5 General-purpose output |
| `GPIOA->OSPEEDR` | OSPEEDR5 = 01 | PA5 Medium speed |

## 3. TIM1 설정

| 순서 | 레지스터 | 설정 | 설명 |
|---|---|---|---|
| 1 | `RCC->APB2ENR` | bit11 TIM1EN = 1 | TIM1 클록 ON |
| 2 | `TIM1->PSC` | 17000 - 1 | 170MHz / 17000 = **10kHz** 카운터 클록 |
| 3 | `TIM1->ARR` | 10000 - 1 | 10kHz / 10000 = **1Hz** (1초마다 오버플로우) |
| 4 | `TIM1->EGR` | UG = 1 | 업데이트 이벤트를 강제로 발생시켜 PSC/ARR을 즉시 로드 |
| 5 | `TIM1->DIER` | UIE = 1 | 업데이트 인터럽트 허용 |
| 6 | NVIC | `NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn)` | TIM1 업데이트 / TIM16 공용 IRQ 활성화 |
| 7 | `TIM1->CR1` | CEN = 1 | 카운터 시작 |

### 참고 포인트

- **주기 계산**: `주기 = (PSC + 1) × (ARR + 1) / 타이머 클록 = 17000 × 10000 / 170MHz = 1초`.
  레지스터 값은 "나누는 수 - 1"을 넣는다 (0이면 1분주).
- **UG가 필요한 이유**: PSC는 버퍼(프리로드) 레지스터라 값을 써도 다음 업데이트 이벤트에서야 실제로 적용된다. UG로 이벤트를 한 번 일으켜 바로 적용한다.
- **IRQ 공유**: TIM1 업데이트 인터럽트는 TIM16과 벡터 하나(`TIM1_UP_TIM16_IRQn`)를 공유하므로 핸들러 안에서 `TIM1->SR`의 UIF를 확인한다.

## 4. 인터럽트 서비스 루틴 — `TIM1_UP_TIM16_IRQHandler()`

- 핸들러 이름은 startup 파일의 벡터 테이블 심볼과 동일해야 한다.
- `TIM1->SR` bit0 UIF로 업데이트 이벤트를 확인하고 클리어한 뒤 LED를 토글한다.

## 개선할 점 / 주의사항

- **시작 직후 1회 토글**: `EGR = UG`가 UIF 플래그도 세우기 때문에, UIE와 NVIC를 켜는 순간 인터럽트가 한 번 바로 들어와 LED가 즉시 토글될 수 있다. 원치 않으면 UG 직후 `TIM1->SR = 0;`으로 플래그를 지운다.
- **SR 클리어 방식**: SR은 `rc_w0`(0을 써서 클리어) 레지스터이다. `TIM1->SR &= ~1`은 읽기-수정-쓰기 사이에 다른 플래그가 세워지면 그 플래그까지 지울 수 있으므로 `TIM1->SR = ~0x1UL;` 처럼 직접 대입하는 것이 일반적이다.
- **LED 토글**: `ODR ^=`도 읽기-수정-쓰기이다. 메인 루프에서 같은 포트를 건드리게 되면 `BSRR` 사용을 고려한다.
- `TimerCounter.ioc`는 CubeMX 기본값 상태이며 실제 설정은 코드에서 레지스터로 직접 처리한다.
- 보드 동작(1초 주기 토글) 확인 필요.

## 하드웨어

- 보드: NUCLEO-G474RE
- LED: LD2 — PA5
