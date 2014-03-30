#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "stdio.h"

#define LEDS_GPIO_PERIPH	 		GPIOF
#define LEDS_GPIO_PERIPH_CLK		RCC_APB2Periph_GPIOF
#define LED1 						GPIO_Pin_6
#define LED2						GPIO_Pin_7
#define LED3 						GPIO_Pin_8
#define LED4 						GPIO_Pin_9

#define USART1_PINS_GPIO_PERIPH	  	GPIOA
#define USART1_PINS_GPIO_PERIPH_CLK	RCC_APB2Periph_GPIOA
#define USART1_PERIPH_CLK           RCC_APB2Periph_USART1
#define USART1_TX				  	GPIO_Pin_9
#define USART1_RX				  	GPIO_Pin_10

/* Define which USART use for stdin, stdout, stderr */
#define STDIN_USART 				1
#define STDOUT_USART 				1
#define STDERR_USART 				1

/* FORWARD DECLARATIONS */
void GPIO_configure(void);
void RCC_configure(void);
void USART_configure(void);
void delay (uint32_t nCount);

/* Global variables */
uint32_t g_msCounter = 0;

/*
 * MAIN
 */
int main()
{
	char name[32];

	/* Setup SysTick Timer for 1 msec interrupts */
	SysTick_Config(SystemCoreClock / 1000);

	/* Needed configuration */
	RCC_configure();
	GPIO_configure();
	USART_configure();

	/* Disable buffering for stin and stdout */
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

    while(1)
    {
    	printf("Insert your name:");
    	scanf("%s", name);
    	printf("Hey %s, how are you?\n\n", name);
    }

    /* Control should never come here */
    return 0;
}

/*
 * System clock configuration
 */
void RCC_configure(void)
{
	// Enable clock for GPIO peripheral the LEDS are connected to
	RCC_APB2PeriphClockCmd(LEDS_GPIO_PERIPH_CLK, ENABLE);

	// Enable clock for USART1, and USART1 GPIO PINS RX and TX
	RCC_APB2PeriphClockCmd(USART1_PERIPH_CLK | USART1_PINS_GPIO_PERIPH_CLK, ENABLE);
}

/*
 * GPIO_configure
 */
void GPIO_configure(void)
{
	GPIO_InitTypeDef GPIO_LEDS_InitStructure;
	GPIO_InitTypeDef GPIO_USART1_InitStructure;

	// Configure pins as push pull (LED)
	GPIO_LEDS_InitStructure.GPIO_Pin =  LED1 | LED2 | LED3 | LED4;
	GPIO_LEDS_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_LEDS_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDS_GPIO_PERIPH, &GPIO_LEDS_InitStructure);

	// Start with all LEDs off
	GPIO_ResetBits(LEDS_GPIO_PERIPH, LED1 | LED2 | LED3 | LED4);

	// Configure pins as alternate function (USART1 TX)
	GPIO_USART1_InitStructure.GPIO_Pin = USART1_TX;
	GPIO_USART1_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_USART1_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(USART1_PINS_GPIO_PERIPH, &GPIO_USART1_InitStructure);

	// Configure pins as alternate function (USART1 RX)
	GPIO_USART1_InitStructure.GPIO_Pin = USART1_RX;
	GPIO_USART1_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_USART1_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1_PINS_GPIO_PERIPH, &GPIO_USART1_InitStructure);
}

/*
 * USART configuration
 */
void USART_configure(void)
{
	USART_InitTypeDef USART1_InitStructure;

	// Enable USART1
	USART_Cmd(USART1, ENABLE);

	// Baud rate 9600, 8-bit data, One stop bit
	// No parity, Do both Rx and Tx, No HW flow control
	USART1_InitStructure.USART_BaudRate = 9600;
	USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART1_InitStructure.USART_StopBits = USART_StopBits_1;
	USART1_InitStructure.USART_Parity = USART_Parity_No ;
	USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART1, &USART1_InitStructure);
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
