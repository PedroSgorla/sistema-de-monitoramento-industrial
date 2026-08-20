#include "driver/ledc.h"
#include "driver/gpio.h"
#include "bsp_motor_node.h"
#include "app_motor_ctrl.h"

#define MOTOR_LEDC_TIMER     LEDC_TIMER_0
#define MOTOR_LEDC_MODE      LEDC_LOW_SPEED_MODE
#define MOTOR_LEDC_CHANNEL   LEDC_CHANNEL_0
#define MOTOR_LEDC_RES_BITS  LEDC_TIMER_8_BIT
#define MOTOR_LEDC_FREQ_HZ   20000                /*20kHz acima do audível*/

static void ledc_init()
{
    ledc_timer_config_t timer_cfg = {
        .speed_mode = MOTOR_LEDC_MODE,
        .timer_num = MOTOR_LEDC_TIMER,
        .duty_resolution = MOTOR_LEDC_RES_BITS,
        .freq_hz = MOTOR_LEDC_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ledc_timer_config(&timer_cfg);

    ledc_channel_config_t channel_cfg = {
        .speed_mode = MOTOR_LEDC_MODE,
        .channel = MOTOR_LEDC_CHANNEL,
        .timer_sel = MOTOR_LEDC_TIMER,
        .gpio_num = BSP_MOTOR_PWM_GPIO,
        .duty = 0,
        .hpoint = 0, 
    };

    ledc_channel_config(&channel_cfg);
}