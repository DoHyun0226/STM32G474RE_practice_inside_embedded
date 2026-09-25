# LED_Switch_int

User 버튼(B1, PC13)을 누르면 **EXTI 인터럽트**로 LD2 LED(PA5)를 토글하는 실습.
HAL 없이 레지스터를 직접 설정하며, 메인 루프는 비워 두고 모든 동작을 인터럽트 서비스 루틴(ISR)에서 처리한다.

모든 코드는 `Core/Src/main.c`에 있다.

## 동작 흐름

```
main()
 ├─ Initialize_MCU()    // SYSCLK 170MHz 설정 (Clock 프로젝트와 동일)
 ├─ GPIO_EXTI_Init()    // PA5 출력, PC13 입력, EXTI13 → NVIC 연결
 └─ while(1) { }        // 대기만 함

버튼 입력 (PC13 falling edge)
 └─ EXTI15_10_IRQHandler()
     ├─ PR1 bit13 펜딩 확인 및 클리어
     └─ GPIOA->ODR ^= (1<<5)  // LED 토글
```

## 1. 클록 설정 — `Initialize_MCU()`

[Clock](../Clock) 프로젝트와 동일하게 HSE(24MHz) + PLL로 SYSCLK = **170MHz**, AHB = APB1 = APB2 = 170MHz로 설정한다.
(FLASH 4 wait state, PLLM=/6, PLLN=×85, PLLR=/2, boost 모드, CSS 활성화 등 상세 내용은 Clock 프로젝트 README 참고)

마지막 단계에서 SYSCFG 클록(`RCC->APB2ENR` bit0)을 켜는데, 이번 실습에서 EXTI 라우팅에 실제로 사용된다.

## 2. GPIO & EXTI 설정 — `GPIO_EXTI_Init()`

| 순서 | 레지스터 | 설정 | 설명 |
|---|---|---|---|
| 1 | `RCC->AHB2ENR` | bit0, bit2 = 1 | GPIOA, GPIOC 클록 ON |
| 2 | `GPIOA->MODER` | MODER5 = 01 | PA5 General-purpose output |
| 3 | `GPIOA->OSPEEDR` | OSPEEDR5 = 01 | PA5 Medium speed |
| - | `GPIOA->ODR` | ODR5 = 0 | LED OFF로 초기화 |
| - | `GPIOC->MODER` | MODER13 = 00 | PC13 Input (버튼) |
| 4 | `RCC->APB2ENR` | SYSCFGEN = 1 | SYSCFG 클록 ON (EXTI 라우팅용) |
| 5 | `SYSCFG->EXTICR[3]` | EXTI13[7:4] = 0010 | EXTI13 소스를 Port C로 선택 |
| 6 | `EXTI->IMR1` | IM13 = 1 | Line 13 인터럽트 마스크 해제 |
| 6 | `EXTI->FTSR1` | FT13 = 1 | Line 13 falling edge 트리거 |
| 6 | `EXTI->PR1` | bit13 | 펜딩 비트 클리어 |
| 7 | NVIC | `NVIC_EnableIRQ(EXTI15_10_IRQn)` | EXTI10~15 공용 IRQ 활성화 |

### 참고 포인트

- **EXTICR 인덱스**: EXTI 라인 4개당 EXTICR 레지스터 1개를 사용한다. EXTI13은 `EXTICR4`의 bit 7:4이며, CMSIS 구조체에서는 0부터 시작하므로 `SYSCFG->EXTICR[3]`이 된다.
- **포트 코드**: PA=0000, PB=0001, PC=0010 … → PC13은 `0010` (`0x1UL<<5`).
- **IRQ 공유**: EXTI10~15 라인은 하나의 인터럽트 벡터(`EXTI15_10_IRQn`)를 공유하므로, 핸들러 안에서 `PR1`을 확인해 어떤 라인이 발생했는지 구분해야 한다.

## 3. 인터럽트 서비스 루틴 — `EXTI15_10_IRQHandler()`

- 핸들러 이름은 startup 파일(`startup_stm32g474retx.s`)의 벡터 테이블 심볼과 동일해야 한다 (weak 심볼을 main.c에서 재정의).
- `EXTI->PR1` bit13으로 펜딩을 확인하고 클리어한 뒤 `GPIOA->ODR`을 XOR 해서 LED를 토글한다.
- 핸들러에서 긴 지연(delay)을 쓰는 대신 토글 방식으로 짧게 처리하도록 개선한 상태.

## 개선할 점 / 주의사항

- **PR1 클리어 방식**: PR1은 `rc_w1`(1을 써서 클리어) 레지스터이다. 현재 `EXTI->PR1 &= 1<<13`도 결과적으로 bit13에 1을 쓰게 되어 동작은 하지만, 의도를 명확히 하려면 `EXTI->PR1 = 1UL<<13;` 처럼 직접 대입하는 것이 일반적이다.
- **채터링**: 디바운싱 처리가 없어 버튼을 한 번 눌러도 여러 번 인터럽트가 발생해 LED가 의도와 다르게 토글될 수 있다 (타이머/SysTick 기반 디바운싱으로 개선 가능).
- `LED_Switch_int.ioc`는 CubeMX 기본값 상태이며 실제 핀 설정은 코드에서 레지스터로 직접 처리한다.

## 하드웨어

- 보드: NUCLEO-G474RE
- LED: LD2 — PA5
- 스위치: B1 (USER) — PC13
