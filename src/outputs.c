
#include "outputs.h"
#include "spi.h"

void fill_channelconfig_struct( channelconfig *thischannel, uint8_t *rxdata, uint8_t midichannel)
{
	uint8_t i;
	for(i=0; i<18; i++)
	{
		thischannel->poti[i]=rxdata[i];
	}
	thischannel->outregister=((rxdata[17]<<24) | (rxdata[18]<< 16) | (rxdata[19] << 8) | (rxdata [20]));
	thischannel->midi_cc_channel=rxdata[21];
	thischannel->midi_cc_param=rxdata[22];
	thischannel->midi_cc_low=rxdata[23];
	thischannel->midi_cc_high=rxdata[24];
	thischannel->midi_channel_send=rxdata[25];
	thischannel->channel=rxdata[26];
	thischannel->midi_channel=midichannel;
}

void fill_eeprom_txbuffer( channelconfig *thischannel, uint8_t *txdata)
{
	uint8_t i;
	uint8_t *regpointer;
	regpointer = &(thischannel->outregister);
	txdata[17]=regpointer[0];
	txdata[18]=regpointer[1];
	txdata[19]=regpointer[2];
	txdata[20]=regpointer[3];
	for(i=0; i<18; i++)
	{
		txdata[i]=thischannel->poti[i];
	}

}
void led_controller_hardware_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(LED_LE_PERIPH , ENABLE);
	RCC_APB2PeriphClockCmd(LED_OE_PERIPH , ENABLE);
	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LED_LE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_LE_GPIO_PORT, &GPIO_InitStructure);

	/* Set latch enable pin of the led controller to low state */
	GPIO_WriteBit(LED_LE_GPIO_PORT, LED_LE_PIN, Bit_RESET);


	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LED_OE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_OE_GPIO_PORT, &GPIO_InitStructure);

	/* Set the OUTPUT ENABLE Pin of the LED Controller HIGH. This disables output. So all LDRs are off */
	GPIO_WriteBit(LED_OE_GPIO_PORT, LED_OE_PIN, Bit_SET);
}

void poti_hardware_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(POTI_CS_PERIPH , ENABLE);

	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = POTI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(POTI_CS_GPIO_PORT, &GPIO_InitStructure);

	/* Set Pin to its default state, high in Case of the M95M01 */
	GPIO_WriteBit(POTI_CS_GPIO_PORT, POTI_CS_PIN, Bit_RESET);
}

