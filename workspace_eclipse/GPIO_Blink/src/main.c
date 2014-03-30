#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


/* FORWARD DECLARATIONS */
void GPIO_Configuration(void);
void Delay (uint32_t nCount);

/*
 * MAIN
 */
int main() {

	GPIO_Configuration();

  	while (1)
	{
		GPIO_SetBits(GPIOF, GPIO_Pin_6);
		Delay(1000000);
	 	GPIO_ResetBits(GPIOF, GPIO_Pin_6);

		GPIO_SetBits(GPIOF, GPIO_Pin_7);
		Delay(1000000);
	 	GPIO_ResetBits(GPIOF, GPIO_Pin_7);

		GPIO_SetBits(GPIOF, GPIO_Pin_8);
		Delay(1000000);
	 	GPIO_ResetBits(GPIOF, GPIO_Pin_8);

		GPIO_SetBits(GPIOF, GPIO_Pin_9);
		Delay(1000000);
	 	GPIO_ResetBits(GPIOF, GPIO_Pin_9);
	}
}

/*
 * GPIO_Configuration
 */
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable clock for GPIO peripheral F
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF, ENABLE);

	/**
	*  LED1 -> PF6 , LED2 -> PF7 , LED3 -> PF8 , LED4 -> PF9
	*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/*
 * Delay
 */
void Delay (uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
