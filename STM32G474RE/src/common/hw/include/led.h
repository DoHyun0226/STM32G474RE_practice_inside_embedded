/*
 * led.h
 *
 *  Created on: 2026. 9. 13.
 *      Author: yoodo
 */

#ifndef SRC_COMMON_HW_INCLUDE_LED_H_
#define SRC_COMMON_HW_INCLUDE_LED_H_

#include "hw_def.h"

#ifdef _USE_HW_LED // LED를 사용하지 않을 수도 있으므로

#define LED_MAX_CH	HW_LED_MAX_CH
// hw_def.h에서 HW_LED_MAX_CH가 define 되어 있음
// LED 개수 정의

bool ledInit(void); // 초기화 함수
void ledOn(uint8_t ch); // LED가 여러개일 수 있으므로 채널을 이용해 조정
void ledOff(uint8_t ch);
void ledToggle(uint8_t ch);

#endif /* _USE_HW_LED */

#endif /* SRC_COMMON_HW_INCLUDE_LED_H_ */
