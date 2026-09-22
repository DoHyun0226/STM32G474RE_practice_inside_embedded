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
| [LED_ON_OFF_Res](LED_ON_OFF_Res) | LED On/Off 실습 관련 프로젝트 |
| [LED_Switch](LED_Switch) | 스위치(버튼) 입력을 받아 LED 제어 |
| [Clock](Clock) | HSE + PLL로 SYSCLK을 170MHz로 설정 (FLASH wait state, boost 모드 포함) |
| [STM32G474RE](STM32G474RE) | 바람(chcbaram) "STM32 펌웨어 기초" 시리즈 실습 (HAL 기반, USB CDC 진행 중) |

각 프로젝트의 세부 내용(레지스터 설명, 진행 상황, 남은 작업 등)은 해당 폴더의 `README.md`를 참고.
