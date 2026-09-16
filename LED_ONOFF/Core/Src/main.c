#include "stm32g474xx.h" //CMSIS 디바이스 헤더

uint8_t Test = 0; // 8bit unsigned int

int main(void)
{
	// 주변장치를 사용할 떄 가장 먼저 해야하는 것은 clock 설정
	//GPIOAEN: GPIOA enable
	RCC->AHB2ENR |= 0x00000001;
	// RM p306 참고
	// RCC: Reset and Clock Control

	GPIOA->MODER &= ~(3UL << 10); // 내가 원하는 부분인 bit 11:10만 00으로 초기화. 나머지는 유지
	GPIOA->MODER |= 1UL << 10; // bit 11:10 에 내가 원하는 값 입력
	// GPIOA의 PA5를 general purpose output mode로 설정
	// 01: general purpose output mode(RM p355 참고)
	// default GPIOA_MODER: 0xABFF FFFF

	GPIOA->OSPEEDR &= ~(3UL << 10);
	GPIOA->OSPEEDR |= 1UL << 10; // OSPEED5 = 01 --> medium speed

	while(1)
	{
		if(Test == 0)
		{
			GPIOA->ODR &= ~(1UL << 5); // // OD5출력을 0로 설정-->LED off
		}
		else
		{
		GPIOA->ODR |= 0x00000020; // OD5출력을 1로 설정-->LED on
		}
	}
}
