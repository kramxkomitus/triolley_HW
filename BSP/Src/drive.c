#include "drive.h"

#define MAX_TIME_STEP 3000

void drive_init(
    struct drive *D,
    enum drv_location side,
    TIM_HandleTypeDef *tim,
    uint32_t tim_chanel,
    uint32_t ena_pin_port,
    uint32_t ena_pin,
    uint32_t dir_pin_port,
    uint32_t dir_pin,
    uint32_t axeleration,
    uint32_t PPR)
{
    D->enable = false;
    D->side = side;
    D->direction = forward;
    D->target_speed = 0;  // RPM
    D->current_speed = 0; // RPM NU
    D->target_step_time = MAX_TIME_STEP;
    D->current_step_time = MAX_TIME_STEP;
    D->tim = tim;
    D->tim_chanel = tim_chanel;
    D->ena_pin_port = ena_pin_port;
    D->ena_pin = ena_pin;
    D->dir_pin_port = dir_pin_port;
    D->dir_pin = dir_pin;
    D->PPR = PPR;
    D->axeleration_step = axeleration;
    // drive_set_axel(&D, axeleration);
}

void drive_on(struct drive *D)
{
    D->enable = true;
    HAL_GPIO_WritePin(D->ena_pin_port, D->ena_pin, SET);
}

void drive_off(struct drive *D)
{
    D->enable = false;
    HAL_GPIO_WritePin(D->ena_pin_port, D->ena_pin, RESET);
    HAL_TIM_PWM_Stop(D->tim, D->tim_chanel);
    HAL_TIM_Base_Stop_IT(D->tim);
}

bool drive_set_axel(struct drive *D, uint32_t axeleration)
{
    // нужно написать нормальный пересчет единиц
    // не работает
    D->axeleration_step = axeleration;
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

bool drive_set_vel(struct drive *D, int32_t vel)
{
    if (D->enable == false)
    {
        return false;
    }
    else
    {
        if ((vel < 150) && (vel > -150))
        {
            HAL_TIM_PWM_Stop(D->tim, D->tim_chanel);
            HAL_TIM_Base_Stop_IT(D->tim);
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

        D->target_speed = vel;
        D->target_step_time = 48000000 /*MHz*/ / (((vel)*D->PPR) / 60 /*RPM*/);
        __HAL_TIM_SetAutoreload(D->tim, D->current_step_time - 1);
        __HAL_TIM_SET_COMPARE(D->tim, D->tim_chanel, (D->current_step_time >> 1) - 1);
        HAL_TIM_PWM_Start(D->tim, D->tim_chanel);
        HAL_TIM_Base_Start_IT(D->tim);

        return true;
    }
}

void drive_increese_speed(struct drive *D)
{
    if (D->target_step_time > D->current_step_time)
    {
        if (D->target_step_time - D->current_step_time > D->axeleration_step)
        {
            D->current_step_time = D->current_step_time + D->axeleration_step;
            __HAL_TIM_SetAutoreload(D->tim, D->current_step_time - 1);
            __HAL_TIM_SET_COMPARE(D->tim, D->tim_chanel, (D->current_step_time >> 1) - 1);
        }
        D->current_speed = (48000000 * 60) / (D->current_step_time * D->PPR);
    }
    else if (D->target_step_time < D->current_step_time)
    {
        if (D->current_step_time - D->target_step_time > D->axeleration_step)
        {
            D->current_step_time = D->current_step_time - D->axeleration_step;
            __HAL_TIM_SetAutoreload(D->tim, D->current_step_time - 1);
            __HAL_TIM_SET_COMPARE(D->tim, D->tim_chanel, (D->current_step_time >> 1) - 1);
        }
        D->current_speed = (48000000 * 60) / (D->current_step_time * D->PPR);
    }
}

void driveElapsedCallback(struct drive *D)
{

    //
    // int32_t delta = D->current_step_time - D->target_step_time;
    // if (delta == 0)
    //    HAL_TIM_Base_Stop_IT(D->tim);
    // if (delta > 0)
    //    D->current_step_time = D->current_step_time - D->axeleration_step;
    //    // D->current_step_time = (D->current_step_time * 90) / 100;
    // if (delta < 0)
    //    D->current_step_time = D->current_step_time + D->axeleration_step;
    //    // D->current_step_time = (D->current_step_time * 110) / 100;
    //
    //__HAL_TIM_SetAutoreload(D->tim, D->current_step_time - 1);
    //__HAL_TIM_SET_COMPARE(D->tim, D->tim_chanel, (D->current_step_time >> 1) - 1);
}