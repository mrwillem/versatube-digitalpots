#include "hw_config.h"
#include "usart.h"
#include "midi.h"
#include "stm32f10x_usart.h"


volatile char BufferUsart2[32];


void USART_2_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef   USART_InitStructure;

	// Enable periphals
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;        // we want to configure the USART2 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// this sets the priority group of the USART2 interrupts
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       // this sets the subpriority inside the group
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          // the USART2 interrupts are globally enabled
    NVIC_Init(&NVIC_InitStructure);

    /* Define Pin Tx2 as Push-Pull Output */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Define Pin Rx2 as Input Floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);



    // Initialize USART
    USART_InitStructure.USART_BaudRate = 31250;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    /* Configure USART2 */
    USART_Init(USART2, &USART_InitStructure);

    /* Enable the USART */
    USART_Cmd(USART2, ENABLE);
    /* enable the USART2 receive not empty interrupt */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    /* Enable USART2 global interrupt */
    NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_InterruptHandler(void)
{
  static uint8_t rx_index = 0;



  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // Received characters modify string
  {
	  BufferUsart2[rx_index++] = USART_ReceiveData(USART2);
	  if (rx_index > 1)
	  {
	  	  if(midi_message_dispatcher(&BufferUsart2) == 0)
	  	  {
	  		  rx_index=0;
	  		  BufferUsart2[0]=0xFF;
	  		  BufferUsart2[1]=0xFF;
	  		  BufferUsart2[2]=0xFF;

	  	  }
	  }
	  if (rx_index >= 6)
		  rx_index = 0;
  }
}

