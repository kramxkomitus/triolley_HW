#ifndef INC_UART_H_
#define INC_UART_H_

#include "stdbool.h"
#include "inttypes.h"
#include "string.h"
#include "usart.h"



bool uart_send_mes_IT(uint8_t *message);
bool uart_send_mes(uint8_t *message, uint32_t Timeout);
bool uart_ask_str_IT(uint8_t *cur_destignation);
void uart_str_RxCPLTCallback();
// void uart_error(enum ERROR error);

#endif /* INC_APP_H_ */
