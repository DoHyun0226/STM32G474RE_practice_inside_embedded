/*
 * bsp.h
 *
 *  Created on: 2026. 9. 10.
 *      Author: yoodo
 */
/*
 * bsp(Board Support Package): 보드를 사용할 수 있도록 하기 위한 Software 묶음
 *  - 운영체제를 로드하기 위한 최소한의 장치를 지원, 하드웨어 보드의 모든 장치를 위한 드라이버
 *  - bsp의 일반적 구성
 *   1. Bootloader: 시스템의 하드웨어를 초기화하고 운영체제의 이미지를 RAM에 올려주는 역할을 하는 시스템 프로그램. 하드웨어 의존성이 강함
 *   2. OAL(OEM Adaption Layer): HAL(Hardware Abstraction Layer) 아래에 위치하며 하드웨어 초기화 및 관리를 지원함
 *    * HAL이란 컴퓨터의 하드웨어와 소프트웨어 사이의 추상화 계층
 *   3. Device Driver: 특정 하드웨어나 장치를 제어하기 위한 커널의 일부분으로 동작하는 프로그램으로, 각각 장치 드라이버가 프로그램 되어 커널에 통합되어 실행됨
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_

#include "def.h" // 공통적으로 사용하는 common에 있는 헤더

#include "stm32g4xx_hal.h"



void bspInit(void);

void delay(uint32_t ms); // 밀리세컨드 단위로 지연시켜주는 함수
uint32_t millis(void); // 밀리세컨드 단위로 바뀌는 카운터 값이라고 보면 됨

void Error_Handler(void);

#endif /* SRC_BSP_BSP_H_ */
