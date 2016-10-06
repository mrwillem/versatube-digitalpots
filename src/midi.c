#include <stdio.h>
#include <stdint.h>
#include "hw_config.h"
#include "midi.h"
#include "main.h"

volatile uint8_t midichannel;
volatile uint8_t midiccvalue;

void midi_setup(void)
{
	/* Setup Ports for midi level translator */
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(MIDI_EN_PERIPH, ENABLE);
 	GPIO_InitStructure.GPIO_Pin =  MIDI_EN_PIN;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_Init(MIDI_EN_GPIO, &GPIO_InitStructure);
 	midi_disable();
}
void midi_enable(void)
{
	/* Enable the midi level translator */
	MIDI_EN_GPIO->BSRR = MIDI_EN_PIN;
}

void midi_disable(void)
{
	/* Disable the midi level translator */
	MIDI_EN_GPIO->BRR = MIDI_EN_PIN;
}

uint8_t midi_message_dispatcher(char * RxBuffer)
{
	/* Test for program changee */
	if((RxBuffer[0] & 0xC0) == 0xC0)
	{
		if(RxBuffer[1] != 255)
		{
			if(midichannel != RxBuffer[1])
			{
				midichannel=RxBuffer[1];
				systemevent = systemevent | SYSEV_MIDICHAN_RX;
			}
			return 0;
		}
	}
	/* Test for Controller message */
	else if((RxBuffer[0] & 0xB0) == 0xB0)
	{
		if(RxBuffer[2] != 255)
		{
			if(midiccvalue != RxBuffer[2])
			{
				midiccvalue=RxBuffer[2];
				systemevent = systemevent | SYSEV_MIDICC_RX;
			}
			return 0;
		}
	}
	return 1;
}


