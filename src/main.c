/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file provides main program functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "hw_config.h"
#include <stdio.h>
#include "stm32f10x_crc.h"
#include "midi.h"
#include "usart.h"
#include "spi.h"
#include "eeprom.h"
#include "userio.h"
#include "configmenu.h"

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @defgroup Main
  * @brief Main program body
  * @{
  */

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @defgroup Main
  * @brief Main program body
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#if defined(USE_STM32100E_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08080000)
#elif defined(USE_STM322xG_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08100000)
#elif defined(USE_STM3210C_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08040000)
#endif
/* Private macros ------------------------------------------------------------*/

/**
  * @brief   Small printf for GCC/RAISONANCE
  */
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#endif /* __GNUC__ */

/* Private variables ---------------------------------------------------------*/

uint8_t LEDON;
uint8_t RAMPLED;
uint8_t counter;
volatile uint8_t irqvals;
volatile uint16_t systemevent;


/* Private function prototypes -----------------------------------------------*/
void Delay_us(int);
void Timer_Setup(void);
displaydata GL_displaydata;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */


int main(void)
{
	int8_t tmp;
	uint8_t send_data;
	uint8_t i;
	channelconfig thischannel;
	send_data=0;
	int8_t value[16];
	uint8_t message[3];
	value[1]=0;
	systemevent=0;
	uint8_t current_midichannel;
	current_midichannel=0xFF;
	SPI2_BLOCK=SPI_BLOCK_FREE;

	/* Wait for power good signal */
	setup_powergood();
	while(powergood())
	{
	}
	/* Startup code for clock etc ... */
	RCC_Configuration();

	/* Device specific NVIC Configuration */
	NVIC_Configuration();


	InitializeTimer();
	/* Load start configuration parameters */
	// LoadConfig();
	midichannel=1;


	/*
	 * Before initializing the SPI hardware setup each SPI hardware part
	 * thus all Chip Select lines should be in theyr default state up on SPI Bus
	 * initialization.
	 */
	eeprom_hardware_init();
	led_controller_hardware_init();
	poti_hardware_init();


	SPI2_BusInit();

	userio_setup();
	I2C_LowLevel_Init(I2C2);
	I2C_Slave_BufferReadWrite(I2C2);

	configmenu_init();

	/* Now everything is set up start listening on the midi port */

	midi_setup();
	USART_2_Configuration();
	midi_enable();
	for(i=0; i<13; i++)
		GL_displaydata.i2cdisplaybuffer[i]=0;
	segment_display_float_text("Hello Willem", 12);

	/* read default channel parameters before entering main loop */
	eeprom_read((midichannel*128), 128, &eeprom_rx_buffer);
	fill_channelconfig_struct(&thischannel, &eeprom_rx_buffer, midichannel);

	/* Infinite main loop */
	while (1)
	{
		if(systemevent)
		{
			if(systemevent & SYSEV_MIDICHAN_RX)
			{
				if(current_midichannel != midichannel)
				{
					// Load Parameters
					eeprom_read((midichannel*128), 128, &eeprom_rx_buffer);
					fill_channelconfig_struct(&thischannel, &eeprom_rx_buffer, midichannel);

					current_midichannel=midichannel;
					// Update Parameters
					// Update Display
					systemevent= (systemevent & (~SYSEV_MIDICHAN_RX));
				}
			}
			if(systemevent & SYSEV_MIDICC_RX)
			{
				systemevent= (systemevent & (~SYSEV_MIDICC_RX));
			}
			if((systemevent & SYSEV_GETI2CMESSAGE) && (GPIO_ReadInputDataBit(USERIO_IRQ_GPIO, USERIO_IRQ_PIN)== 1))
			{
				I2C_Master_BufferRead(I2C2, Buffer_Rx2, 3, 0x34);
				systemevent= (systemevent & (~SYSEV_GETI2CMESSAGE));
				message[0]=Buffer_Rx2[1];
				message[1]=Buffer_Rx2[2];
				Buffer_Tx2[0]=I2C_MESSAGE_ACKNO;
				Buffer_Tx2[1]=Buffer_Rx2[0];
				I2C_Master_BufferWrite(I2C2, Buffer_Tx2, 2, 0x34);
				input_dispatch(message[0], message[1]);
				systemevent= (systemevent & (~SYSEV_GETI2CMESSAGE));
			}
			if(systemevent & SYSEV_FLOATTEXT_UPDATE)
			{
				segment_display_float_text_update();
				systemevent= (systemevent & (~SYSEV_FLOATTEXT_UPDATE));
			}
		}
	}
}

void InitializeTimer()
{
	/* Setup a Timer each second */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
    TIM_TimeBase_InitStructure.TIM_Prescaler = 7200;
    TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBase_InitStructure.TIM_Period = 5000;
    TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBase_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBase_InitStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* Enable the Timer Interrupt */
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    nvicStructure.NVIC_IRQChannelSubPriority = 0x0;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);

    /* All is set up, enable timer */
    TIM_Cmd(TIM3, ENABLE);
}


void TIM3_IRQHandler()
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if(GL_floattext)
		{
			systemevent = (systemevent | SYSEV_FLOATTEXT_UPDATE);
		}
	}
}


void Timer_Setup(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 1000 - 1; //ms
    TIM_TimeBaseStructure.TIM_Prescaler = 42 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    //TIM_Cmd(TIM2,ENABLE);
}


void Delay_us( int nTime)
{
    u16 counter=nTime&0xffff;
    TIM_Cmd(TIM2,ENABLE);
    TIM_SetCounter(TIM2,counter);
    while(counter>1)
    {
        counter=TIM_GetCounter(TIM2);
    }
    TIM_Cmd(TIM2,DISABLE);
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
