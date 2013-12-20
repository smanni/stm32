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
