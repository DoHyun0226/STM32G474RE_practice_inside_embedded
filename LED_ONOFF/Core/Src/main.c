#include "stm32g474xx.h" //CMSIS 디바이스 헤더

int main(void)
{
	// 주변장치를 사용할 떄 가장 먼저 해야하는 것은 clock 설정
	RCC->AHB2ENR |= 0x00000001; //GPIOAEN: GPIOA enable
}
