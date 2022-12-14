#include "drive.h"


void drive_init(uint32_t ena_pin_port, uint32_t ena_pin);
//struct drive *D, enum drv_location side, TIM_HandleTypeDef *tim, uint32_t tim_chanel, 
{
    D->enable = false;
    D->side = side;
    D->direction = forward;
    D->speed = 0;
    D->tim = tim;
    D->tim_chanel = tim_chanel;
    D->ena_pin_port = ena_pin_port;
    D->ena_pin = ena_pin;
}

void drive_on(struct drive *D)
{
    D->enable = true;

    HAL_GPIO_WritePin(D->ena_pin_port, D->ena_pin, SET);
    
}