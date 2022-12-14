#include "uart_app.h"

#define Tx_buff_size 32
#define Rx_buff_size 32

static uint8_t stop_byte = '\n';
static uint8_t Tx_buff[Tx_buff_size] = {};
static uint8_t Rx_buff[Rx_buff_size] = {};
static uint8_t *Rx_str_return_ptr = NULL;
static uint8_t *Rx_ptr = NULL;
static uint8_t *Rx_end_ptr = &Rx_buff[Rx_buff_size];

static enum {
  FREE,
  BUSY
} Tx_state = FREE,
  Rx_state = FREE;




static enum ERROR {
  ER_RX_BUF_OVERFLOW,
  ER_RX_BUF_SZ,
  ER_ACCESS,
  ER_INPUT,
  ER_HAL
};


bool uart_send_mes_IT(uint8_t *message)
{
  if (Tx_state == BUSY)
  {
    uart_error(ER_ACCESS);
    return false;
  }
  else
  {
    if (strlen(message) > Tx_buff_size)
    {
      uart_error(ER_INPUT);
      return false;
    }
    Tx_state = BUSY;
    strcpy(Tx_buff, message);
    if (HAL_OK == HAL_UART_Transmit_IT(&huart1, Tx_buff, strlen(Tx_buff)))
      return true;
    else
    {
      uart_error(ER_HAL);
      return false;
    }
  }
}

bool uart_send_mes(uint8_t *message, uint32_t Timeout)    //неправильно работает блокирующий режим, Timeout неверно работает, переписать
{
  if (Tx_state == BUSY)
  {
    uart_error(ER_ACCESS);
    return false;
  }
  else
  {
    if (strlen(message) > Tx_buff_size)
    {
      uart_error(ER_INPUT);
      return false;
    }
    Tx_state = BUSY;
    strcpy(Tx_buff, message);
    if (HAL_OK == HAL_UART_Transmit(&huart1, Tx_buff, strlen(Tx_buff), Timeout))
    {
      Tx_state = FREE;
      return true;
    }
    else
    {
      uart_error(ER_HAL);
      return false;
    }
  }
}

__weak void uart_str_RxCPLTCallback()
{
}

bool uart_ask_str_IT(uint8_t *str)
{
  if (Rx_state == BUSY)
  {
    uart_error(ER_INPUT);
    return false;
  }
  else
  {
    if (strlen(str) > Rx_buff_size)
    {
      uart_error(ER_RX_BUF_SZ);
      return false;
    }
    Rx_state = BUSY;
    Rx_str_return_ptr = str;
    Rx_ptr = Rx_buff;
    if (HAL_OK == HAL_UART_Receive_IT(&huart1, Rx_ptr, 1))
      return true;
    else
    {
      uart_error(ER_HAL);
      return false;
    }
  }
}

void uart_error(enum ERROR error)
{
  switch (error)
  {
  case ER_RX_BUF_OVERFLOW:
    Rx_state = FREE;
    break;
  case ER_ACCESS:
    break;
  case ER_INPUT:
    break;
  default:
    break;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    if (*Rx_ptr != stop_byte)
    {
      Rx_ptr++;
      HAL_UART_Receive_IT(&huart1, Rx_ptr, 1);
    }
    else
    {      
      memcpy(Rx_str_return_ptr, Rx_buff, strlen(Rx_buff) - 1);
      Rx_str_return_ptr[strlen(Rx_buff) - 1] = 0;
      memset(Rx_buff, 0, Rx_buff_size);
      Rx_state = FREE;
      uart_str_RxCPLTCallback();
    }
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    if (Tx_state == BUSY)
      Tx_state = FREE;
  }
}