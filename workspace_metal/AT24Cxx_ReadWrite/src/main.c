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
	uint8_t data_w[32] = {0xca, 0xfe, 0xca, 0xfe};
	uint8_t data_r[32];

	struct AT24Cxx_init_struct AT24C01A_init;

	AT24C01A_init.type = AT24C01A;
	AT24C01A_init.I2C_address = AT24C01A_ADDRESS;
	AT24C01A_init.I2C_master_address = 0x30;
	AT24C01A_init.GPIO_clock = RCC_APB2Periph_GPIOB;
	AT24C01A_init.I2C_clock = RCC_APB1Periph_I2C1;
	AT24C01A_init.I2C_peripheral = I2C1;
	AT24C01A_init.GPIO_peripheral = GPIOB;
	AT24C01A_init.GPIO_SCL_pin = GPIO_Pin_6;
	AT24C01A_init.GPIO_SDA_pin = GPIO_Pin_7;

	AT24Cxx_init(&AT24C01A_init);

	AT24Cxx_write(&AT24C01A_init, 0, data_w, 4);

	AT24Cxx_read(&AT24C01A_init, 0, data_r, 4);

	while(1) {

	}

	/* Control should never come here */
}
