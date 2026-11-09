# UART_HAL

[UART](../UART) 프로젝트와 같은 USART1(PC4/PC5, 9600 8N1, BT04-A 블루투스 모듈) 설정을 **CubeMX + HAL**로 만든 버전.
`A`~`Z`, `a`~`z`를 0.1초마다 한 글자씩 반복해서 보낸다.

## 파일 구성

| 파일 | 내용 |
|---|---|
| `Core/Src/main.c` | `HAL_Init()` → `SystemClock_Config()`(HSE 24MHz, PLL 170MHz) → `MX_GPIO_Init()` → `MX_USART1_UART_Init()` → 송신 루프 |
| `Core/Src/usart.c` | `huart1` 핸들, `MX_USART1_UART_Init()`, `HAL_UART_MspInit()`. **함수별 용도, HAL 원본 위치(파일·줄 번호), 레지스터 버전과의 대응을 주석으로 정리해 둠** |

`usart.c`에는 USER CODE 영역 밖에도 학습용 주석이 있다. CubeMX로 다시 생성하면 지워지므로 이 프로젝트는 재생성하지 않는다.

## CubeMX 설정

- **RCC:** HSE = Crystal/Ceramic Resonator
- **SYS:** Debug = Serial Wire (PA13/PA14 예약)
- **Clock:** PLL Source = HSE, SYSCLK = PLLCLK, HCLK = 170MHz
- **USART1:** Asynchronous, PC4(TX)/PC5(RX), 9600 bps, 8 bit, None, 1 stop, 흐름 제어 없음, FIFO 비활성

## 호출 흐름과 레지스터 버전 비교

```
MX_USART1_UART_Init()            // huart1.Init에 설정값 채우기
 └ HAL_UART_Init(&huart1)        // stm32g4xx_hal_uart.c
     ├ HAL_UART_MspInit()        // usart.c: 클록, 핀
     └ UART_SetConfig()          // BRR, CR1~CR3 쓰기
```

| 레지스터 버전 (`UART/Core/Src/main.c`) | HAL에서 하는 곳 |
|---|---|
| `RCC->APB2ENR` USART1 클록 ON | `HAL_UART_MspInit()` → `__HAL_RCC_USART1_CLK_ENABLE()` |
| `RCC->AHB2ENR` GPIOC 클록 ON | `HAL_UART_MspInit()` → `__HAL_RCC_GPIOC_CLK_ENABLE()` |
| `GPIOC->MODER`, `AFR[0]` (AF7) | `HAL_UART_MspInit()` → `HAL_GPIO_Init()` |
| `USART1->BRR = 17708`, `CR1` TE/RE/UE | `HAL_UART_Init()` → `UART_SetConfig()` (BRR은 PCLK2 주파수로 자동 계산) |
| TXE 대기 → `TDR` 쓰기 | `HAL_UART_Transmit()` |

MSP = MCU Support Package. 주변장치를 쓰기 전 칩 쪽 준비(클록, 핀, NVIC)를 맡는 함수로, HAL 안의 `__weak` 기본 함수를 `usart.c`의 같은 이름 함수가 대신한다.

## 송신 코드

```c
HAL_UART_Transmit(&huart1, &i, 1, 100);  // 핸들 주소, 데이터 주소, 개수, 타임아웃(ms)
```

- `pData`는 포인터라 일반 변수는 `&i`처럼 주소로 넘긴다. 배열은 이름 자체가 주소다.
- `Size`는 보낼 데이터 개수. 한 데이터가 몇 비트인지는 `huart1.Init`에 이미 있다. 문자열 길이는 `sizeof` 대신 `strlen`을 쓴다(`sizeof`는 끝의 `'\0'`까지 셈).
- 9600bps에서 1바이트(시작 1 + 데이터 8 + 정지 1 = 10비트)는 약 1.04ms 걸린다. 타임아웃은 이보다 넉넉하게 잡는다.

## 실험 기록

- 영상: [STM32 UART Bluetooth Transmit Test (HAL, NUCLEO-G474RE)](https://www.youtube.com/watch?v=bu-3NrXJ_uU)

레지스터로 만든 버전은 [UART](../UART) 참고.

## 참고

자주 쓰는 HAL 함수 정리(함수 원형, 레지스터 대응, 콜백)는 별도 PDF로 정리해 두었다(저장소 밖, `05_MCU_자료/STM/STM32G4_HAL_함수_정리.pdf`).
