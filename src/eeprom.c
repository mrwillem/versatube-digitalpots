#include "spi.h"
#include "eeprom.h"
#include "hw_config.h"
#include "core_cm3.h"

/*
 * The eeprom_data_buffer can be used by software for data,
 * this buffer can be manipulated by the software
 * the eeprom command buffer is to provide the address and other commands to the spi interface
 * this buffer should only be manipulated by the eeprom routines and thus is neither extern nor volatile
 */
volatile uint8_t eeprom_tx_buffer[128];
uint8_t eeprom_rx_buffer[128];
uint8_t eeprom_command_buffer[6];

/*
 * variables that are needed in the spi function
 */
volatile uint32_t eeprom_rx_address;
volatile uint32_t eeprom_tx_address;
volatile uint16_t eeprom_nbytes;

/*
 * Set up Flash GPIO Ports
 * In Case of the M95M01 it is only the Chip Select line
 * The SPI lines are configured in general SPI Configuration
 *
 */

void eeprom_hardware_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(EEPROM_CS_PERIPH , ENABLE);

	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = EEPROM_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(EEPROM_CS_GPIO_PORT, &GPIO_InitStructure);

	/* Set Pin to its default state, high in Case of the M95M01 */
	GPIO_WriteBit(EEPROM_CS_GPIO_PORT, EEPROM_CS_PIN, Bit_SET);
}


/*
 * Write Disable
 */
uint8_t eeprom_wrdi(void)
{
	eeprom_command_buffer[0]=EEPROM_WRDI;
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_EEPROM_WRITE;
	SPI2_send(1, SPI_BLOCK_EEPROM_WRITE, (uint32_t)eeprom_command_buffer, (uint32_t)eeprom_command_buffer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	return 0;
}

/*
 * Write Enable
 * Needs to be called before writing to EEPROM or status register
 */
uint8_t eeprom_wren(void)
{
	eeprom_command_buffer[0]=EEPROM_WREN;
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_EEPROM_WRITE;
	SPI2_send(1, SPI_BLOCK_EEPROM_WRITE, (uint32_t)eeprom_command_buffer, (uint32_t)eeprom_command_buffer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	return 0;
}

/*
 * Read the EEPROM Status Register
 */
uint8_t eeprom_rdsr(void)
{
	eeprom_command_buffer[0]=eeprom_rdsr;
	eeprom_command_buffer[1]=0x00;
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_EEPROM_WRITE;
	SPI2_send(2, SPI_BLOCK_EEPROM_WRITE, (uint32_t)eeprom_command_buffer, (uint32_t)eeprom_command_buffer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	return eeprom_command_buffer[1];
}


/*
 * A function that reads nbytes at the given address to the rxbuffer
 */

uint8_t eeprom_read(uint32_t address, uint16_t nbytes, uint32_t rxbuffer_address)
{
	uint32_t revaddress;
	uint8_t* addresspointer;
	eeprom_rx_address=rxbuffer_address;
	eeprom_tx_address=rxbuffer_address;
	eeprom_nbytes = nbytes;
	eeprom_command_buffer[0] = EEPROM_READ;
	revaddress=__REV(address);
	addresspointer=&revaddress;

	eeprom_command_buffer[1]=((addresspointer[1] * 0x0802LU & 0x22110LU) | (addresspointer[1] * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
	eeprom_command_buffer[2]=((addresspointer[2] * 0x0802LU & 0x22110LU) | (addresspointer[2] * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
	eeprom_command_buffer[3]=((addresspointer[3] * 0x0802LU & 0x22110LU) | (addresspointer[3] * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;

	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK = SPI_BLOCK_EEPROM_COMMAND;
	SPI2_send(4, SPI_BLOCK_EEPROM_COMMAND, (uint32_t)eeprom_command_buffer, (uint32_t)rxbuffer_address);
	return 0;
}


/*
 * A function to program a 256 byte block at the given address
 * the programmed block is found at the txbuffer_address
 */
uint8_t eeprom_write(uint32_t address, uint32_t txbuffer_address, uint8_t nbytes)
{
	uint8_t tmpdata;
	uint32_t revaddress;
	uint8_t *addresspointer;


	/* initialize variable */
	tmpdata=0xFF;

	/* Set up the data to write */
	eeprom_nbytes = nbytes;
	eeprom_rx_address=txbuffer_address;
	eeprom_tx_address=txbuffer_address;
	/* First of all send just the Write Enable */
	eeprom_wren();
	/* Next Program the write command into the command buffer */
	eeprom_command_buffer[0]=EEPROM_WRITE;
	/* The address needs to be reversed because of the endianess of the system */
	revaddress = __REV(address);
	/* Then each address byte has to be reversed on its own because data is transmitted LSB first */
	addresspointer=&revaddress;
	eeprom_command_buffer[1]=((addresspointer[1] * 0x0802LU & 0x22110LU) | (addresspointer[1] * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
	eeprom_command_buffer[2]=((addresspointer[2] * 0x0802LU & 0x22110LU) | (addresspointer[2] * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
	eeprom_command_buffer[3]=((addresspointer[3] * 0x0802LU & 0x22110LU) | (addresspointer[3] * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;


	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK = SPI_BLOCK_EEPROM_COMMAND;
	SPI2_send(4, SPI_BLOCK_EEPROM_COMMAND, (uint32_t)eeprom_command_buffer, (uint32_t)eeprom_command_buffer);
	tmpdata=0;
	while(SPI2_BLOCK != SPI_BLOCK_FREE)
	{
		if(DMA_GetITStatus(DMA1_IT_TC4))
		{
			tmpdata=2;
		}
	}
	/* Next have a look whether the command was successfull */
	tmpdata=2;
	tmpdata=EEPROM_RDSR();
	/* if bit 1 is set, writing is in progress */
	while(tmpdata & 0b10000000)
	{
		tmpdata=EEPROM_RDSR();

	}
	return 0;

}

