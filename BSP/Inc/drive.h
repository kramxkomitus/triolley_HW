#ifndef INC_DRIVE_H_
#define INC_DRIVE_H_

#include "tim.h"
#include "stdbool.h"

#define PIN_ON RESET
#define PIN_OFF SET

enum drv_location
{
    left,
    right
};

struct drive
{
    bool enable;

    enum drv_location side;

    enum
    {
        forward,
        backward
    } direction;

    uint16_t speed;

    TIM_HandleTypeDef *tim;

    uint32_t tim_chanel;
    uint32_t ena_pin_port;
    uint32_t ena_pin;
    uint32_t dir_pin_port;
    uint32_t dir_pin;
};

void drive_init(
    struct drive *D,
    enum drv_location side,
    TIM_HandleTypeDef *tim,
    uint32_t tim_chanel,
    uint32_t ena_pin_port,
    uint32_t ena_pin,
    uint32_t dir_pin_port,
    uint32_t dir_pin);

void drive_on(struct drive *D);
void drive_off(struct drive *D);


bool drive_set_vel(struct drive *D, int32_t vel);

#endif /* INC_APP_H_ */
