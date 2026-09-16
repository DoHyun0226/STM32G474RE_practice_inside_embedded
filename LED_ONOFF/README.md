# README


본 저장소는 유튜브(인사이드 임베디드)에 올라온 STM32 입문 강의를 Nucleo-G474RE를 이용하여 실습하는 용도이다. 
동영상 URL: https://www.youtube.com/watch?v=XOsyrZGZtR8&list=PLtoKvna7TPSI6-mNxULFJo3tpSDLaGQ44

## LED_ONOFF
 - LD2를 작동시키기 위해 GPIOA에 클럭을 인가, GPIOA의 PA5를 general purpose output mode로 설정, medium speed로 설정, 전원 인가함
 - RCC관련
   - GPIOA의 clock enable register는 AHB2를 사용 
   - RM p306 참고
 - MODER 관련
   - MODER
     - 00: Input mode
     - 01: General purpose output mode
     - 10: Alternate function mode
     - 11: Analog mode(reset state)
   - RM p355 참고
 - OSPEEDR
   - GPIO port output speed register
   - 속도 분류
     - 00: Low speed
     - 01: Medium speed
     - 10: High speed
     - 11: Very high speed
   - RM p356 참고
 - ODR
   - GPIO port output data register
   - RM p357 참고
<!-- 
양식 참고용: 내용 연관 없음
 - 해당 프로젝트는 작성자 본인이 STM32G474RE를 이용해 Baram 유튜브 영상을 따라 실습하는 용도이다. 
 - 해당 파일에는 작성자가 유용하다고 생각되는 정보 그냥 형식 없이 작성할꺼다.

 1. 000.h 파일에거 Ctrl + Tap 누르면 000.c로 넘어감
 2. 111.h에서 #include "000.h"에 커서가 있는 상태에서 F3 누르면 000.h 파일로 넘어감

 
 # STM32G474RE — USB CDC 진행 상황 (2026-09-14)

프로젝트: `D:\Programming\STM32\STM32G474RE` (바람/chcbaram "STM32 펌웨어 기초" 시리즈, 3~5편 USB CDC 구간)
보드: NUCLEO-G474RE

## 오늘 잡은 문제

**증상:** LED가 안 켜짐 → `bsp.c` `SystemClock_Config()`의 `HAL_RCC_OscConfig()` 실패 → `Error_Handler()` 무한루프

**원인:** HSE 설정이 보드 실물과 불일치

UM2505 Features: "24 MHz HSE **on-board oscillator**" (크리스탈 아님)

| 항목 | 설정값 (틀림) | 실제 보드 |
|---|---|---|
| HSE 모드 | Crystal/Ceramic Resonator (`RCC_HSE_ON`) | 오실레이터 → **BYPASS** |
| HSE 주파수 | 8 MHz | **24 MHz** |

두 가지가 겹쳐 실패:
1. 크리스탈 모드로 발진 대기 → `HSERDY` 안 뜸 → 타임아웃
2. 24 ÷2 ×85 = 1020 MHz → VCO 상한 344 MHz 초과 → PLL lock 실패

CubeMX가 "이상 없음"으로 통과시킨 이유: `HSE_VALUE=8000000`이라는 거짓 입력값 기준으로 계산. CubeMX는 보드에 실제로 뭐가 붙어 있는지 모름.

## 남은 작업 (다음에 이어서)

**1. CubeMX `.ioc`**
- Pinout & Configuration → System Core → RCC → High Speed Clock (HSE) → `BYPASS Clock Source`
  (제대로 되면 핀아웃에서 PF1/OSC_OUT이 회색으로 풀림)
- Clock Configuration → HSE Input frequency `8` → `24`
- Clock Configuration → PLLM `/2` → `/6`
- 확인: SYSCLK 170 / HCLK 170 / To USB 48 → `24 ÷6 = 4 ×85 = 340 ÷2 = 170`

**2. 생성된 코드를 손으로 옮기기** (빌드에 쓰이는 건 `src/bsp/` 쪽, `cube_g474/Core/`는 빌드 제외)
- `cube_g474/Core/Src/main.c`의 `SystemClock_Config()` → `src/bsp/bsp.c`
- `HSE_VALUE`를 `24000000UL`로 → `src/bsp/stm32g4xx_hal_conf.h` 118번 줄
  (안 고치면 `SystemCoreClock`이 56.67MHz로 계산되어 `HAL_Delay`가 3배 느려짐. 에러 없이 조용히 틀림)

**3. CRS 활성화** — `.ioc`에서 `To CRS`가 회색. HSI48은 RC라 USB 규격 오차에 아슬아슬하므로 CRS로 SOF 동기 보정 필요. 없으면 인식/끊김 반복하거나 보드가 따뜻해지면 끊김

**4. PA11/PA12 물리 배선** — Nucleo의 USB 단자는 ST-LINK 전용. 모르포 헤더에서 PA11(DM)/PA12(DP)/GND를 USB 브레이크아웃으로 빼야 함. VBUS는 불필요(ST-LINK 케이블로 급전)

## 같이 정리한 것들 (해결됨)

- `led.c` 테이블이 `{GPIOB, PIN_12}`인데 `ledToggle`은 PA5 하드코딩 → 테이블을 `{GPIOA, GPIO_PIN_5}`로 수정 완료
- 남은 개선: `ledToggle`이 아직 테이블 대신 PA5 하드코딩 (채널 인자 무시), Nucleo LD2는 active **HIGH**라 `on_state`/`off_state`가 반대
- `Error_Handler`의 `while(1)`을 주석 처리하면 안 됨 — `__disable_irq()`만 남아 인터럽트 죽은 채로 실행 계속 → `HAL_Delay()` 영구 정지. 복구 완료
- LQFP64에서는 PA11/PA12 리맵 불필요 (별개 핀으로 존재)

## 작업 시 주의

파일을 CubeIDE에서 열어둔 채로 외부에서 수정하면 **IDE가 자기 버퍼로 덮어씀.** 실제로 한 번 당함. 외부 수정 시 해당 파일을 닫거나 F5로 새로고침할 것. -->