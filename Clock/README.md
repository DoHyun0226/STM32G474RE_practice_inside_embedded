# README

본 프로젝트는 유튜브(인사이드 임베디드)에 올라온 STM32 입문 강의를 Nucleo-G474RE를 이용하여 실습하는 용도이다.
동영상 URL: https://www.youtube.com/watch?v=XOsyrZGZtR8&list=PLtoKvna7TPSI6-mNxULFJo3tpSDLaGQ44

## Clock

`Core/Src/main.c`의 `Initialize_MCU()`에서 HSE + PLL을 이용해 SYSCLK을 170MHz로 설정한다.
AHB = APB1 = APB2 = 170MHz (분주 없음).

### 진행 순서

1. **FLASH 설정** — ART 가속기(ICEN/DCEN/PRFTEN) 활성화, 4 wait state 설정 (170MHz 구간에서 필요)
2. **HSE/HSI 활성화** — HSE(보드 온보드 24MHz)와 HSI를 모두 켜고, 우선 HSI로 SYSCLK 전환(PLL 재설정을 위한 사전 조건), HSERDY 대기
3. **PLL 설정** — PLL을 끈 뒤 PLLCFGR 설정
   - PLLSRC = HSE, PLLM = /6, PLLN = ×85, PLLR = /2
   - SYSCLK = HSE / PLLM × PLLN / PLLR = 24MHz / 6 × 85 / 2 = **170MHz**
   - PLL 활성화 후 PLLRDY 대기
4. **Boost 모드 설정** — Range 1 normal 모드의 최대 속도는 150MHz이므로, 170MHz 동작을 위해 PWR 레지스터(VOS, R1MODE)로 boost 모드 설정 (F4/F7의 오버드라이브에 대응)
5. **SYSCLK → PLL 전환** — RCC->CFGR의 SW를 PLL로 설정하고 SWS로 전환 완료 대기, CSS(Clock Security System) 활성화
6. **SYSCFG 클록 인가** — 추후 EXTI 등에서 사용 예정(현재 미사용)

### 참고

- HSE는 크리스탈이 아니라 보드 온보드 24MHz 오실레이터 (UM2505 참고)
- PLL 입력 허용 범위: 2.66~16MHz, VCO 허용 범위: 96~344MHz
- 레지스터 상세 비트 필드는 `main.c` 주석 및 Reference Manual(RM0440) 참고
