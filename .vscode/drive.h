#ifndef INC_DRIVE_H_
#define INC_DRIVE_H_

#include "tim.h"
#include "stdbool.h"


enum drv_lacation {
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
};

void drive_init();
// void drive_init(struct drive *D, enum drv_location side, TIM_HandleTypeDef *tim, uint32_t tim_chanel, uint32_t ena_pin_port, uint32_t ena_pin);


#endif /* INC_APP_H_ */
