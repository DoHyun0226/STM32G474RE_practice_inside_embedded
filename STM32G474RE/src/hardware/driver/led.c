/*
 * led.c
 *
 *  Created on: 2026. 9. 13.
 *      Author: yoodo
 */


#include "led.h"

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t 	  pin;
	GPIO_PinState on_state;
	GPIO_PinState off_state;
} led_tbl_t; // LED가 여러개일 경우 일일이 지정할 수 없으므로 구조체를 사용

led_tbl_t led_tbl[LED_MAX_CH] = //여러개니까 당연히 LED_MAX_CH 수만큼 배열로 선언
{
		{GPIOA, GPIO_PIN_5, GPIO_PIN_RESET, GPIO_PIN_SET}, // ch0
		//{GPIOC, GPIO_PIN_10, GPIO_PIN_RESET, GPIO_PIN_SET}, //이런 방식으로 ch 추가 가능
};

bool ledInit(void) // 초기화 함수
{
	bool ret = true;

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	// GPIO_InitTypeDef 구조체의 인스턴스인 GPIO_InitStruct의 Pin, Mode, Pull, Speed, Alternate를 전부 0으로 설정


	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE(); //GIPO port A의 클럭이 enable
	//__HAL_RCC_GPIOC_CLK_ENABLE(); //GPIO port C 클럭 enable(사용할꺼면 주석 해제)


	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	//GPIOA: GPIO port A/GIPO_PIN_5: GIPO pin 5번/GPIO_PIN_SET: High로 설정해서 LED 끄끼(HIGH일 때 LED가 꺼지고 LOW일 때 켜짐)

	/*Configure GPIO pin : PA5 */
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;


	for (int i = 0; i < LED_MAX_CH; i++)
	{
		GPIO_InitStruct.Pin = led_tbl[i].pin;
		HAL_GPIO_Init(led_tbl[i].port, &GPIO_InitStruct);

		ledOff(i);
	}

	return ret;
}

void ledOn(uint8_t ch) // LED가 여러개일 수 있으므로 채널을 이용해 조정
{
	if (ch >= LED_MAX_CH) return; // 혹시 ch가 LED_MAX_CH보다 클 수 있으므로 에러 처리
	HAL_GPIO_WritePin(led_tbl[ch].port, led_tbl[ch].pin, led_tbl[ch].on_state);
	// 0일 때 켜지므로 GPIO_PIN_RESET 사용
	// GPIO_PIN_RESET 가보면 enum으로 되어있어서 0 지정되어 있고 GIPO_PIN_SET은 1로 지정되어 있음
	// 근데 궁금한게 이거 왜 1 on으로 안하냐... 헷갈리게...
}

void ledOff(uint8_t ch)
{
	if (ch >= LED_MAX_CH) return; // 혹시 ch가 LED_MAX_CH보다 클 수 있으므로 에러 처리
	HAL_GPIO_WritePin(led_tbl[ch].port, led_tbl[ch].pin, led_tbl[ch].off_state);
}

void ledToggle(uint8_t ch)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); //실행할 때마다 0,1을 바꿔주는 것
}
