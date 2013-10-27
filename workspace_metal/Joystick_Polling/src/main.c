#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#define BUTTONS_GPIO_PORT	 	GPIOC
#define BUTTONS_GPIO_PORT_CLK	RCC_APB2Periph_GPIOC
#define BTN_UP 					GPIO_Pin_1
#define BTN_RIGHT				GPIO_Pin_0
#define BTN_DOWN				GPIO_Pin_2
#define BTN_LEFT				GPIO_Pin_3

#define LEDS_GPIO_PORT	 		GPIOF
#define LEDS_GPIO_PORT_CLK		RCC_APB2Periph_GPIOF
#define LED1 					GPIO_Pin_6
#define LED2					GPIO_Pin_7
#define LED3 					GPIO_Pin_8
#define LED4 					GPIO_Pin_9

/* FORWARD DECLARATIONS */
void GPIO_configure(void);
void delay (uint32_t nCount);
uint32_t button_digital_read(GPIO_TypeDef* port, uint16_t pin);
void led_on_off(GPIO_TypeDef* port, uint16_t pin);

/*
 * MAIN
 */
int main() {

	GPIO_configure();

    while(1)
    {
    	if(button_digital_read(BUTTONS_GPIO_PORT, BTN_UP))
    	{
    		led_on_off(LEDS_GPIO_PORT, LED1);
    	}
    	if(button_digital_read(BUTTONS_GPIO_PORT, BTN_RIGHT))
    	{
    		led_on_off(LEDS_GPIO_PORT, LED2);
    	}
    	if(button_digital_read(BUTTONS_GPIO_PORT, BTN_DOWN))
    	{
    		led_on_off(LEDS_GPIO_PORT, LED3);
    	}
    	if(button_digital_read(BUTTONS_GPIO_PORT, BTN_LEFT))
    	{
    		led_on_off(LEDS_GPIO_PORT, LED4);
    	}
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

	/*
	 * Joystick configuration
	 */

	// Enable clock for GPIO peripheral C
	RCC_APB2PeriphClockCmd( BUTTONS_GPIO_PORT_CLK, ENABLE);

	// Configure pins
	GPIO_InitStructure.GPIO_Pin = BTN_UP | BTN_RIGHT | BTN_DOWN | BTN_LEFT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // input pull-up (when button is pressed pin goes to GND)
	GPIO_Init(BUTTONS_GPIO_PORT, &GPIO_InitStructure);
}

/*
 * Read thw value of a GPIO as a bit
 */
uint32_t button_digital_read(GPIO_TypeDef* port, uint16_t pin)
{
	return !GPIO_ReadInputDataBit(port, pin);
}

/*
 * Turn a LED on, wait and the turn it off
 */
void led_on_off(GPIO_TypeDef* port, uint16_t pin)
{
	GPIO_SetBits(port, pin);
	delay(500000);
	GPIO_ResetBits(port, pin);
}

/*
 * Delay function
 */
void delay (uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
