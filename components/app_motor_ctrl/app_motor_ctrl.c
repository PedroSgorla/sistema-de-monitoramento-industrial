/**
 * @file app_motor_ctrl.c
 * @brief Driver para o controle do motor DC via PWM.
 */
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "bsp_motor_node.h"
#include "app_motor_ctrl.h"
#include "esp_log.h"

#define MOTOR_LEDC_TIMER     LEDC_TIMER_0
#define MOTOR_LEDC_MODE      LEDC_LOW_SPEED_MODE
#define MOTOR_LEDC_CHANNEL   LEDC_CHANNEL_0
#define MOTOR_LEDC_RES_BITS  LEDC_TIMER_8_BIT
#define MOTOR_LEDC_FREQ_HZ   20000                /*20kHz acima do audível*/
#define MOTOR_LEDC_DUTY_MAX  ((1 << MOTOR_LEDC_RES_BITS) - 1)

static const char *TAG = "app_motor_ctrl";

/**
 * @brief Enumeração para representar o sentido de rotação do motor.
 */
typedef enum {
    CLOCKWISE = 0,
    COUNTERCLOCKWISE = 1,
} direction_t;


static direction_t current_direction = CLOCKWISE; /* Sentido de rotação atual do motor */

/**
 * @brief Inicializa o periferico LEDC para controle de PWM do motor.
 */
static void ledc_init(void)
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

/**
 * @brief Inicializa os pinos GPIO para controle do motor.
 */
static void motor_gpio_init(void)
{
    gpio_config_t io_cfg = {
        .pin_bit_mask = (1ULL << BSP_MOTOR_IN1_GPIO) |
                        (1ULL << BSP_MOTOR_IN2_GPIO)  |
                        (1ULL << BSP_MOTOR_STBY_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&io_cfg);

    gpio_set_level(BSP_MOTOR_IN1_GPIO, 0);
    gpio_set_level(BSP_MOTOR_IN2_GPIO, 0);
    gpio_set_level(BSP_MOTOR_STBY_GPIO, 1);
}

void app_motor_ctrl_init(void)
{
    ledc_init();
    motor_gpio_init();
}

void app_motor_ctrl_set_command(int8_t duty_pct)
{
    if(duty_pct > 100) {
        ESP_LOGW(TAG, "Duty cycle acima do limite, ajustando para 100%%");
        duty_pct = 100;
    } else if(duty_pct < -100) {
        ESP_LOGW(TAG, "Duty cycle abaixo do limite, ajustando para -100%%");
        duty_pct = -100;
    }

    direction_t new_direction = (duty_pct < 0) ? COUNTERCLOCKWISE : CLOCKWISE;

    int8_t magnitude = (duty_pct < 0) ? (int8_t)(-duty_pct) : duty_pct;

    if(new_direction != current_direction) {
        ledc_set_duty(MOTOR_LEDC_MODE, MOTOR_LEDC_CHANNEL, 0);
        ledc_update_duty(MOTOR_LEDC_MODE, MOTOR_LEDC_CHANNEL);

        if(new_direction == CLOCKWISE) {
            gpio_set_level(BSP_MOTOR_IN1_GPIO, 1);
            gpio_set_level(BSP_MOTOR_IN2_GPIO, 0);
        } else {
            gpio_set_level(BSP_MOTOR_IN1_GPIO, 0);
            gpio_set_level(BSP_MOTOR_IN2_GPIO, 1);
        }

        current_direction = new_direction;
    }

    uint32_t duty_ledc = ((uint32_t)magnitude * MOTOR_LEDC_DUTY_MAX) /100;

    ledc_set_duty(MOTOR_LEDC_MODE, MOTOR_LEDC_CHANNEL, duty_ledc);
    ledc_update_duty(MOTOR_LEDC_MODE, MOTOR_LEDC_CHANNEL);
}