#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#define LEDS_GPIO_PORT	 		GPIOF
#define LEDS_GPIO_PORT_CLK		RCC_APB2Periph_GPIOF
#define LED1 					GPIO_Pin_6
#define LED2					GPIO_Pin_7
#define LED3 					GPIO_Pin_8
#define LED4 					GPIO_Pin_9

/* FORWARD DECLARATIONS */
void GPIO_configure(void);
void delay (uint32_t nCount);

/* Global variables */
uint32_t g_msCounter = 0;

/*
 * MAIN
 */
int main() {

	GPIO_configure();

	/* Setup SysTick Timer for 1 msec interrupts */
	SysTick_Config(SystemCoreClock / 1000);

    while(1)
    {
    	GPIO_SetBits(LEDS_GPIO_PORT, GPIO_Pin_6);
    	delay(1000);
    	GPIO_ResetBits(LEDS_GPIO_PORT, GPIO_Pin_6);
    	delay(1000);
    }
}

/*
 * GPIO_configure
 */
void GPIO_configure(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*
	 * LED configuration
	 */

	// Enable clock for GPIO peripheral F
	RCC_APB2PeriphClockCmd( LEDS_GPIO_PORT_CLK, ENABLE);

	// Configure pins
	GPIO_InitStructure.GPIO_Pin =  LED1 | LED2 | LED3 | LED4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDS_GPIO_PORT, &GPIO_InitStructure);

	// Start with all LEDs off
	GPIO_ResetBits(LEDS_GPIO_PORT, LED1 | LED2 | LED3 | LED4);
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
