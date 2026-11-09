# STM32

Nucleo-G474RE 보드로 STM32 레지스터/HAL을 실습하는 저장소이다. 각 폴더는 STM32CubeIDE 프로젝트 단위로 구성되어 있다.

- 보드: NUCLEO-G474RE (STM32G474RE, LQFP64)
- 참고 강의: 유튜브 "인사이드 임베디드" STM32 입문 강의
  (https://www.youtube.com/watch?v=XOsyrZGZtR8&list=PLtoKvna7TPSI6-mNxULFJo3tpSDLaGQ44)
- 레지스터 상세 비트 설명은 각 프로젝트 폴더의 소스/README 및 RM0440(Reference Manual) 참고

## 프로젝트 목록

| 폴더 | 내용 |
|---|---|
| [LED_ONOFF](LED_ONOFF) | GPIO 레지스터(RCC, MODER, OSPEEDR, ODR)를 직접 건드려 LD2 LED On/Off |
| [LED_ON_OFF_Res](LED_ON_OFF_Res) | GPIO/RCC 레지스터 주소를 `#define`과 구조체로 정의하는 연습 (`main()` 없이 정의만 있음) |
| [LED_Switch](LED_Switch) | 버튼(PC13) 입력을 IDR 폴링으로 읽어 LED(PA5) 제어 |
| [Clock](Clock) | HSE + PLL로 SYSCLK을 170MHz로 설정 (FLASH wait state, boost 모드 포함) |
| [LED_Switch_int](LED_Switch_int) | 버튼(PC13) EXTI 인터럽트로 LED(PA5) 토글 (SYSCFG EXTICR, EXTI, NVIC 레지스터 설정) |
| [ADC](ADC) | PA9(ADC5_IN2) 아날로그 전압을 ADC5 단일 변환(polling)으로 읽어 전압 환산 |
| [TimerCounter](TimerCounter) | TIM1 업데이트 인터럽트(PSC/ARR)로 1초마다 LED(PA5) 토글 |
| [UART](UART) | USART1(PC4/PC5, 9600bps) 레지스터 설정, 블루투스 모듈(BT04-A)로 PC와 에코 통신 |
| [UART_HAL](UART_HAL) | UART와 같은 설정을 CubeMX + HAL로 구현, A~Z a~z 반복 송신 (`usart.c`에 HAL 함수 위치·레지스터 대응 주석) |
| [STM32G474RE](STM32G474RE) | 바람(chcbaram) "STM32 펌웨어 기초" 시리즈 실습 (HAL 기반, USB CDC 진행 중) |

각 프로젝트의 세부 내용(레지스터 설명, 진행 상황, 남은 작업 등)은 해당 폴더의 `README.md`를 참고. `LED_ON_OFF_Res`와 `LED_Switch`는 README 없이 소스 주석에 설명이 있다.

이후 라인트레이서에 쓰는 실습(TCRT5000 2채널 ADC + DMA 등, HAL 기반)은 [Line_Tracer](https://github.com/DoHyun0226/Line_Tracer) 저장소의 `Project_code`에서 이어서 진행한다.
