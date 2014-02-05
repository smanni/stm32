#ifndef __AT24CXX_H__
#define __AT24CXX_H__

#include <stdint.h>
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef enum {AT24C01A} AT24Cxx_type;

struct AT24Cxx_init_struct {
	uint8_t pages;							/* number of pages */
	uint8_t page_size;						/* page size (in bytes) */
	uint8_t I2C_address;					/* I2C slave (EEPROM) address */
	uint16_t I2C_master_address;			/* I2C master (mcu)  address */
	uint32_t I2C_clock;						/* I2C peripheral clock */
	uint32_t GPIO_clock;					/* GPIO peripheral clock */
	I2C_TypeDef* I2C_peripheral;			/* I2C peripheral */
	GPIO_TypeDef* GPIO_peripheral;			/* GPIO peripheral */
	uint16_t GPIO_SCL_pin;					/* I2C SCL pin */
	uint16_t GPIO_SDA_pin;					/* I2C SDA pin */
	uint16_t t_write;						/* time needed for a write operation [ms] */
	void(*delay_fn)(uint32_t ms);			/* function delay(ms) */
};


void AT24Cxx_init(struct AT24Cxx_init_struct* init);

uint32_t AT24Cxx_read(struct AT24Cxx_init_struct* init, uint8_t addr, uint8_t* buff, uint32_t count);

uint32_t AT24Cxx_write(struct AT24Cxx_init_struct* init, uint8_t addr, uint8_t* buff, uint32_t count);


#endif /* __AT24CXX_H__ */
