/*
 * def.h
 *
 *  Created on: 2026. 9. 10.
 *      Author: yoodo
 */
// 모든 소스파일이 공통적으로 사용하는 파일
// 공통적으로 사용해야할 헤더가 있다면 def.h에 추가/이거 하나만 수정하면 되니까

#ifndef SRC_COMMON_DEF_H_
#define SRC_COMMON_DEF_H_

#include <stdio.h>
#include <stdint.h> //Fixed-width integer types/uint32_t 등이 들어있음
#include <stdbool.h>

#define _DEF_LED1	0
#define _DEF_LED2	1
#define _DEF_LED3	2
#define _DEF_LED4	3
#endif /* SRC_COMMON_DEF_H_ */
