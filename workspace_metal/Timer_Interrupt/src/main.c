#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "misc.h"

#define LEDS_GPIO_PORT	 		GPIOF
#define LEDS_GPIO_PORT_CLK		RCC_APB2Periph_GPIOF
#define LED1 					GPIO_Pin_6
#define LED2					GPIO_Pin_7
#define LED3 					GPIO_Pin_8
#define LED4 					GPIO_Pin_9

/* FORWARD DECLARATIONS */
void GPIO_configure(void);
void TIMER_configure(void);
void RCC_configure(void);
void NVIC_configure(void);
void delay (uint32_t nCount);

/* Global variables */
uint32_t g_msCounter = 0;

/*
 * MAIN
 */
int main()
{
	/* Setup SysTick Timer for 1 msec interrupts */
	SysTick_Config(SystemCoreClock / 1000);

	/* Needed configuration */
	RCC_configure();
	GPIO_configure();
	NVIC_configure();
	TIMER_configure();

	/* Do nothing here. Everything works via ISR */
    while(1)
    {
    }

    /* Control should never come here */
    return 0;
}

/*
 * System clock configuration
 */
void RCC_configure(void)
{
	// Enable clock for GPIO peripheral
	RCC_APB2PeriphClockCmd(LEDS_GPIO_PORT_CLK, ENABLE);

	// Enable clock for TIM peripheral
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

/*
 * GPIO_configure
 */
void GPIO_configure(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure pins as push pull (LED)
	GPIO_InitStructure.GPIO_Pin =  LED1 | LED2 | LED3 | LED4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDS_GPIO_PORT, &GPIO_InitStructure);

	// Start with all LEDs off
	GPIO_ResetBits(LEDS_GPIO_PORT, LED1 | LED2 | LED3 | LED4);
}

/*
 * Nested vector table configuration
 */
void NVIC_configure(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
 * TIM2 peripheral configure
 */
void TIMER_configure(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* TIM2 generates an interrupt with a period of 1second (1Hz) */
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock  / 7200) - 1; // to 10KHz
	TIM_TimeBaseStructure.TIM_Period = 10000 - 1; // to 1Hz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* TIM Interrupts enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

/*
 * TIM2 global interrupt handler
 */
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

		// Not using STDPeriph API is more efficient here
		GPIOF->ODR ^= LED1;
	}
}

/*
 * Overwrite weak handler for systick interrupts
 */
void SysTick_Handler(void)
{
	if(g_msCounter > 0)
		g_msCounter--;
}

/*
 * Delay function
 * @param ms milliseconds to wait
 */
void delay (uint32_t ms)
{
	g_msCounter = ms;
	while(g_msCounter > 0);
}
