/*
 * hw_def.h
 *
 *  Created on: 2026. 9. 10.
 *      Author: yoodo
 */
// 하드웨어 정의하는 헤더
// hardware 폴더 아래 만드는 파일들이 공통으로 사용할 하드웨어 관련된 정의를 추가한 파일


#ifndef SRC_HARDWARE_HW_DEF_H_
#define SRC_HARDWARE_HW_DEF_H_

#include "def.h"
#include "bsp.h"

#define _USE_HW_LED //이 프로젝트에서는 LED를 사용할 것이므로 선언

#define HW_LED_MAX_CH	1 // 제어할 LED를 1개로 설정

#endif /* SRC_HARDWARE_HW_DEF_H_ */
