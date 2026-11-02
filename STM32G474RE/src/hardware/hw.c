/*
 * hw.c
 *
 *  Created on: 2026. 9. 10.
 *      Author: yoodo
 */


#include "hw.h"



void hwInit(void)
{
	bspInit();
	// 나중에 드라이버들이 있으면 여기에 초기화 코드를 넣을 예정

	ledInit(); //LED 관련 설정 초기화
}
