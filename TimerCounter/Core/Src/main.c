/*  ‣ 목표 : TIM1 을 1 초 주기로 오버플로우시켜 PA5(LED)을 토글한다.
 *  ‣ 시스템 클록 : 170 MHz (Initialize_MCU()로 이미 설정됨)
 *  ‣ TIM1 클록   : 170 MHz  (APB2 프리스케일러 = 1 → PCLK2 그대로 사용)
 *                  └ prescaler = 17 000-1  → 10 kHz 카운터
 *                    auto-reload = 10 000-1 → 1 s 주기
 */
#include "stm32g474xx.h"

void Initialize_MCU(void) /* initialize STM32G474RE MCU */
{
    // (1) 명령 캐시 및 데이터 캐시 설정
    // (2) ART 가속기, 프리페치 버퍼, 웨이트(4 waits) 사이클 설정
    FLASH->ACR |= 0x7UL<<8; // bit 10:8 DCEN, ICEN, PRFTEN set (리셋값 0x0004_0600 — ICEN, DCEN은 이미 1이라 실제로 켜지는 건 PRFTEN)
    /*
     * PRFTEN(bit 8) : 프리페치 버퍼. CPU가 지금 명령을 실행하는 동안 다음 명령을 미리 읽어 둔다.
     * ICEN  (bit 9) : 명령어 캐시. 반복문처럼 자주 실행하는 명령을 캐시에 보관해서 플래시를 다시 읽지 않는다.
     * DCEN  (bit 10): 데이터 캐시. 플래시에 있는 상수 데이터(const 테이블 등)를 캐시에 보관한다.
     * 이 세 기능을 묶어 ST는 "ART 가속기"라고 부른다. 플래시는 170MHz를 따라가지 못해서
     * 아래처럼 대기 사이클(LATENCY)을 넣어야 하는데, 그만큼 생기는 손실을 이 기능들이 대부분 메워 준다.
     */
    FLASH->ACR &= ~(0xFUL); // bit 3:0 LATENCY 초기화
    FLASH->ACR |= 0x4UL; // 4 waits cycle — boost 모드 136~170MHz 구간


// 캐시를 사용해 코드 실행속도 향상
//-------------------------------------------------------------



    // (3) HSE 및 PLL 설정(시스템 클록 SYSCLK = 170MHz)
    RCC->CR |= 0x00010100; // bit 16 HSEON, bit 8 HSION
    while((RCC->CR & 0x00000400) == 0); // wait until HSIRDY(bit 10) = 1

    RCC->CFGR &= ~(0x3UL); // SW 필드 클리어 (이 순간 SW = 00, G4에서는 reserved 값)
    RCC->CFGR |= 0x1UL;    // bit 1:0 SW = 01 (HSI16)
    while((RCC->CFGR & 0xCUL) != 0x4UL);// wait until SWS(bit 3:2) = 01, SYSCLK = HSI
    while(!(RCC->CR & (0x1UL<<17)));   // HSERDY(bit 17) — HSE(온보드 24MHz 오실레이터) 안정 대기. 오실레이터면 원칙상 HSEBYP=1 (현재 미설정)

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
 * Range 1 안에서 R1MODE만 바꿀 때는 완료 플래그가 없어 폴링 불가
 * 아래 설정들이 시간을 벌어주긴 하지만 보장된 것은 아님 —
 * RM0440 절차는 AHB ÷2 상태로 PLL 전환 후 1us 이상 대기, 그다음 ÷1로 복귀
 */

//--------------------------------------------------------

    // (5) 시스템/주변장치 클록 설정 (AHB = APB1 = APB2 = 170MHz)
    // G4는 APB1/APB2 모두 170MHz까지 가능하므로 분주하지 않음
    // bit 13:11 PPRE2 = 000 → APB2 = HCLK = 170MHz
    // bit 10:8  PPRE1 = 000 → APB1 = HCLK = 170MHz
    // bit  7:4  HPRE  = 0000 → AHB  = SYSCLK = 170MHz
    // bit  1:0  SW    = 11  → SYSCLK 소스를 PLL로 선택
    RCC->CFGR = (RCC->CFGR & ~0x3FFFUL) | 0x3UL; // SWS; System clock switch status
    /*
     * 00: Reserved, must be kept at reset value
     * 01: HSI16 oscillator used as system clock
     * 10: HSE used as system clock
     * 11: PLL used as system clock
     */
    while((RCC->CFGR & 0xCUL) != 0xCUL);   // SWS(bit 3:2) = 11, PLL 전환 완료까지 대기
    // ↑ 여기서부터 실제로 CPU가 170MHz로 동작
    // CSS(Clock Security System) on — HSE 고장 시 HSI16 자동 전환 + NMI 발생
    RCC->CR |= 0x1UL<<19; // bit 19 CSSON = 1 (set-only, 리셋으로만 해제)




    // (6) SYSCFG 클록 — EXTI 설정 시 필요 (현재 미사용)
    RCC->APB2ENR |= 0x1UL; // bit 0 SYSCFGEN
}
/* ───────────────────────── 타이머 1 오버플로우 인터럽트 ───────────────────────── */
void TIM1_UP_TIM16_IRQHandler(void)
{
    if (TIM1->SR & 0x1UL)                // UIF(Update Interrupt Flag) 확인
    {
        TIM1->SR &= ~(0x1UL);              // UIF 클리어
        GPIOA->ODR ^= 0x1UL<<5;  // LED 토글
    }
}

/* ─────────────────────────────────────────────────────────────────────────────── */
int main(void)
{
/* 0) 시스템 클록·캐시 초기화 (170 MHz) */
    Initialize_MCU();


/* 1) GPIOA 클록 활성화 */
    RCC->AHB2ENR |= 0x00000001;

 //2) PA5을 General-purpose output 모드로 설정
    GPIOA->MODER &= ~(0x3UL << 10);
    GPIOA->MODER |= 0x1UL << 10;
     //PA5 중간 속도 설정
    GPIOA->OSPEEDR &= ~(0x3UL << 10);
    GPIOA->OSPEEDR |= 0x1UL << 10; // OSPEED5 = 01 --> medium speed

/* 3) TIM1 클록 활성화 (RCC_APB2ENR bit11 = TIM1EN) */
    RCC->APB2ENR |= 0x1UL << 11;

/* 4) 타이머 기본 설정 ---------------------------------------------------------
 *    PSC  = 17 000-1   → 170 MHz / 17 000 = 10 kHz
 *    ARR  = 10 000-1   → 10 kHz / 10 000 = 1 Hz (1 초)
 * ---------------------------------------------------------------------------*/
    TIM1->PSC = (uint16_t)(17000 - 1);                // Prescaler
    // (counter clock frequency) = (frequency that is supplied to the prescaler of a timer) / (PSC[15:0] + 1)
    TIM1->ARR = 10000 - 1;                // Auto-reload
    TIM1->EGR = (uint16_t)1;                   // UG = 1 (즉시 로드)

/* 5) 인터럽트 허용 : DIER.UIE = 1, NVIC Enable */
    TIM1->DIER |= 0x0001;                 // Update interrupt enable
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);   // CMSIS 함수

/* 6) 타이머 카운터 시작 : CR1.CEN = 1 */
    TIM1->CR1 |= (uint16_t)1;

/* 7) 메인 루프 – CPU는 슬립 / 다른 작업 수행 가능 */
    while (1)
    {

    }
}
