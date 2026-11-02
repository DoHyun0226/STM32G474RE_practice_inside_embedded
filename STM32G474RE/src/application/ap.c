/*
 * ap.c
 *
 *  Created on: 2026. 9. 10.
 *      Author: yoodo
 */


#include "ap.h"



extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

void apInit(void)
{

}
/*
 * 최상위에서 사용하는 코드에서 제조사의 api를 직접 사용하고 있음
 * 기능에는 문제가 없지만 만약 MCU가 AVR 등으로 바뀌면 위 API를 전부 수정해야함
 * 그러므로 제조사 API를 직접 호출하지 않고 led를 제어하는 별도의 api를 만들어 변경할 예정
 */
// -->완료

void apMain(void) // main.c를 최소화하기 위해 apMain을 통해 일반적인 작업
{
	while(1) // 이건 메인이기 때문에 빠져나오면 안됨
	{
		ledToggle(_DEF_LED1);
		// ledToggle 관련 함수들을 전부 led.c로 이동시켰기 때문에 mcu가 바뀌어도 여기를 수정할 필요가 없어짐
		delay(500); //0.5초 지연

		CDC_Transmit_FS("test\n", 6);
	}
}
