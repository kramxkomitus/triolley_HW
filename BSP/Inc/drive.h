#ifndef INC_DRIVE_H_
#define INC_DRIVE_H_

#include "tim.h"
#include "stdbool.h"

#define PIN_ON RESET
#define PIN_OFF SET

/*обновление контролирующего таймера 0,01 сек = 10 мсек*/
#define AXEL_QANT_TIME 10   //мсек

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

    int32_t target_speed;       //RPM
    int32_t current_speed;       //RPM NU
    int32_t target_step_time;
    int32_t current_step_time;
    uint32_t axeleration_step;
    uint32_t axeleration;

    TIM_HandleTypeDef *tim;

    uint32_t tim_chanel;
    uint32_t ena_pin_port;
    uint32_t ena_pin;
    uint32_t dir_pin_port;
    uint32_t dir_pin;

    uint32_t PPR;
};

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
    uint32_t PPR);

void drive_on(struct drive *D);
void drive_off(struct drive *D);
void driveElapsedCallback(struct drive *D);
bool drive_set_axel(struct drive *D, uint32_t axeleration);
bool drive_set_vel(struct drive *D, int32_t vel);
void drive_increese_speed(struct drive *D);


#endif /* INC_APP_H_ */
