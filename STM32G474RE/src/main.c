/*
 * main.c
 *
 *  Created on: 2026. 9. 9.
 *      Author: yoodo
 */


#include "main.h"


int main(void)
{
	hwInit(); // 하드웨어 초기화
	apInit(); // application 초기화, ap.h 참고

	apMain();

	return 0;

}
