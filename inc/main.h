/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

//void EnableTimerInterrupt();
void InitializeTimer();
extern uint8_t RESET_VALUES;
extern volatile uint8_t irqvals;
extern volatile uint16_t systemevent;


typedef struct {
	uint8_t poti[7];
	uint8_t channel;
	uint8_t segment[3];
	uint8_t blinkled;
	uint8_t midichannel;
} displaybufferstruct;

typedef union
{
	uint8_t i2cdisplaybuffer[12];
	displaybufferstruct displaybuffer;

} displaydata __attribute__((packed));

extern displaydata GL_displaydata;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */ 
#ifdef __cplusplus
}
#endif


#endif /* __MAIN_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
