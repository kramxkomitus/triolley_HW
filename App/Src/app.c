#include "app.h"
#include "stdbool.h"
#include "main.h"
#include "uart_app.h"
#include "tim.h"
#include "drive.h"
#include "string.h"
#include "stdio.h"
#define command_size 32

uint8_t command[command_size] = {0};
bool flag = false;
uint32_t val;
static struct drive drv_l, drv_r;

void app()
{
    uart_send_mes("Hardware is started...\n", 100);
    uart_ask_str_IT(command);

    drive_init(
        &drv_l,
        left,
        &htim4,
        TIM_CHANNEL_3,
        ENA_DRV_L_GPIO_Port,
        ENA_DRV_L_Pin,
        DIR_DRV_L_GPIO_Port,
        DIR_DRV_L_Pin,
        1,
        6400);

    drive_init(
        &drv_r,
        left,
        &htim3,
        TIM_CHANNEL_2,
        ENA_DRV_R_GPIO_Port,
        ENA_DRV_R_Pin,
        DIR_DRV_R_GPIO_Port,
        DIR_DRV_R_Pin,
        1,
        6400);

    int16_t i = 500;
    uint8_t buffer[64];
    HAL_TIM_Base_Start_IT(&htim2);

    while (true)
    {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        sprintf(buffer, "L %d\t\t\tR%d\n", drv_l.current_speed, drv_r.current_speed);
        uart_send_mes_IT(buffer);
        HAL_Delay(100);
    }
}

void uart_str_RxCPLTCallback()
{

    uint8_t *word_arr[10];
    uint8_t *word;
    uint8_t i = 0;
    volatile int32_t vel = 0;
    struct drive *cur_d = NULL;
    word = strtok(command, " ");
    while (word != NULL)
    {
        word_arr[i] = word;
        i++;
        word = strtok(NULL, " ");
    }
    word_arr[i] = NULL;

    if (strcmp(word_arr[0], "whoareyou") == 0)
    {
        uart_send_mes_IT("drives\n");
    }

    if (strcmp(word_arr[0], "start") == 0 || strcmp(word_arr[0], "START") == 0)
    {

        drive_on(&drv_l);
        drive_on(&drv_r);
    }
    if (strcmp(word_arr[0], "stop") == 0 || strcmp(word_arr[0], "STOP") == 0)
    {

        drive_off(&drv_l);
        drive_off(&drv_r);
    }
    if (strcmp(word_arr[0], "r") == 0 || (strcmp(word_arr[0], "R") == 0))
    {

        vel = atoi(word_arr[1]);
        drive_set_vel(&drv_r, vel);
    }
    if (strcmp(word_arr[0], "l") == 0 || strcmp(word_arr[0], "L") == 0)
    {
        vel = atoi(word_arr[1]);
        drive_set_vel(&drv_l, vel);
    }
    command[0] = '\0';
    uart_ask_str_IT(command);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        drive_increese_speed(&drv_l);
        drive_increese_speed(&drv_r);
    }

    // if (htim->Instance == TIM4)
    //     driveElapsedCallback(&drv_l);
    // if (htim->Instance == TIM3)
    //     driveElapsedCallback(&drv_r);
}

/*
void app()
{
    uint8_t usb_buff[] = "I AM OK!";
    int data = 1;
    while (1)
    {
        CDC_Transmit_FS(usb_buff, sizeof(usb_buff));

        if (USB_Rx_flag == true)
        {
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
            CDC_Transmit_FS(usb_buff, sizeof(usb_buff));
        }
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
    }
}

void USB_APP_Receive(uint8_t *Buf, uint32_t *Len)
{
    USB_Rx_flag = true;
    memset(USB_buff, '\0', USB_buff_len);
    if (*Len < USB_buff_len)
        memcpy(USB_buff, Buf, *Len);
}
*/