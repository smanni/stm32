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

	uint32_t id, man, dev, fam, dens;
	id = AT45DBxx_read_id(&init);

	man = ID_TO_MANUFACTURER_ID(id);
	dev = ID_TO_DEVICE_ID(id);
	fam = ID_TO_FAMILY_CODE(id);
	dens = ID_TO_DENSITY_CODE(id);

	/* Control should never come here */
	while(1);
}
