#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "at45dbxx.h"
#include "misc.h"

#define TEST_BUF_SIZE 264

/*
 * MAIN
 */
int main() {

	uint32_t n, id, man, dev, fam, den;
	uint8_t buf_i[TEST_BUF_SIZE];
	uint8_t	buf_o[TEST_BUF_SIZE];

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
	id = AT45DBxx_read_id(&init);
	man = ID_TO_MANUFACTURER_ID(id);
	dev = ID_TO_DEVICE_ID(id);
	fam = ID_TO_FAMILY_CODE(id);
	den = ID_TO_DENSITY_CODE(id);

	/* Internal SRAM buffer1 write and read */
	for(n = 0; n < TEST_BUF_SIZE; n++) {
		buf_i[n] = n;
		buf_o[n] = 0xca;
	}
	AT45DBxx_buffer_write(buf_i, &init, 1, 0x0, TEST_BUF_SIZE);
	AT45DBxx_buffer_read(buf_o, &init, 1, 0x0, TEST_BUF_SIZE);
	for(n = 0; n < TEST_BUF_SIZE; n++)
		if(buf_i[n] != buf_o[n])
			while(1);

	/* Main memory page write and read */
	for(n = 0; n < TEST_BUF_SIZE; n++) {
		buf_i[n] = n;
		buf_o[n] = 0xca;
	}
	AT45DBxx_page_write(buf_i, &init, 0x0, 0x0, TEST_BUF_SIZE);
	AT45DBxx_page_read(buf_o, &init, 0x0, 0x0, TEST_BUF_SIZE);
	for(n = 0; n < TEST_BUF_SIZE; n++)
		if(buf_i[n] != buf_o[n])
			while(1);

	/* Main memory page erase */
	AT45DBxx_page_erase(&init, 0x0);
	AT45DBxx_page_read(buf_o, &init, 0x0, 0x0, TEST_BUF_SIZE);
	for(n = 0; n < TEST_BUF_SIZE; n++)
		if(buf_o[n] != 0xff)
			while(1);

	/* Deep power down mode (remove the comment two lines below if you want to test the feature */
	AT45DBxx_put_deep_power_down(&init);
	/* AT45DBxx_busy_wait(&init);  It should block here */
	AT45DBxx_resume_deep_power_down(&init);

	/* Control should never come here */
	while(1);
}
