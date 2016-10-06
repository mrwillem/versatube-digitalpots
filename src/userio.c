#include "hw_config.h"
#include "userio.h"
#include "midi.h"
#include "main.h"
#include "i2c.h"

uint8_t GL_floattextbuffer[28];
uint8_t GL_floattext;
uint8_t GL_floattextstart;
uint8_t GL_floattextcharacters;

uint8_t input_dispatch(uint8_t message, uint8_t var,  channelconfig *thischannel)
{
	uint8_t number;
	uint8_t direction;
	if(message==MSG_POTITURN)
	{
		if(var & MSG_POTILEFT)
		{
			number=(0b00001111 & var);
			if(GL_displaydata.displaybuffer.poti[number] > 0)
			{
				thischannel->poti[potimap[thischannel->channel][number]]--;
				GL_displaydata.displaybuffer.poti[number]=thischannel->poti[potimap[thischannel->channel][number]];
				poti_display_data(number, &GL_displaydata.displaybuffer.poti[number], 1);
			}

		}
		else if( var & MSG_POTIRIGHT)
		{
			number=(0b00001111 & var);
			if(GL_displaydata.displaybuffer.poti[number] < 63)
			{
				thischannel->poti[potimap[thischannel->channel][number]]++;
				GL_displaydata.displaybuffer.poti[number]=thischannel->poti[potimap[thischannel->channel][number]];
				poti_display_data(number, &GL_displaydata.displaybuffer.poti[number], 1);
			}

		}
	}
	if(message==MSG_BUTTONPRESS)
	{
		configmenu(var);
	}
}

void channelled_display_channel(uint8_t channel)
{
	if(channel>12)
		channel=12;
	Buffer_Tx2[0]=I2C_MESSAGE_DISPLAYDATA;
	Buffer_Tx2[1]=0x07;
	Buffer_Tx2[2]=channel;
	I2C_Master_BufferWrite(I2C2, Buffer_Tx2, 3, 0x34);
}

void display_blink(uint8_t channel)
{
	Buffer_Tx2[0]=I2C_MESSAGE_DISPLAYDATA;
	Buffer_Tx2[1]=0x0B;
	Buffer_Tx2[2]=channel;
	I2C_Master_BufferWrite(I2C2, Buffer_Tx2, 3, 0x34);
}
void segment_display_number(uint8_t number )
{
	uint8_t a,b,c;
	GL_floattext=0;
	while(number > 128)
	{
		number -=100;
	}
	a=number%10;
	number/=10;
	b=number%10;
	number/=10;
	c=number;
	Buffer_Tx2[0]=I2C_MESSAGE_DISPLAYDATA;
	Buffer_Tx2[1]=0x08;
	Buffer_Tx2[2]=c;
	Buffer_Tx2[3]=b;
	Buffer_Tx2[4]=a;
	I2C_Master_BufferWrite(I2C2, Buffer_Tx2, 5, 0x34);
}

void segment_display_text(char *text)
{
	GL_floattext=0;
	Buffer_Tx2[0]=I2C_MESSAGE_DISPLAYDATA;
	Buffer_Tx2[1]=0x08;
	Buffer_Tx2[2]=text[0];
	Buffer_Tx2[3]=text[1];
	Buffer_Tx2[4]=text[2];
	I2C_Master_BufferWrite(I2C2, Buffer_Tx2, 5, 0x34);
}

void segment_display_float_text(char *text, uint8_t characters)
{
	uint8_t i;
	if(characters>25)
		characters=25;
	for(i=0; i<characters; i++)
	{
		GL_floattextbuffer[i]=text[i];
	}
	GL_floattextbuffer[i]=0x20;
	GL_floattextbuffer[i+1]=0x20;
	GL_floattextbuffer[i+2]=GL_floattextbuffer[0];
	GL_floattextbuffer[i+3]=GL_floattextbuffer[1];
	GL_floattextcharacters=characters+1;
	GL_floattextstart=0;
	GL_floattext=1;
}

void segment_display_float_text_update(void)
{
	uint8_t i;

	Buffer_Tx2[0]=I2C_MESSAGE_DISPLAYDATA;
	Buffer_Tx2[1]=0x08;


	for(i=0; i<3; i++)
	{
		Buffer_Tx2[(i+2)]=GL_floattextbuffer[(i+GL_floattextstart)];
	}
	I2C_Master_BufferWrite(I2C2, Buffer_Tx2, 5, 0x34);

	if(GL_floattextstart<(GL_floattextcharacters))
	{
		GL_floattextstart++;
	}
	else
	{
		GL_floattextstart=0;
	}


}

void poti_display_data(uint8_t firstpoti, uint8_t* value, uint8_t num)
{
	uint8_t i;
	for(i=0; i<num; i++)
	{
		if(value[i] > 63)
			value[i]=63;
		Buffer_Tx2[i+2]=value[i];
	}
	if(firstpoti>6)
		firstpoti=0;
	Buffer_Tx2[1]=firstpoti;
	Buffer_Tx2[0]=I2C_MESSAGE_DISPLAYDATA;
	I2C_Master_BufferWrite(I2C2, Buffer_Tx2, (num+2), 0x34);
}

void userio_setup(void)
{
	/* Setup Ports for midi level translator */
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

 	RCC_APB2PeriphClockCmd(USERIO_IRQ_PERIPH, ENABLE);
 	GPIO_InitStructure.GPIO_Pin =  USERIO_IRQ_PIN;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(USERIO_IRQ_GPIO, &GPIO_InitStructure);


 	GPIO_EXTILineConfig(USERIO_IRQ_PORTSOURCE, USERIO_IRQ_PINSOURCE);

 	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
 	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
 	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
 	EXTI_Init(&EXTI_InitStructure);

 	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
 	NVIC_Init(&NVIC_InitStructure);

}

void userio_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line1);
  systemevent=(systemevent | SYSEV_GETI2CMESSAGE);
}

const uint8_t potimap[13][7] =
{
	{0, 0, 0, 0, 0, 0, 0},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
	{1, 2, 3, 4, 5, 6, 7},
};
