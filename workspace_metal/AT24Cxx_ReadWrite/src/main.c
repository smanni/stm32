#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "misc.h"
#include "at24cxx.h"

#define AT24C01A_ADDRESS 0xA0    /* 1 0 1 0 A2 A1 A0 R/W */

/*
 * MAIN
 */
int main()
{
	uint8_t data_w[32] = {0};
	uint8_t data_r[64] = {0};

	struct AT24Cxx_init_struct AT24C01A_init;

	// The AT24C01A is a I2C EEPROM with a capacity of 128 bytes arranged in 16 pages of 8 bytes
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

	AT24Cxx_init(&AT24C01A_init);

	AT24Cxx_write(&AT24C01A_init, 0, data_w, 32);

	AT24Cxx_read(&AT24C01A_init, 0, data_r, 32);

	while(1) {

	}

	/* Control should never come here */
}
