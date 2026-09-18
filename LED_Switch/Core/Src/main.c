#include "stm32g474xx.h"

int main(void)
{
    // 1) GPIOA 클럭 활성화
    // RCC_AHB2ENR 레지스터의 비트 구조:
    // [31:27] Reserved [26]RNGEN [25] Reserved [24]AESEN [23:20] Reserved..... [0] GPIOAEN나머지는 RM p306 참고
	// Reset value: 0x0000 0000
    RCC->AHB2ENR |= 0x00000001;  // bit 0 = 1 (0000 0000 0000 0000 0000 0000 0000 0001), GPIOA clock enable
    RCC->AHB2ENR |= 0x00000004;	 // bit 2 = 1 (0000 0000 0000 0000 0000 0000 0000 0100), GPIOC clock enable

    // 2) PA5을 General-purpose output 모드로 설정 (MODER5[1:0] = 01)
    // Reset value: 0xABFF FFFF
    GPIOA->MODER  &= ~(0x3UL<<2*5);  // clear bits 11:10
    GPIOA->MODER  |=  (0x1UL<<10);  // set bit 10

    // 3) PA5 중간 속도 설정 (OSPEEDR5[1:0] = 01)
    // Reset value: 0x0C00 0000
    GPIOA->OSPEEDR &= ~(0x3UL<<2*5);  // clear bits 11:10
    GPIOA->OSPEEDR |=  (0x1UL<<10);  // set bit 10

    // 4) PC13를 입력으로 설정 (스위치)
    // Reset value: 0xFFFF FFFF
    // MODER13[27:26] = 00 (입력모드)
    GPIOC->MODER &= ~(0x3UL<<2*13);  // bit 27, 26 클리어 --> input mode

    while (1)
    {
        // IDR(Input Data Register) 레지스터 읽기: bit 13 확인
        // IDR = [31:16]Reserved [15]IDR15 ... [4]IDR4 [3]IDR3 ... [0]IDR0
        if ((GPIOC->IDR & (0x1UL<<13)) != 0)  // bit 13 체크
        {
            // 스위치 눌림 → LED ON
        	GPIOA->ODR |= (0x1UL<<5); // set bit 5
        }
        else
        {
            // 스위치 떼짐 → LED OFF
        	GPIOA->ODR &=  ~(0x1UL<<5); // reset bit 5
        }
    }
}

