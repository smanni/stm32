#include "at45dbxx.h"

/* Init peripherals to talk with the dataflash
 *
 * @param init init data structure
 * @return 0 if succedeed, 1 otherwise
 */
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

	/* get the capacity in bytes */
	init->capacity = ID_TO_DENSITY_CODE(AT45DBxx_read_id(init)) * 1024 * 1024 / 8;

	/* success */
	return 0;
}

/* Send a byte to the dataflash
 *
 * @param init
 * @param data data to be written
 */
static void send(struct AT45DBxx_init* init, uint8_t data)
{
	/* Wait fo TX buffer empty */
	while(SPI_I2S_GetFlagStatus(init->spi_periph, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(init->spi_periph, data);

	while(SPI_I2S_GetFlagStatus(init->spi_periph, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(init->spi_periph);
}

/* Receive a byte from the dataflash
 *
 * @param init
 * @return received data
 */
static uint8_t receive(struct AT45DBxx_init* init)
{
	/* Wait fo TX buffer empty */
	while(SPI_I2S_GetFlagStatus(init->spi_periph, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(init->spi_periph, 0x00);

	while(SPI_I2S_GetFlagStatus(init->spi_periph, SPI_I2S_FLAG_RXNE) == RESET);
	return (uint8_t)SPI_I2S_ReceiveData(init->spi_periph);
}

/* Discover the dataflash type (ID, capacity, etc.)
 *
 * @param init
 * @return read ID
 */
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
		id |= receive(init) << (i * 8);
	CS_DEASSERT(init);
	return id;
}

/* Wait until the dataflash is ready
 *
 * @param init
 */
void AT45DBxx_busy_wait(struct AT45DBxx_init* init)
{
	/* SI: OPCODE */
	/* SO: Status register (bit 7 is the Ready) */
 	CS_ASSERT(init);
	send(init, OP_READ_STATUS_REG);
	while(!(receive(init) & 0x80));
	CS_DEASSERT(init);
}

/* SRAM buffers read (two buffers of 256 bytes)
 *
 * @param [OUT] buf buffer where store results
 * @param init
 * @param which buffer number
 * @param addr address to be read
 * @param count number of bytes to read
 * @return number of bytes read
 */
uint32_t AT45DBxx_buffer_read(uint8_t* buf, struct AT45DBxx_init* init, uint32_t which, uint32_t addr, uint32_t count)
{
	/* SI: OPCODE; two dont care bytes; BFA8-BFA0; one dont care byte
     * three address bytes comprised of 16 don’t care bits and 8 buffer address bits (BFA7 - BFA0).
     *
	 * SO: data (n; n+1, ...)
	 */

	uint32_t i = 0;

	/* check arguments */
	if(!buf)
		return 0;
	if(which != 1 && which != 2)
		return 0;
	if(addr + count > BUF_SIZE)
		return 0;

	AT45DBxx_busy_wait(init);
	CS_ASSERT(init);
	send(init, which == 1 ? OP_BUF1_READ : OP_BUF2_READ);
	send(init, 0x00);
	send(init, 0x00);
	send(init, addr);
	send(init, 0x00);
	while(i < count)
		buf[i++] = receive(init);
	CS_DEASSERT(init);

	return count;
}

/* Main memory page read (pages of 256 bytes)
 *
 * @param [OUT] buf buffer where store results
 * @param init
 * @param page page address
 * @param addr address within the page to be read
 * @param count number of bytes to read
 * @return number of bytes read
 */
uint32_t AT45DBxx_page_read(uint8_t* buf, struct AT45DBxx_init* init, uint32_t page, uint32_t addr, uint32_t count)
{
	/* SI: OPCODE; A18-A16; A15-A8; A7-A0; four dont care bytes
	 * The first 11 bits (A18 - A8) of the 19-bits sequence specify which page of the main memory array to read,
	 * and the last 8 bits (A7 - A0) of the 19-bits address sequence specify the starting byte address within the page
	 *
	 * SO: data (n; n+1, ...)
	 */

	uint32_t i = 0;

	/* check arguments */
	if(!buf)
		return 0;
	if(page > (init->capacity / PAGE_SIZE) || (addr + count) > PAGE_SIZE)
		return 0;

	AT45DBxx_busy_wait(init);
	CS_ASSERT(init);
	send(init, OP_PAGE_READ);
	send(init, page >> 8);
	send(init, page & 0xff);
	send(init, addr);
	send(init, 0x00);
	send(init, 0x00);
	send(init, 0x00);
	send(init, 0x00);
	while(i < count)
		buf[i++] = receive(init);
	CS_DEASSERT(init);

	return count;
}

/* SRAM buffers write (two buffers of 256 bytes)
 *
 * @param buf buffer containing data to be written
 * @param init
 * @param which buffer number
 * @param addr address to be written
 * @param count number of bytes to write
 * @return number of bytes written
 */
uint32_t AT45DBxx_buffer_write(uint8_t* buf, struct AT45DBxx_init* init, uint32_t which, uint32_t addr, uint32_t count)
{
	/* SI: OPCODE; two dont care bytes; BFA8-BFA0; data(n); data(n+1); ...
     * three address bytes comprised of 16 don’t care bits and 8 buffer address bits (BFA7 - BFA0).
	 */

	uint32_t i = 0;

	/* check arguments */
	if(!buf)
		return 0;
	if(which != 1 && which != 2)
		return 0;
	if(addr + count > BUF_SIZE)
		return 0;

	AT45DBxx_busy_wait(init);
	CS_ASSERT(init);
	send(init, which == 1 ? OP_BUF1_WRITE : OP_BUF2_WRITE);
	send(init, 0x00);
	send(init, 0x00);
	send(init, addr);
	while(i < count)
		send(init, buf[i++]);
	CS_DEASSERT(init);

	return count;
}

/* Main memory page write (pages of 256 bytes) through internal buffers
 *
 * @param buf buffer containing data to be written
 * @param init
 * @param page page address
 * @param addr address to be written within the page
 * @param count number of bytes to write
 * @return number of bytes written
 */
uint32_t AT45DBxx_page_write(uint8_t* buf, struct AT45DBxx_init* init, uint32_t page, uint32_t addr, uint32_t count)
{
	/* SI: OPCODE; A18-A16; A15-A8; A7-A0; data(n); data(n+1); ...
	 * The first 11 bits (A18 - A8) of the 19-bits sequence specify which page of the main memory array to read,
	 * and the last 8 bits (A7 - A0) of the 19-bits address sequence specify the starting byte address within the page
	 */

	uint32_t i = 0;

	/* check arguments */
	if(!buf)
		return 0;
	if(page > (init->capacity / PAGE_SIZE) || (addr + count) > PAGE_SIZE)
		return 0;

	AT45DBxx_busy_wait(init);
	CS_ASSERT(init);
	send(init, OP_PAGE_WRITE);
	send(init, page >> 8);
	send(init, page & 0xff);
	send(init, addr);
	while(i < count)
		send(init, buf[i++]);
	CS_DEASSERT(init);

	return count;
}

/* Main memory page erase (pages of 256 bytes)
 *
 * @param init
 * @param page page address
 * @return 0 if succedeed, 1 otherwise
 */
uint32_t AT45DBxx_page_erase(struct AT45DBxx_init* init, uint32_t page)
{
	/* SI: OPCODE; A18-A16; A15-A8; dont care byte
	 * three address bytes consist of 5 don’t care bits, 11 page address bits (A18 - A8) that specify the
	 * page in the main memory to be erased and 8 don’t care bits
	 */

	/* check arguments */
	if(page > (init->capacity / PAGE_SIZE))
		return 1;

	AT45DBxx_busy_wait(init);
	CS_ASSERT(init);
	send(init, OP_PAGE_ERASE);
	send(init, page >> 8);
	send(init, page & 0xff);
	send(init, 0x00);
	CS_DEASSERT(init);

	return 0;
}
