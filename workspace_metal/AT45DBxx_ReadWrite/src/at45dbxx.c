#include "at45dbxx.h"

/* Init peripherals to talk with the dataflash */
uint32_t AT45DBxx_init(struct AT45DBxx_init* init)
{
	GPIO_InitTypeDef gpio_init;
	SPI_InitTypeDef spi_init;

	/* invalid argument */
	if(!init)
		return 1;

	/* clocks configuration */
	RCC_APB2PeriphClockCmd(init->gpio_clock_periph | init->spi_clock_periph | RCC_APB2Periph_AFIO, ENABLE);

	/* GPIOs (SPI related) configuration */
	gpio_init.GPIO_Pin = init->gpio_ck | init->gpio_si | init->gpio_so ;
	gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(init->gpio_periph, &gpio_init);

	/* GPIOs (not SPI related) configuration */
	gpio_init.GPIO_Pin = init->gpio_cs | init->gpio_rs | init->gpio_wp ;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(init->gpio_periph, &gpio_init);

	/* SPI configuration */
	spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi_init.SPI_Mode = SPI_Mode_Master;
	spi_init.SPI_DataSize = SPI_DataSize_8b;
	spi_init.SPI_CPOL = SPI_CPOL_High;
	spi_init.SPI_CPHA = SPI_CPHA_2Edge;
	spi_init.SPI_NSS = SPI_NSS_Soft;
	spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
	spi_init.SPI_CRCPolynomial = 7;

	SPI_Init(init->spi_periph, &spi_init);
	SPI_Cmd(SPI1, ENABLE);

	/* start with deasserted (set) CS */
	CS_DEASSERT(init);

	/* success */
	return 0;
}

/* Send a byte to the dataflash */
static void send(struct AT45DBxx_init* init, uint8_t data)
{
	/* Wait fo TX buffer empty */
	while(SPI_I2S_GetFlagStatus(init->spi_periph, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(init->spi_periph, data);

	while(SPI_I2S_GetFlagStatus(init->spi_periph, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(init->spi_periph);
}

/* Receive a byte from the dataflash */
static uint8_t receive(struct AT45DBxx_init* init)
{
	/* Wait fo TX buffer empty */
	while(SPI_I2S_GetFlagStatus(init->spi_periph, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(init->spi_periph, 0x00);

	while(SPI_I2S_GetFlagStatus(init->spi_periph, SPI_I2S_FLAG_RXNE) == RESET);
	return (uint8_t)SPI_I2S_ReceiveData(init->spi_periph);
}

/* Discover the dataflash type (ID, capacity, etc.) */
uint32_t AT45DBxx_read_id(struct AT45DBxx_init* init)
{
	/* SI: OPCODE */
	/* SO: Manufacturer ID, Device ID(part1), Device ID(part2), NOT USED */
	uint32_t id = 0;
	uint32_t i = 4;

	AT45DBxx_busy_wait(init);
	CS_ASSERT(init);
	send(init, OP_MANUFACTURER_DEVICE_ID);
	while(i-- > 0)
	{
		id |= receive(init) << (i * 8);
	}
	CS_DEASSERT(init);
	return id;
}

/* Wait until the dataflash is ready */
void AT45DBxx_busy_wait(struct AT45DBxx_init* init)
{
	/* SI: OPCODE */
	/* SO: Status register (bit 7 is the Ready) */
 	CS_ASSERT(init);
	send(init, OP_READ_STATUS_REG);
	while(!(receive(init) & 0x80));
	CS_DEASSERT(init);
}
