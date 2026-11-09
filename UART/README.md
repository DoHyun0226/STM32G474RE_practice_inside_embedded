# UART

USART1을 레지스터로 직접 설정해 블루투스 모듈(ZS-040 기판의 BT04-A)과 9600bps로 통신하는 실습.
받은 문자에 1을 더해 다시 보내는 에코 프로그램이다 (`a` 입력 → `b` 출력).

모든 코드는 `Core/Src/main.c`에 있다.

## 동작 흐름

```
main()
 ├─ Initialize_MCU()        // SYSCLK 170MHz 설정 (Clock 프로젝트와 동일)
 ├─ UART1_Init()            // USART1, PC4(TX)/PC5(RX), 9600 8N1
 └─ while(1)
     ├─ UART1_ReceiveChar() // RXNE 대기 → RDR 읽기
     └─ UART1_SendChar(c+1) // TXE 대기 → TDR 쓰기
```

## 배선

| STM32 (NUCLEO-G474RE) | 블루투스 모듈 (ZS-040 / BT04-A) |
|---|---|
| PC4 (USART1_TX) | RXD |
| PC5 (USART1_RX) | TXD |
| 5V | VCC (기판의 LDO가 3.3V로 낮춤, 허용 3.6~6V) |
| GND | GND |

TX와 RX는 엇갈려 연결한다. 신호선은 3.3V 레벨이라 STM32와 바로 연결해도 된다.

## 레지스터 설정 — `UART1_Init()`

| 레지스터 | 설정 | 설명 |
|---|---|---|
| `RCC->APB2ENR` | USART1EN = 1 | USART1 클록 ON (USART1은 APB2, USART2는 APB1) |
| `RCC->AHB2ENR` | GPIOCEN = 1 | GPIOC 클록 ON |
| `GPIOC->MODER` | MODE4, MODE5 = 10 | PC4, PC5 Alternate Function 모드 |
| `GPIOC->AFR[0]` | AFSEL4, AFSEL5 = 7 | AF7 = USART1 |
| `USART1->CR1~CR3` | 0 | 초기화 (FIFO, 흐름 제어 등 부가 기능 OFF) |
| `USART1->BRR` | 17708 | 170MHz / 9600 ≈ 17708.3 (OVER8 = 0, 16배 오버샘플링) |
| `USART1->CR1` | TE, RE, UE = 1 | 송신, 수신, USART 활성화 |

USART1 커널 클록은 리셋 기본값(`RCC->CCIPR` USART1SEL = 00)인 PCLK2 = 170MHz를 그대로 쓴다.

## 송수신

- **송신:** `ISR`의 TXE(TDR 비었음)를 기다린 뒤 `TDR`에 1바이트 쓰기. 하드웨어가 TDR → 시프트 레지스터 → TX 핀으로 BRR 속도에 맞춰 내보낸다.
- **수신:** `ISR`의 RXNE(RDR에 데이터 있음)를 기다린 뒤 `RDR` 읽기. RDR을 읽으면 RXNE가 자동으로 0이 된다.
  다음 바이트가 오기 전에 RDR을 읽지 않으면 ORE(오버런) 플래그가 켜지고 새 데이터는 버려진다.

## PC에서 확인

1. Windows 블루투스 설정에서 `BT04-A`와 페어링 (PIN `1234`). BT04-A는 클래식과 BLE를 동시에 지원해서 목록에 두 번 보일 수 있다.
2. COM 포트가 2개 생긴다. **"보내는(Outgoing)"** 포트(이 PC에서는 COM5)를 쓴다.
   "보내는/들어오는"은 데이터 방향이 아니라 블루투스 연결을 누가 먼저 거느냐를 뜻한다. BT04-A는 먼저 연결을 걸지 못하므로 PC가 거는 Outgoing 포트로만 연결된다. 연결 후에는 한 포트로 양방향 통신이 된다.
3. Tera Term에서 해당 COM 포트를 9600, 8N1로 연다.

## 주의할 점

- 연결 직후 모듈이 보내는 상태 메시지(`+CONNECTING<<…`, `+CONNECTED`)도 STM32가 받아서 +1 해 돌려보내므로, 화면에 한 글자씩 밀린 문자(`,DPOOFDUFE`)가 찍힌다.
- Enter(CR, 0x0D)를 치면 0x0E(Shift Out)가 돌아와서 Tera Term이 문자 세트를 바꾼다. 이후 소문자가 그리스 문자(α, β, …)로 보인다.
  Ctrl+N(0x0E)을 보내면 0x0F(Shift In)가 돌아와서 원래대로 돌아오고, Tera Term 메뉴 **제어 → 터미널 초기화**로도 되돌릴 수 있다.

같은 동작을 HAL로 만든 버전은 [UART_HAL](../UART_HAL) 참고.
