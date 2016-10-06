#ifndef USERIO_H
#define USERIO_H


extern uint8_t GL_floattextbuffer[28];
extern uint8_t GL_floattext;
extern uint8_t GL_floattextstart;
extern uint8_t GL_floattextcharacters;


uint8_t input_dispatch(uint8_t, uint8_t);
void channelled_display_channel(uint8_t);
void userio_setup(void);
void userio_IRQHandler(void);
void segment_display_number(uint8_t);
void segment_display_text(char *);
void segment_display_float_text(char *, uint8_t);
void segment_display_float_text_update(void);
void poti_display_data(uint8_t, uint8_t*, uint8_t);
void display_blink(uint8_t);

#define USERIO_IRQ_PERIPH RCC_APB2Periph_GPIOB
#define USERIO_IRQ_PIN GPIO_Pin_1
#define USERIO_IRQ_GPIO GPIOB
#define USERIO_IRQ_PINSOURCE GPIO_PinSource1
#define USERIO_IRQ_PORTSOURCE GPIO_PortSourceGPIOB

#define MSG_POTITURN 1
#define MSG_BUTTONPRESS 2
#define MSG_LEDRING 3
#define MSG_CHANNELLED 4
#define MSG_SEGMENT 5
#define MSG_LEDVALUES 6

#define MSG_POTILEFT 16
#define MSG_POTIRIGHT 32

#define BLINKRING1 1
#define BLINKRING2 2
#define BLINKRING3 3
#define BLINKRING4 4
#define BLINKRING5 5
#define BLINKRING6 6
#define BLINKRING7 7
#define BLINKSEGMENT 8
#define BLINKCHANNEL 9



#endif //USERIO_H
