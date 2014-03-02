#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "at45dbxx.h"
#include "misc.h"

/*
 * MAIN
 */
int main() {

	/* 4 Mbit dataflash (AT45DB041D) organized in 2048 pages of 256 bytes */
	struct AT45DBxx_init init = {
		.spi_clock_periph = RCC_APB2Periph_SPI1,
		.gpio_clock_periph = RCC_APB2Periph_GPIOA,
		.spi_periph = SPI1,
		.gpio_periph = GPIOA,
		.gpio_cs = GPIO_Pin_4,  /* PA4 */
		.gpio_ck = GPIO_Pin_5,	/* PA5 */
		.gpio_so = GPIO_Pin_6,  /* PA6 */
		.gpio_si = GPIO_Pin_7,	/* PA7 */
		.gpio_rs = 0,			/* reset not connected */
		.gpio_wp = 0, 			/* write-protection not connected */
	};

	if(AT45DBxx_init(&init))
		while(1);

	/* Read device ID */
	uint32_t id, man, dev, fam, den;
	id = AT45DBxx_read_id(&init);
	man = ID_TO_MANUFACTURER_ID(id);
	dev = ID_TO_DEVICE_ID(id);
	fam = ID_TO_FAMILY_CODE(id);
	den = ID_TO_DENSITY_CODE(id);

	/* Internal SRAM buffer1 write and read */
	uint32_t n;
	uint8_t buf_i[32];
	uint8_t	buf_o[32];
	for(n = 0; n < 32; n++) {
		buf_i[n] = n;
		buf_o[n] = 0xff;
	}
	AT45DBxx_buffer_write(buf_i, &init, 1, 0x0, 32);
	AT45DBxx_buffer_read(buf_o, &init, 1, 0x0, 32);
	for(n = 0; n < 32; n++)
		if(buf_i[n] != buf_o[n])
			while(1);

	/* Main memory write and read */
	for(n = 0; n < 32; n++) {
		buf_i[n] = n;
		buf_o[n] = 0xff;
	}
	AT45DBxx_page_write(buf_i, &init, 0x0, 0x0, 32);
	AT45DBxx_page_read(buf_o, &init, 0x0, 0x0, 32);
		for(n = 0; n < 32; n++)
			if(buf_i[n] != buf_o[n])
				while(1);

	/* Control should never come here */
	while(1);
}
