//AHB : 170MHz
//APB1 : 54MHz
//APB2 : 54Mhz
#include "stm32g474xx.h"

void Initialize_MCU(void) /* initialize STM32G474RE MCU */
{
    // (1) 명령 캐시 및 데이터 캐시 설정
    // (2) ART 가속기, 프리페치 버퍼, 웨이트(4 waits) 사이클 설정
    FLASH->ACR |= 0x7UL<<8; // DCEN, ICEN, PRFTEN set
    FLASH->ACR &= ~(0xFUL); // wait cycle 초기화.
    FLASH->ACR |= 0x4UL; // 4 waits cycle 설정

    // (3) HSE 및 PLL 설정(시스템 클록 SYSCLK = 170MHz)
    RCC->CR |= 0x00010100; // HSE on, HSI on
    while((RCC->CR & 0x00000400) == 0); // wait until HSIRDY = 1

    RCC->CFGR &= ~(0x3UL); // SYSCLK = HSI
    RCC->CFGR |= 0x1UL;
    while((RCC->CFGR & 0xCUL) != 0x4UL);// wait until SYSCLK = HSI

    //PLL 설정
    RCC->CR &= ~(0x1UL<<24);// PLL off
    while(RCC->CR & (0x1UL<<25));   // PLLRDY==0 대기
    RCC->PLLCFGR &= ~(0x07007FF3UL);
    RCC->PLLCFGR |= 0x01005553UL;
    //1001 0100 0000 0011 0110 0000 1000
	// HSE 24MHz를 6로 나눠서 4MHz를 만듬
	// 4MHz에 85을 곱해서 340MHz --> 다시 2로 나눠서 170MHz 클럭을 생성
	// SYSCLK = HSE*PLLM/PLLN/PLLR = 24MHz/6*85/2 = 170MHz
    RCC->CR |= 0x1UL<<24;// PLL off
    while((RCC->CR & 0x02000000) == 0); // wait until PLLRDY = 1

    // (4) 오버드라이브 설정
    // 오버드라이브를 사용하지 않으면 STM32F767의 최대 속도는 180MHz
    // 216MHz를 위해서 오버드라이브 설정 --> MCU 내부 전압 레귤레이터 출력을 높임

    RCC->APB1ENR |= 0x10000000; // 전원모듈 클록(PWREN = 1)
    PWR->CR1 |= 0x00010000; // over-drive enable(ODEN = 1)
    while((PWR->CSR1 & 0x00010000) == 0); // ODRDY = 1 ?
    PWR->CR1 |= 0x00020000; // over-drive switching enable(ODSWEN = 1)
    while((PWR->CSR1 & 0x00020000) == 0); // ODSRDY = 1 ?
    //CPU 216MHz 설정 완료

    // (5) 주변장치 클록 설정(APB1CLK = APB2CLK = 54MHz)
    RCC->CFGR = 0x3040B402; // SYSCLK = PLL, AHB = 216MHz, APB1 = APB2 = 54MHz
    RCC->DCKCFGR1 = 0x01000000; // TIMxCLK = 216MHz
    while((RCC->CFGR & 0x0000000C) != 0x00000008); // wait until SYSCLK = PLL
    RCC->CR |= 0x00080000; // CSS on

    // (6) I/O 보상 설정
    RCC->APB2ENR |= 0x00004000; // 주변장치 클록(SYSCFG = 1)
    SYSCFG->CMPCR = 0x00000001; // enable compensation cell
}
