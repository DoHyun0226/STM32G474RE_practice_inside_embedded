/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/*
 * ─────────────────────────────────────────────────────────────────────────────
 * [학습용 주석] 이 파일 전체 구조
 * ─────────────────────────────────────────────────────────────────────────────
 * CubeMX가 USART1 설정(9600bps, 8N1, PC4=TX, PC5=RX)을 바탕으로 생성한 파일.
 * CubeMX로 다시 생성하지 않을 예정이라 USER CODE 영역 밖에도 주석을 적어 둠.
 * (다시 생성하면 USER CODE 영역 밖의 주석은 지워짐)
 *
 * 호출 흐름:
 *   main()                              Core/Src/main.c
 *    └ MX_USART1_UART_Init()            ← 이 파일: "어떤 UART를 어떻게 쓸지" 값 채우기
 *        └ HAL_UART_Init(&huart1)       ← HAL 라이브러리: 공통 초기화 절차
 *            ├ HAL_UART_MspInit()       ← 이 파일: 칩 쪽 준비(클록, 핀)
 *            └ UART_SetConfig()         ← HAL 라이브러리: BRR, CR1~CR3 레지스터 쓰기
 *
 * 레지스터 버전(D:\Programming\STM32\UART\Core\Src\main.c의 UART1_Init)과 비교:
 *   1. RCC->APB2ENR USART1 클록 ON      → HAL_UART_MspInit()의 __HAL_RCC_USART1_CLK_ENABLE()
 *   2. RCC->AHB2ENR GPIOC 클록 ON       → HAL_UART_MspInit()의 __HAL_RCC_GPIOC_CLK_ENABLE()
 *   3~4. PC4/PC5 MODER(AF), AFR(AF7)    → HAL_UART_MspInit()의 HAL_GPIO_Init()
 *   5. BRR = 17708, CR1 TE/RE/UE        → HAL_UART_Init() → UART_SetConfig()
 *
 * 함수 원본 찾는 법:
 *   CubeIDE에서 함수/매크로 이름을 Ctrl + 클릭하면 정의로 이동.
 *   아래 주석의 경로는 UART_HAL 프로젝트 폴더 기준, 줄 번호는 현재 포함된 HAL 버전 기준.
 * ─────────────────────────────────────────────────────────────────────────────
 */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
/*
 * usart.h (Core/Inc/usart.h)
 *  - main.h를 include → 그 안에서 stm32g4xx_hal.h(HAL 전체)가 들어옴
 *  - extern UART_HandleTypeDef huart1; 선언 → main.c 등 다른 파일에서도 huart1 사용 가능
 *  - MX_USART1_UART_Init() 함수 원형 선언
 */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
/*
 * huart1: USART1을 다루는 "핸들" 구조체 (전역 변수)
 *  - HAL UART 함수는 전부 이 핸들의 주소(&huart1)를 첫 인자로 받음
 *  - 들어 있는 것: 어느 UART인지(Instance), 설정값(Init), 현재 상태(gState/RxState),
 *    송수신 버퍼 포인터, 에러 코드 등
 *  - 정의 위치: Drivers/STM32G4xx_HAL_Driver/Inc/stm32g4xx_hal_uart.h
 *      UART_HandleTypeDef : 217~287번째 줄
 *      UART_InitTypeDef   : 95번째 줄에서 끝나는 구조체 (BaudRate, WordLength 등)
 */

/* USART1 init function */

