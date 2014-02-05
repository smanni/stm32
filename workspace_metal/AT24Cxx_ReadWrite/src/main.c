#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "misc.h"
#include "at24cxx.h"

#define AT24C01A_ADDRESS 0xA0    /* 1 0 1 0 A2 A1 A0 R/W */
#define AT24C01A_T_WRITE 0x05    /* Time needed for a write operation[ms] */

#define POISON_START 	 0x00
#define DATA_SIZE		 21

/* FORWARD DECLARATIONS */
void delay (uint32_t ms);

/* Global variables */
uint32_t g_msCounter = 0;

/*
 * MAIN
 */
int main()
{
	uint8_t i = 0;
	uint8_t data_w[DATA_SIZE] = {0};
	uint8_t data_r[DATA_SIZE] = {0};

	/* Setup SysTick Timer for 1 ms interrupts */
	SysTick_Config(SystemCoreClock / 1000);

	struct AT24Cxx_init_struct AT24C01A_init;

	/* The AT24C01A is a I2C EEPROM with a capacity of 128 bytes arranged in 16 pages of 8 bytes.
	 * The time it needs to complete a write a operation is 5ms */
	AT24C01A_init.pages = 16;
	AT24C01A_init.page_size = 8;
	AT24C01A_init.I2C_address = AT24C01A_ADDRESS;
	AT24C01A_init.I2C_master_address = 0x30;
	AT24C01A_init.GPIO_clock = RCC_APB2Periph_GPIOB;
	AT24C01A_init.I2C_clock = RCC_APB1Periph_I2C1;
	AT24C01A_init.I2C_peripheral = I2C1;
	AT24C01A_init.GPIO_peripheral = GPIOB;
	AT24C01A_init.GPIO_SCL_pin = GPIO_Pin_6;
	AT24C01A_init.GPIO_SDA_pin = GPIO_Pin_7;
	AT24C01A_init.t_write = AT24C01A_T_WRITE;
	AT24C01A_init.delay_fn = delay;

	AT24Cxx_init(&AT24C01A_init);

	while(i < DATA_SIZE){
		data_w[i] = POISON_START + i;
		i++;
	}

	AT24Cxx_write(&AT24C01A_init, 0, data_w, DATA_SIZE);

	AT24Cxx_read(&AT24C01A_init, 0, data_r, DATA_SIZE);

	while(1);

	/* Control should never come here */
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

