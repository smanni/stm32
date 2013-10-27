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
void EXTI_configure(void);
void NVIC_configure(void);
void delay (uint32_t nCount);
uint32_t button_digital_read(GPIO_TypeDef* port, uint16_t pin);
void led_on_off(GPIO_TypeDef* port, uint16_t pin);

/*
 * MAIN
 */
int main() {

	GPIO_configure();
	EXTI_configure();
	NVIC_configure();

    while(1)
    {
    	/* Do nothing since all is managed via ISR */
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
 * Configure external interrupts
 */
void EXTI_configure(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    //enable AFIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);

    //Connect EXTI Lines to Button Pins
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3);

    //select external lines to enable
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line3;
    //select interrupt mode
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //generate interrupt on falling edge
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    //enable
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    //send values to registers
    EXTI_Init(&EXTI_InitStructure);
}

/**
 * Configure NVIC (nested vector interrupt controller)
 */
void NVIC_configure(void)
{
    //NVIC structure to set up NVIC controller
    NVIC_InitTypeDef NVIC_InitStructure;

    //select NVIC channel to configure
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; // this cannot be an OR bitwise of values
    //set priority to lowest
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    //set subpriority to lowest
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    //enable IRQ channel
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //update NVIC registers
    NVIC_Init(&NVIC_InitStructure);

    // do the same for exti1
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    // do the same for exti2
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    // do the same for exti3
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * Interrupt handler associated with EXTI0 (it must have this name as defined in the vector table)
 */
void EXTI0_IRQHandler(void)
{

    //Check if EXTI_Line0 is asserted
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        led_on_off(LEDS_GPIO_PORT, LED2);
    }
    //we need to clear line pending bit manually
    EXTI_ClearITPendingBit(EXTI_Line0);
}

/*
 * Interrupt handler associated with EXTI1 (it must have this name as defined in the vector table)
 */
void EXTI1_IRQHandler(void)
{

    //Check if EXTI_Line0 is asserted
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        led_on_off(LEDS_GPIO_PORT, LED1);
    }
    //we need to clear line pending bit manually
    EXTI_ClearITPendingBit(EXTI_Line1);
}

/*
 * Interrupt handler associated with EXTI2 (it must have this name as defined in the vector table)
 */
void EXTI2_IRQHandler(void)
{

    //Check if EXTI_Line0 is asserted
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        led_on_off(LEDS_GPIO_PORT, LED3);
    }
    //we need to clear line pending bit manually
    EXTI_ClearITPendingBit(EXTI_Line2);
}

/*
 * Interrupt handler associated with EXTI3 (it must have this name as defined in the vector table)
 */
void EXTI3_IRQHandler(void)
{

    //Check if EXTI_Line0 is asserted
    if(EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        led_on_off(LEDS_GPIO_PORT, LED4);
    }
    //we need to clear line pending bit manually
    EXTI_ClearITPendingBit(EXTI_Line3);
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
