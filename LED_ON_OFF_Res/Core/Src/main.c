//주소를 정의를 해줘야 함

// RM p83 Memory map and register boundary addresses 참고
#define GPIOA_BASE		0x48000000UL // GIPOA 레지스터 시작점 주소
#define RCC_BASE		0x40021000UL// RCC 레지스터 시작점 주소, RCC: Reset and Clock Control


// GPIO 레지스터 오프셋: BASE에서 얼마나 떨어졌는지 나타냄 RM p362 GPIO register map 참고
#define GPIO_MODER_OFFSET		0x00UL
#define GPIO_OTYPER_OFFSET		0x04UL
#define GPIO_OSPEEDR_OFFSET		0x08UL
#define GPIO_PUPDR_OFFSET		0x0CUL
#define GPIO_IDR_OFFSET			0x10UL
#define GPIO_ODR_OFFSET 		0x14UL

// RCC 레지스터 오프셋 RM p287 RCC registers 참고
#define RCC_AHB1ENR_OFFSET 0x48UL
#define RCC_AHB2ENR_OFFSET 0x4CUL

// 레지스터 주소 정의 (포인터로 캐스팅)

#define RCC_AHB1ENR		(*((volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET)))
#define RCC_AHB2ENR		(*((volatile uint32_t *)(RCC_BASE + RCC_AHB2ENR_OFFSET)))
#define GPIOA_MODER		(*((volatile uint32_t *)(GPIOA_BASE + GPIO_MODER_OFFSET)))
#define GPIO_OTYPER		(*((volatile uint32_t *)(GPIOA_BASE + GPIO_OTYPER_OFFSET)))
#define GPIO_OSPEEDR	(*((volatile uint32_t *)(GPIOA_BASE + GPIO_OSPEEDR_OFFSET)))
#define GPIO_PUPDR		(*((volatile uint32_t *)(GPIOA_BASE + GPIO_PUPDR_OFFSET)))
#define GPIO_IDR		(*((volatile uint32_t *)(GPIOA_BASE + GPIO_IDR_OFFSET)))
#define GPIO_ODR		(*((volatile uint32_t *)(GPIOA_BASE + GPIO_ODR_OFFSET)))
/*
 * volatile
 * 변수를 선언할 때 앞에 volatile을 붙이면 컴파일러는 해당 변수를 최적화에서 제외해 항상 메모리에 접근하게 한다..
 * 즉 volatile 변수를 참조할 경우 레지스터에 로드된 값을 사용하지 않고 매번 메모리를 참조한다.
 * <syntax>
 * volatile [type] [variable_name];
 *
 * 		*(unsigned int *)0x8C0F = 0x8001;
 *		*(unsigned int *)0x8C0F = 0x8002;
 *		*(unsigned int *)0x8C0F = 0x8003;
 *		*(unsigned int *)0x8C0F = 0x8004;
 *		*(unsigned int *)0x8C0F = 0x8005;
 *
 *	위와 같이 작성할 경우 컴파일러는 최적화를 위해 이전의 코드를 실행하지 않고,
 *	가장 마지막 코드만 실행시킴.
 *	일반적 코드라면 최적화로 속도에서 이득을 보지만, 이 코드가 메모리 주소에 연결된 하드웨어
 *	레지스터에 값을 쓰는 프로그램이라면, 각각의 쓰기가 하드웨어에 특정 명령을 전달하는 것이므로,
 *	주소가 같다는 이유만으로 중복되는 쓰기 명령을 없애버리면 하드웨어가 오작동하게 된다.
 *	이럴 경우 volatile 타입으로 지정하면 최적화를 수행하지 않고 모든 메모리 쓰기를 지정한 대로
 *	수행한다.
 */


typedef struct {
	volatile uint32_t MODER; 	// 오프셋 0x00 //오프셋 0x00
	volatile uint32_t OTYPER; 	// 오프셋 0x04 //오프셋 0x04
	volatile uint32_t OSPEEDR;	// 오프셋 0x08 //오프셋 0x08
	volatile uint32_t PUPDR;	// 오프셋 0x0C //오프셋 0x0C
	volatile uint32_t IDR;		// 오프셋 0x10 //오프셋 0x10
	volatile uint32_t ODR;		// 오프셋 0x14 //오프셋 0x14
	volatile uint32_t BSRR;		// 오프셋 0x18 //오프셋 0x18
	volatile uint32_t LCKR;		// 오프셋 0x1C //오프셋 0x1C
	volatile uint32_t AFR[2];	// 오프셋 0x20 오프셋 0x24 // 오프셋 0x20 오프셋 0x24
}GPIO_TypeDef;

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)


























