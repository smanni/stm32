#include "at24cxx.h"

void AT24Cxx_init(struct AT24Cxx_init_struct* init)
{
	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure clocks */
	RCC_APB1PeriphClockCmd(init->I2C_clock, ENABLE);
	RCC_APB2PeriphClockCmd(init->GPIO_clock | RCC_APB2Periph_AFIO , ENABLE);

	/* Configure I2C1 pins: SCL and SDA */
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
	I2C_InitStructure.I2C_ClockSpeed = 100000;

	I2C_Cmd(init->I2C_peripheral, ENABLE);
	I2C_Init(init->I2C_peripheral, &I2C_InitStructure);
	I2C_AcknowledgeConfig(init->I2C_peripheral, ENABLE);
}


static uint32_t AT24Cxx_write_byte(struct AT24Cxx_init_struct* init, uint8_t addr, uint8_t* buff)
{
	/* start, I2c_address, memory_address, data, stop */

  	I2C_GenerateSTART(init->I2C_peripheral, ENABLE);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_MODE_SELECT));

  	I2C_Send7bitAddress(init->I2C_peripheral, init->I2C_address, I2C_Direction_Transmitter);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  	I2C_SendData(init->I2C_peripheral, addr);
	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  	I2C_SendData(init->I2C_peripheral, *buff);
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  	I2C_GenerateSTOP(init->I2C_peripheral, ENABLE);

  	/* TODO: wait for the ACK */

  	/* TODO: return 1 only if the write has been successful */
  	return 1;
}

uint32_t AT24Cxx_write(struct AT24Cxx_init_struct* init, uint8_t addr, uint8_t* buff, uint32_t count)
{
	while (count--) {
		AT24Cxx_write_byte(init, addr, buff);
	}

	/* TODO: return number of written bytes */
	return count;
}

uint32_t AT24Cxx_read(struct AT24Cxx_init_struct* init, uint8_t addr, uint8_t* buff, uint32_t count)
{
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
  	while (!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  	while (count--) {
		while(!I2C_CheckEvent(init->I2C_peripheral, I2C_EVENT_MASTER_BYTE_RECEIVED));
	    *buff = I2C_ReceiveData(init->I2C_peripheral);
	    buff++;
  	}

  	I2C_GenerateSTOP(init->I2C_peripheral, ENABLE);


  	/* TODO: wait for the ACK */

  	/* TODO: return 1 only if the write has been successful */
  	return 1;
}
