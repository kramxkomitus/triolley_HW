#include "drive.h"

void drive_init(
    struct drive *D,
    enum drv_location side,
    TIM_HandleTypeDef *tim,
    uint32_t tim_chanel,
    uint32_t ena_pin_port,
    uint32_t ena_pin,
    uint32_t dir_pin_port,
    uint32_t dir_pin)
{
    D->enable = false;
    D->side = side;
    D->direction = forward;
    D->speed = 0;
    D->tim = tim;
    D->tim_chanel = tim_chanel;
    D->ena_pin_port = ena_pin_port;
    D->ena_pin = ena_pin;
    D->dir_pin_port = dir_pin_port;
    D->dir_pin = dir_pin;
}

void drive_on(struct drive *D)
{
    D->enable = true;
    HAL_GPIO_WritePin(D->ena_pin_port, D->ena_pin, SET);
    // HAL_TIM_PWM_Start(D->tim, D->tim_chanel);
}

void drive_off(struct drive *D)
{
    D->enable = false;
    HAL_GPIO_WritePin(D->ena_pin_port, D->ena_pin, RESET);
    HAL_TIM_PWM_Stop(D->tim, D->tim_chanel);
}

bool drive_set_vel(struct drive *D, int32_t vel)
{
    if (D->enable == false)
    {
        return false;
    }
    else
    {
        // if ((vel < 1 && vel > 400) || (vel > -1 && vel < -400))
        if ((vel < 150) && (vel > -150))
        {
            HAL_TIM_PWM_Stop(D->tim, D->tim_chanel);
            return;
        }
        if (vel < 0)
        {
            HAL_GPIO_WritePin(D->dir_pin_port, D->dir_pin, PIN_OFF);
            D->direction = backward;
            vel = vel * (-1);
        }
        else
        {
            D->direction = forward;
            HAL_GPIO_WritePin(D->dir_pin_port, D->dir_pin, PIN_ON);
        }

        D->speed = 48000000 / (((vel) * 6400) / 60);
        __HAL_TIM_SetAutoreload(D->tim, D->speed - 1);
        __HAL_TIM_SET_COMPARE(D->tim, D->tim_chanel, D->speed >> 1); // /2
        HAL_TIM_PWM_Start(D->tim, D->tim_chanel);

        return true;
    }
}