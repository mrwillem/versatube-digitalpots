
#ifndef MIDI_H
#define MIDI_H

#define MIDI_EN_GPIO GPIOA
#define MIDI_EN_PERIPH RCC_APB2Periph_GPIOA
#define MIDI_EN_PIN GPIO_Pin_1

#define SYSEV_MIDICHAN_RX      0b0000000000000001
#define SYSEV_MIDICC_RX        0b0000000000000010
#define SYSEV_GETI2CMESSAGE    0b0000000000000100
#define SYSEV_FLOATTEXT_UPDATE 0b1000000000000000

extern uint8_t midioutstate;
extern volatile uint8_t midichannel;
extern volatile uint8_t midiccvalue;


void midi_setup(void);
void midi_enable(void);
void midi_disable(void);
uint8_t midi_message_dispatcher(char * RxBuffer);


#endif //MIDI_H
