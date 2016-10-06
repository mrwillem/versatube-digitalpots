#ifndef EEPROM_H
#define EEPROM_H

#define EEPROM_CS_GPIO_PORT GPIOB
#define EEPROM_CS_PERIPH RCC_APB2Periph_GPIOB
#define EEPROM_CS_PIN GPIO_Pin_12


/*
 * Define EEPROM Command bytes
 * Note: The bytes are in reverse order because the eeprom needs MSB first data
 * but the SPI Bus is configured LSB first due to the SPI POTIs
 */

#define EEPROM_WREN 0x60
#define EEPROM_WRDI 0x20
#define EEPROM RDSR 0xA0
#define EEPROM_WRSR 0x80
#define EEPROM_READ 0xC0
#define EEPROM_WRITE 0x40

void EEPROM_Hardware_Init(void);
uint8_t eeprom_wrdi(void);
uint8_t eeprom_wren(void);
uint8_t eeprom_rdsr(void);
uint8_t eeprom_read(uint32_t, uint16_t, uint32_t);
uint8_t eeprom_write(uint32_t, uint32_t, uint8_t);

extern volatile uint32_t eeprom_rx_address;
extern volatile uint32_t eeprom_tx_address;
extern volatile uint16_t eeprom_nbytes;
extern volatile uint8_t eeprom_tx_buffer[128];
extern uint8_t eeprom_rx_buffer[128];
extern uint8_t eeprom_command_buffer[6];

#endif //EEPROM_H
