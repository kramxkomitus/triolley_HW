#include "drive.h"

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
    D->current_dir = forward;
    D->target_speed = 0;  // RPM
    D->current_speed = 0; // RPM NU
    D->tim = tim;
    D->tim_chanel = tim_chanel;
    D->ena_pin_port = ena_pin_port;
    D->ena_pin = ena_pin;
    D->dir_pin_port = dir_pin_port;
    D->dir_pin = dir_pin;
    D->PPR = PPR;
    D->target_step = ((TIM_CLOCK / MIN_SPEED) * 60 / D->PPR);
    D->current_step = ((TIM_CLOCK / MIN_SPEED) * 60 / D->PPR);
    D->axeleration_koef = AXEL_RES + axeleration;
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
    // D->axeleration_koef = 100 + axeleration;
    // D->axeleration = axeleration;
    // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

bool drive_set_vel(struct drive *D, int32_t vel)
{
    if (D->enable == false)
    {
        return false;
    }
    else
    {
        if ((vel < MIN_SPEED) && (vel > -MIN_SPEED))
        {
            HAL_TIM_PWM_Stop(D->tim, D->tim_chanel);
            HAL_TIM_Base_Stop_IT(D->tim);
            return;
        }
        if (vel > MAX_SPEED)
        {
            vel = MAX_SPEED;
        }
        if (vel < -MAX_SPEED)
        {
            vel = - MAX_SPEED;
        }
        if (vel < 0)
        {
            HAL_GPIO_WritePin(D->dir_pin_port, D->dir_pin, PIN_OFF);
            D->target_dir = backward;
            vel = vel * (-1);
        }
        else
        {
            D->target_dir = forward;
            HAL_GPIO_WritePin(D->dir_pin_port, D->dir_pin, PIN_ON);
        }

        D->target_speed = vel;
        D->target_step = TIM_CLOCK / (vel) / D->PPR * 60 /*RPM*/;
        __HAL_TIM_SetAutoreload(D->tim, D->current_step - 1);
        __HAL_TIM_SET_COMPARE(D->tim, D->tim_chanel, (D->current_step >> 1) - 1);
        HAL_TIM_PWM_Start(D->tim, D->tim_chanel);
        HAL_TIM_Base_Start_IT(D->tim);

        return true;
    }
}

void driveElapsedCallback(struct drive *D)
{
    int32_t delta;
    uint32_t new_step = ((TIM_CLOCK / MIN_SPEED) * 60 / D->PPR);

    if (D->target_dir != D->current_dir) // скорости разных знаков, необходимо произвести торможение
    {
        // если движок стоял, то сразу переходим к разгону приравняв направления
        if (D->current_step >= ((TIM_CLOCK / MIN_SPEED) * 60 / D->PPR))
        {
            D->current_dir = D->target_dir;
            return;
        }
        new_step = D->current_step * D->axeleration_koef / AXEL_RES /*%*/;
        // определяем шаг после уменьшения скорости, если скорость меньше минимальной
        // не забываем что скорость обратнопропорционально шагам
        if (new_step > ((TIM_CLOCK / MIN_SPEED) * 60 / D->PPR))
        {
            // принимаем ее минимальной
            new_step = ((TIM_CLOCK / MIN_SPEED) * 60 / D->PPR);
            // можно изменить направление движения и перейти к следующей итеррации
            D->current_dir = D->target_dir;
            return;
        }
    }
    else
    {
        delta = D->current_step - D->target_step;
        // delta определяет разгон (>0) или торможение (<0) или остановку (=0)

        if (delta == 0)
        {
            HAL_TIM_Base_Stop_IT(D->tim); // останавливаем прерывания т.к. достигли нужной скорости
            if (D->current_step == ((TIM_CLOCK / MIN_SPEED) * 60 / D->PPR))
            {
                HAL_TIM_PWM_Stop(D->tim, D->tim_chanel);
                D->current_speed = 0;
            }
            return;
        }
        if (delta > 0) // разгон
        {
            new_step = (D->current_step * AXEL_RES) / D->axeleration_koef /*%*/;
            // определяем шаг после увеличения скорости, если скорость будет больше целевой
            if (new_step < D->target_step)
            {
                // то принимаем новую скорость = целевой
                new_step = D->target_step;
            }
        }

        if (delta < 0) // торможение
        {
            new_step = D->current_step * D->axeleration_koef / AXEL_RES /*%*/;
            // определяем шаг после уменьшения скорости, если скорость будет меньше целевой
            if (new_step > D->current_step)
            {
                // то принимаем новую скорость = целевой
                new_step = D->target_step;
            }
        }
    }
    // записываем текущий шаг в двигатель
    D->current_step = new_step;

    D->current_speed = (TIM_CLOCK / D->current_step) * 60 / D->PPR;
    if (D->current_dir == backward)
        D->current_speed = -D->current_speed;
    __HAL_TIM_SetAutoreload(D->tim, D->current_step - 1);
    __HAL_TIM_SET_COMPARE(D->tim, D->tim_chanel, (D->current_step >> 1) - 1);
}