/*
 * MX_USART1_UART_Init()
 *  - 용도: huart1에 "USART1을 9600bps, 8N1로 쓰겠다"는 설정값을 채우고 HAL_UART_Init() 호출
 *  - 호출 위치: Core/Src/main.c의 main()에서 MX_GPIO_Init() 다음
 *  - MX_ 접두사 = CubeMX가 생성한 함수라는 표시
 *  - 여기서는 아직 레지스터를 직접 건드리지 않음. 실제 레지스터 쓰기는 HAL_UART_Init() 안에서 일어남
 */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  /* USART1 = ((USART_TypeDef *) USART1_BASE) = 0x4001_3800 주소의 레지스터 묶음
     (정의: Drivers/CMSIS/Device/ST/STM32G4xx/Include/stm32g474xx.h, RCC와 같은 방식) */
  huart1.Init.BaudRate = 9600;                          // 통신 속도. BRR 값은 HAL이 클록에서 자동 계산
  huart1.Init.WordLength = UART_WORDLENGTH_8B;          // 데이터 8비트 (패리티 포함 길이)
  huart1.Init.StopBits = UART_STOPBITS_1;               // 정지 비트 1개
  huart1.Init.Parity = UART_PARITY_NONE;                // 패리티 없음 → 여기까지가 "8N1"
  huart1.Init.Mode = UART_MODE_TX_RX;                   // 송신+수신 모두 사용 (CR1 TE, RE)
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;          // RTS/CTS 하드웨어 흐름 제어 안 씀
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;      // 16배 오버샘플링 (CR1 OVER8 = 0)
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE; // 비트당 3번 샘플링해 다수결 (노이즈에 강함)
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;     // USART 입력 클록 분주 없음 (170MHz 그대로)
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; // TX/RX 핀 반전, 스왑 등 고급 기능 안 씀
  if (HAL_UART_Init(&huart1) != HAL_OK)
  /*
   * HAL_UART_Init()
   *  - 위치: Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c 307번째 줄
   *  - 하는 일(순서대로):
   *      1) 처음 초기화하는 핸들이면(gState == RESET) HAL_UART_MspInit() 호출 → 클록, 핀 준비
   *      2) UART 끄기 (CR1 UE = 0, 설정 레지스터는 UE = 0일 때만 바꿀 수 있음)
   *      3) UART_SetConfig() (같은 파일 3021번째 줄)
   *         - CR1: 워드 길이, 패리티, TE/RE, OVER8
   *         - CR2: 정지 비트 / CR3: 흐름 제어, 1비트 샘플링 / PRESC: 분주
   *         - BRR: HAL_RCC_GetPCLK2Freq()로 클록(170MHz)을 읽어 170MHz/9600 ≈ 17708 계산
   *      4) 비동기 모드에 필요 없는 비트(LIN, 스마트카드, 반이중 등) 정리
   *      5) UART 켜기 (CR1 UE = 1)
   *      6) UART_CheckIdleState() (3312번째 줄): TE/RE가 실제로 켜질 때까지 대기
   *  - 반환값: 성공 HAL_OK, 실패 HAL_ERROR 등
   */
  {
    Error_Handler();
    /* Error_Handler(): Core/Src/main.c 159번째 줄. 인터럽트를 끄고 무한 루프에서 멈춤.
       디버깅 중 여기서 멈춰 있으면 초기화가 실패했다는 뜻 */
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  /*
   * HAL_UARTEx_SetTxFifoThreshold()
   *  - 위치: Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c 605번째 줄
   *  - Ex = Extended. G4처럼 일부 칩에만 있는 확장 기능 함수라는 뜻
   *  - G474 USART에는 송신/수신 각 8바이트짜리 FIFO가 있음
   *  - 송신 FIFO가 1/8(1바이트) 이하로 비었을 때 플래그(TXFT)를 띄우라는 기준값 설정 (CR3 TXFTCFG)
   *  - 아래에서 FIFO를 끄기 때문에 지금은 실제로 쓰이지 않음. CubeMX가 기본으로 넣는 코드
   */
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  /*
   * HAL_UARTEx_SetRxFifoThreshold()
   *  - 위치: stm32g4xx_hal_uart_ex.c 654번째 줄
   *  - 수신 FIFO 기준값 설정 (CR3 RXFTCFG). 위와 마찬가지로 FIFO를 끄면 의미 없음
   */
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  /*
   * HAL_UARTEx_DisableFifoMode()
   *  - 위치: stm32g4xx_hal_uart_ex.c 559번째 줄
   *  - FIFO 끄기 (CR1 FIFOEN = 0) → TDR/RDR 1바이트 + 시프트 레지스터 구조로 동작
   *  - 레지스터 버전에서 CR1 = 0으로 FIFO를 꺼 둔 것과 같은 상태
   *  - CubeMX USART1 설정의 "Fifo Mode: Disable"이 이 줄로 생성됨
   */
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/*
 * HAL_UART_MspInit()
 *  - MSP = MCU Support Package. 주변장치를 쓰기 전 "칩 쪽 준비"(클록, 핀, 인터럽트)를 맡는 함수
 *  - 직접 호출하지 않음. HAL_UART_Init()이 내부에서 자동으로 호출
 *  - HAL 라이브러리 안에도 같은 이름의 빈 함수가 __weak로 있음
 *      (stm32g4xx_hal_uart.c 679번째 줄)
 *    → 이 파일에 같은 이름의 함수가 있으면 링커가 이쪽을 대신 사용
 *  - 모든 UART가 이 함수 하나를 공유하므로, Instance로 어느 UART인지 구분해서 처리
 *  - 인터럽트(NVIC)를 켜면 이 함수 안에 HAL_NVIC_SetPriority/EnableIRQ가 추가로 생성됨
 */
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};             // 핀 설정값을 담는 구조체 (모든 멤버 0으로 시작)
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};       // 주변장치 클록 소스 선택값을 담는 구조체
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    /*
     * HAL_RCCEx_PeriphCLKConfig()
     *  - 위치: Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c 123번째 줄
     *  - USART1에 들어갈 클록 소스 선택: RCC->CCIPR의 USART1SEL = 00 (PCLK2 = APB2 = 170MHz)
     *  - 선택지: PCLK2, SYSCLK, HSI16, LSE. 리셋 기본값도 PCLK2라서
     *    레지스터 버전에서는 이 설정을 생략해도 동작했음
     */
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
    /*
     * __HAL_RCC_USART1_CLK_ENABLE()  (함수가 아니라 매크로)
     *  - 위치: Drivers/STM32G4xx_HAL_Driver/Inc/stm32g4xx_hal_rcc.h 1131번째 줄
     *  - 내용: SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN) 후 같은 레지스터를 한 번 다시 읽음
     *    (다시 읽기 = 클록이 실제로 켜질 때까지 잠깐 기다리는 효과. 레지스터 버전의
     *     (void)RCC->APB1ENR1; 과 같은 역할)
     *  - 레지스터 버전: RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
     */

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /*
     * __HAL_RCC_GPIOC_CLK_ENABLE()  (매크로)
     *  - 위치: stm32g4xx_hal_rcc.h 614번째 줄
     *  - 내용: SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_GPIOCEN) + 다시 읽기
     *  - 레지스터 버전: RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
     */

    /**USART1 GPIO Configuration
    PC4     ------> USART1_TX
    PC5     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;      // PC4, PC5 두 핀을 한 번에 설정
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;           // AF(Alternate Function) + 푸시풀 출력 → MODER = 10b
    GPIO_InitStruct.Pull = GPIO_NOPULL;               // 내부 풀업/풀다운 없음 (PUPDR = 00b)
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;      // 출력 속도 Low (OSPEEDR). 9600bps에는 충분
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;      // AF7 = USART1 (AFR[0]). 정의: stm32g4xx_hal_gpio_ex.h 174번째 줄
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    /*
     * HAL_GPIO_Init()
     *  - 위치: Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c 162번째 줄
     *  - Pin에 표시된 핀마다 반복하면서 MODER, OTYPER, OSPEEDR, PUPDR, AFR 레지스터를 씀
     *  - 레지스터 버전의 GPIOC->MODER, GPIOC->AFR[0] 두 줄이 하는 일 + 속도/풀업 설정까지 포함
     */

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

