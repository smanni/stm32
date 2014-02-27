#ifndef __AT45DBXX_h__
#define __AT45DBXX_h__

#include <stdint.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

/* Macros */
#define CS_ASSERT(init) do { init->gpio_periph->BRR = init->gpio_cs; } while(0) /* reset */
#define CS_DEASSERT(init) do { init->gpio_periph->BSRR = init->gpio_cs; } while(0) /* set */

#define ID_TO_MANUFACTURER_ID(id) (id >> 24)     				/* byte 1 (MSB) of id */
#define ID_TO_DEVICE_ID(id) ((id >> 8) & 0xFFFF)    				/* bytes 2-3  of id */
#define ID_TO_FAMILY_CODE(id) (ID_TO_DEVICE_ID(id) >> 13)  			/* 3 MSB bits of byte 2 */
#define ID_TO_DENSITY_CODE(id) ((ID_TO_DEVICE_ID(id) >> 8) & 0x1F) 	/* 5 LSB bits of byte 2 */

/* Opcodes */
#define OP_MANUFACTURER_DEVICE_ID	0x9F
#define OP_READ_STATUS_REG			0xD7


/* Data types */
struct AT45DBxx_init {
	uint32_t spi_clock_periph;				/* clock for SPI peripheral */
	uint32_t gpio_clock_periph;				/* clock for GPIO peripheral (related and not-related to SPI) */
	SPI_TypeDef* spi_periph;				/* SPI peripheral */
	GPIO_TypeDef* gpio_periph;				/* GPIO peripheral */
	uint16_t gpio_ck;						/* clock GPIO pin (related to SPI) */
	uint16_t gpio_si;						/* slave input GPIO pin (related to SPI) */
	uint16_t gpio_so;						/* slave output GPIO pin (related to SPI) */
	uint16_t gpio_cs;						/* chip select GPIO pin (related to SPI) */
	uint16_t gpio_wp;						/* hardware write protection GPIO pin (not-related to SPI) (OPTIONAL) */
	uint16_t gpio_rs;						/* hardware reset GPIO pin (not-related to SPI) (OPTIONAL) */
};

/* Public functions */
uint32_t AT45DBxx_init(struct AT45DBxx_init* init);
uint32_t AT45DBxx_read_id(struct AT45DBxx_init* init);
void AT45DBxx_busy_wait(struct AT45DBxx_init* init);

/* Private functions */
static void send(struct AT45DBxx_init* init, uint8_t data);
static uint8_t receive(struct AT45DBxx_init* init);

#endif /* __AT45DBXX_h__ */
