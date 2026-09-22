//AHB   : 170MHz
//APB1  : 170MHz
//APB2  : 170MHz
#include "stm32g474xx.h"

void Initialize_MCU(void) /* initialize STM32G474RE MCU */
{
    // (1) 명령 캐시 및 데이터 캐시 설정
    // (2) ART 가속기, 프리페치 버퍼, 웨이트(4 waits) 사이클 설정
    FLASH->ACR |= 0x7UL<<8; // bit 10:8 DCEN, ICEN, PRFTEN set (리셋 시 ICEN은 이미 1)
    FLASH->ACR &= ~(0xFUL); // bit 3:0 LATENCY 초기화
    FLASH->ACR |= 0x4UL; // 4 waits cycle — boost 모드 136~170MHz 구간


// 캐시를 사용해 코드 실행속도 향상
//-------------------------------------------------------------



    // (3) HSE 및 PLL 설정(시스템 클록 SYSCLK = 170MHz)
    RCC->CR |= 0x00010100; // bit 16 HSEON, bit 8 HSION
    while((RCC->CR & 0x00000400) == 0); // wait until HSIRDY(bit 10) = 1

    RCC->CFGR &= ~(0x3UL); // SYSCLK = HSI
    RCC->CFGR |= 0x1UL;    // bit 1:0 SW = 01 (HSI16)
    while((RCC->CFGR & 0xCUL) != 0x4UL);// wait until SWS(bit 3:2) = 01, SYSCLK = HSI
    while(!(RCC->CR & (0x1UL<<17)));   // HSERDY — 크리스털 발진 안정까지 약 2ms

    //PLL 설정
    RCC->CR &= ~(0x1UL<<24);// PLL off (PLL 파라미터는 PLL이 꺼져 있어야 변경 가능)
    while(RCC->CR & (0x1UL<<25));   // PLLRDY==0 대기
    RCC->PLLCFGR &= ~(0x07007FF3UL); // PLLR/PLLREN/PLLN/PLLM/PLLSRC 필드 클리어
    RCC->PLLCFGR |= 0x01005553UL;    // PLLSRC=11(HSE), PLLM=5(/6), PLLN=85, PLLREN=1, PLLR=00(/2)
	// HSE 24MHz를 6으로 나눠서 4MHz를 만듦 (PLL 입력 허용 범위 2.66~16MHz)
	// 4MHz에 85를 곱해서 340MHz(VCO, 허용 96~344MHz) --> 다시 2로 나눠서 170MHz 생성
	// SYSCLK = HSE / PLLM * PLLN / PLLR = 24MHz / 6 * 85 / 2 = 170MHz
    RCC->CR |= 0x1UL<<24;// PLL on
    while(!(RCC->CR & (0x1UL<<25))); // wait until PLLRDY = 1




/*
 * HSE, HSI 모두 On
 * HSI 안정 확인 후 임시로 HSI를 SYSCLK로 사용
 * While문으로 SYSCLK 전환 완료 확인
 * HSE 안정 확인
 * HSE, HSI On 유지한 채로 PLL 설정
 * HSE를 PLL 입력으로 사용해 170MHz 생성 (SYSCLK 전환은 (5)에서)
 * PLL lock 대기
 */
//---------------------------------------------------------------------



    // (4) Boost 모드 설정 (F4/F7의 오버드라이브에 해당)
    // Range 1 normal 모드에서 STM32G474의 최대 속도는 150MHz
    // 170MHz를 위해서 boost 모드 설정 --> MCU 내부 전압 레귤레이터 출력을 1.2V에서 1.28V로 높임

    RCC->APB1ENR1 |= 0x1UL<<28; // PWR 주변장치 클록(PWREN = 1), PWR 레지스터에 읽기/쓰기가 가능해짐
    (void)RCC->APB1ENR1;            // 쓰기 완료 대기 (APB 브리지 지연)
    PWR->CR1 = (PWR->CR1 & ~(0x3UL << 9)) | (0x1UL << 9); // bit 10:9 VOS = 01, Range 1 (리셋 기본값)
    //                       ↑ 해당 필드만 0으로 밀고      ↑ 원하는 값 넣기
    PWR->CR5 &= ~(0x1UL<<8); // bit 8 R1MODE = 0 → boost mode (논리 반대 주의)
    // 전압 설정 완료 (170MHz 동작 준비). PLL은 켜졌지만 SYSCLK은 아직 HSI16 16MHz

/*
 * 170MHz 사용 위해선 내부 LDO 전압을 더 높여서 사용해야 함
 * 대신 소비 전류가 증가
 * boost 완료를 알리는 플래그가 없어 폴링 불가 — 아래 설정들이 시간을 벌어줌
 */

//--------------------------------------------------------

    // (5) 시스템/주변장치 클록 설정 (AHB = APB1 = APB2 = 170MHz)
    // G4는 APB1/APB2 모두 170MHz까지 가능하므로 분주하지 않음
    // bit 13:11 PPRE2 = 000 → APB2 = HCLK = 170MHz
    // bit 10:8  PPRE1 = 000 → APB1 = HCLK = 170MHz
    // bit  7:4  HPRE  = 0000 → AHB  = SYSCLK = 170MHz
    // bit  1:0  SW    = 11  → SYSCLK 소스를 PLL로 선택
    RCC->CFGR = (RCC->CFGR & ~0x3FFFUL) | 0x3UL;
    while((RCC->CFGR & 0xCUL) != 0xCUL);   // SWS(bit 3:2) = 11, PLL 전환 완료까지 대기
    // ↑ 여기서부터 실제로 CPU가 170MHz로 동작
    // CSS(Clock Security System) on — HSE 고장 시 HSI16 자동 전환 + NMI 발생
    RCC->CR |= 0x1UL<<19; // bit 19 CSSON = 1 (set-only, 리셋으로만 해제)




    // (6) SYSCFG 클록 — EXTI 설정 시 필요 (현재 미사용)
    RCC->APB2ENR |= 0x1UL; // bit 0 SYSCFGEN
}
