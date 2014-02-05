#include "at24cxx.h"

void AT24Cxx_init(struct AT24Cxx_init_struct* init)
{
	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure clocks */
	RCC_APB1PeriphClockCmd(init->I2C_clock, ENABLE);
	RCC_APB2PeriphClockCmd(init->GPIO_clock | RCC_APB2Periph_AFIO , ENABLE);

	/* Configure I2C pins: SCL and SDA */
	GPIO_InitStructure.GPIO_Pin =  init->GPIO_SCL_pin | init->GPIO_SDA_pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(init->GPIO_peripheral, &GPIO_InitStructure);

	/* Configure I2C */
	I2C_DeInit(init->I2C_peripheral);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = init->I2C_master_address;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 10000;

	I2C_Cmd(init->I2C_peripheral, ENABLE);
	I2C_Init(init->I2C_peripheral, &I2C_InitStructure);
	I2C_AcknowledgeConfig(init->I2C_peripheral, ENABLE);
}

#ifdef ACK_POLLING_SUPPORT
static void AT24Cxx_ack_polling(struct AT24Cxx_init_struct* init)
{
	/* Until ack fails (I2C_FLAG_AF) continue with the polling */
	do {
		I2C_GenerateSTART(init->I2C_peripheral, ENABLE);
		I2C_Send7bitAddress(init->I2C_peripheral, init->I2C_address, I2C_Direction_Transmitter);
	} while((I2C_GetLastEvent(init->I2C_peripheral) & I2C_FLAG_AF));

	I2C_ClearFlag(init->I2C_peripheral, I2C_FLAG_AF);
	I2C_GenerateSTOP(init->I2C_peripheral, ENABLE);
}
#endif

static uint32_t AT24Cxx_write_page(struct AT24Cxx_init_struct* init, uint8_t addr, uint8_t* buff, uint8_t count)
{
	uint8_t _count = count;

	/* start, I2c_address, memory_address, data(n), data(n+1), ... , stop */

  	I2C_GenerateSTART(init->I2C_peripheral, ENABLE);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_MODE_SELECT));

  	I2C_Send7bitAddress(init->I2C_peripheral, init->I2C_address, I2C_Direction_Transmitter);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  	I2C_SendData(init->I2C_peripheral, addr);
	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	while(count--) {
		I2C_SendData(init->I2C_peripheral, *buff);
  		while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  		buff++;
	}

  	I2C_GenerateSTOP(init->I2C_peripheral, ENABLE);

#ifdef ACK_POLLING_SUPPORT
  	AT24Cxx_ack_polling(init);
#endif

  	/* After stop let the EEPROM to complete the internal write cycle */
  	init->delay_fn(init->t_write);

  	return _count;
}

uint32_t AT24Cxx_write(struct AT24Cxx_init_struct* init, uint8_t addr, uint8_t* buff, uint32_t count)
{
	uint8_t n = 0;				/* number of bytes to write for each page */
	uint32_t written = 0;		/* total number of written bytes */

	/* Is it a valid address? */
	if((addr + count) >  (init->pages) * (init->page_size))
		return 0;

	uint8_t page = (addr / init->page_size);
	uint8_t page_boundary = (page + 1) * init->page_size - 1;

	/* Write as much bytes as possible until the page boundary */
	while(count) {

		n = page_boundary - addr + 1;

		if(n > count)
			n = count;

		written += AT24Cxx_write_page(init, addr, buff, n);

		buff += n;
		addr += n;
		count -= n;
		page_boundary = (++page + 1) * init->page_size - 1;
	}

	return written;
}

uint32_t AT24Cxx_read(struct AT24Cxx_init_struct* init, uint8_t addr, uint8_t* buff, uint32_t count)
{
	/* Is it a valid address? */
	if((addr + count) >  (init->pages) * (init->page_size))
		return 0;

	/* start, dummy write, read, stop */
	/* start, I2c_address(W), memory_address, start, I2C_address(R), receive, stop */

  	I2C_GenerateSTART(init->I2C_peripheral, ENABLE);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_MODE_SELECT));

  	I2C_Send7bitAddress(init->I2C_peripheral, init->I2C_address, I2C_Direction_Transmitter);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  	I2C_SendData(init->I2C_peripheral, addr);
	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  	I2C_GenerateSTART(init->I2C_peripheral, ENABLE);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_MODE_SELECT));

  	I2C_Send7bitAddress(init->I2C_peripheral, init->I2C_address, I2C_Direction_Receiver);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  	while (count--) {

  		/* In a sequential read if the master does not ack then this is the last byte to read */
  		if(count == 0)
  			I2C_AcknowledgeConfig(init->I2C_peripheral, DISABLE);

		while(!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_BYTE_RECEIVED));
	    *buff = I2C_ReceiveData(init->I2C_peripheral);
	    buff++;
  	}

  	/* Re-enable ack */
  	I2C_AcknowledgeConfig(init->I2C_peripheral, ENABLE);
  	I2C_GenerateSTOP(init->I2C_peripheral, ENABLE);

  	return count;
}