/*
 * HAL_UART_MspDeInit()
 *  - MspInit의 반대: UART 사용을 끝낼 때 클록을 끄고 핀을 리셋 상태로 되돌림
 *  - HAL_UART_DeInit(&huart1) (stm32g4xx_hal_uart.c 632번째 줄)을 호출하면 내부에서 자동 호출
 *  - 지금 코드에서는 DeInit을 부르지 않으므로 실행되지 않음
 *  - __weak 기본 함수: stm32g4xx_hal_uart.c 694번째 줄
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
    /* 매크로. stm32g4xx_hal_rcc.h 1209번째 줄: CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN) */

    /**USART1 GPIO Configuration
    PC4     ------> USART1_TX
    PC5     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4|GPIO_PIN_5);
    /* stm32g4xx_hal_gpio.c 293번째 줄. PC4, PC5를 리셋 상태(아날로그 모드)로 되돌림.
       GPIOC 클록은 다른 핀이 쓰고 있을 수 있어서 끄지 않음 */

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/*
 * [참고] 초기화 다음에 main.c에서 쓰는 송수신 함수 (stm32g4xx_hal_uart.c)
 *
 *  HAL_UART_Transmit(&huart1, 데이터 주소, 바이트 수, 타임아웃ms)   1121번째 줄
 *    - 폴링 방식 송신. 바이트마다 TXE 플래그를 기다렸다가 TDR에 씀
 *    - 레지스터 버전 UART1_SendChar()와 같은 동작을 여러 바이트로 반복
 *
 *  HAL_UART_Receive(&huart1, 버퍼 주소, 바이트 수, 타임아웃ms)      1219번째 줄
 *    - 폴링 방식 수신. RXNE 플래그를 기다렸다가 RDR을 읽음
 *    - 레지스터 버전 UART1_ReceiveChar()와 같은 동작
 *
 *  타임아웃에 HAL_MAX_DELAY를 주면 데이터가 올 때까지 무한히 기다림
 *  반환값: HAL_OK(성공), HAL_TIMEOUT(시간 초과), HAL_ERROR, HAL_BUSY
 *
 *  다음 단계(인터럽트 방식): HAL_UART_Receive_IT() + HAL_UART_RxCpltCallback()
 *    - CubeMX에서 USART1 → NVIC Settings → USART1 global interrupt를 켜야 사용 가능
 */

/* USER CODE END 1 */

