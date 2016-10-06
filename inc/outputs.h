#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <stdint.h>

#define POTI_CS_PIN GPIO_Pin_8
#define POTI_CS_GPIO_PORT GPIOA
#define POTI_CS_PERIPH RCC_APB2Periph_GPIOA
#define LED_LE_PIN GPIO_Pin_6
#define LED_LE_GPIO_PORT GPIOA
#define LED_LE_PERIPH RCC_APB2Periph_GPIOA
#define LED_OE_PIN GPIO_Pin_7
#define LED_OE_GPIO_PORT GPIOA
#define LED_OE_PERIPH RCC_APB2Periph_GPIOA

typedef struct {
	uint8_t poti[18];
	uint32_t outregister;
	uint8_t channel;
	uint8_t midi_channel;
	uint8_t midi_cc_channel;
	uint8_t midi_cc_param;
	uint8_t midi_cc_low;
	uint8_t midi_cc_high;
	uint8_t midi_channel_send;
} channelconfig;

#define CHANNELRED   0b00000000 00000000 00000000 00000001
#define CHANNELGREEN 0b00000000 00000000 00000000 00000010
#define CHANNELBLUE  0b00000000 00000000 00000000 00000100

#endif //OUTPUTS_H
