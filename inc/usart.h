#ifndef USART_H
#define USART_H

extern volatile char BufferUsart2[32];


void USART_2_Configuration(void);

void USART2_InterruptHandler(void);

#endif //USART_H
