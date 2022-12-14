#include "rgb_led.h"
#include "tim.h"

// WS2812B
//  ___________
// _| 0.4 mks |_______0.85mks_ --> 0 code
//  __________________
// _|    0.85 mks    |_0.4mks_ --> 1 code
// RESET - >50mks.
// Counter period = 3, 1 tic = 400ns --> 2.5 Mhz
// my timer - 72Mhz, Prescaler = 29 - 1.
// RESET = 125 tic

#define PREAMBLESIZE 125
#define LEDS 22
#define PACKEGE_LEN PREAMBLESIZE + LEDS * 24
#define BitIsSet(reg, bit) ((reg & (1 << bit)) != 0)

static uint16_t led_buf[PACKEGE_LEN] = {};

void ws2812_pixel_set(uint8_t red, uint8_t green, uint8_t blue, uint16_t numder_led)
{
    for (uint16_t i = 0; i < 8; i++)
    {
        if (BitIsSet(red, (7 - i)) == 1)
        {
            led_buf[PREAMBLESIZE + numder_led * 24 + i + 8] = 2;
        }
        else
        {
            led_buf[PREAMBLESIZE + numder_led * 24 + i + 8] = 1;
        }
        if (BitIsSet(green, (7 - i)) == 1)
        {
            led_buf[PREAMBLESIZE + numder_led * 24 + i + 0] = 2;
        }
        else
        {
            led_buf[PREAMBLESIZE + numder_led * 24 + i + 0] = 1;
        }
        if (BitIsSet(blue, (7 - i)) == 1)
        {
            led_buf[PREAMBLESIZE + numder_led * 24 + i + 16] = 2;
        }
        else
        {
            led_buf[PREAMBLESIZE + numder_led * 24 + i + 16] = 1;
        }
    }
}

void ws2812_pixel_show()
{
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)led_buf, PACKEGE_LEN);
}

void ws2812_clean()
{
    for (int i = 0; i < LEDS; i++)
    {
        ws2812_pixel_set(0, 0, 0, i);
    }
    ws2812_pixel_show();
}