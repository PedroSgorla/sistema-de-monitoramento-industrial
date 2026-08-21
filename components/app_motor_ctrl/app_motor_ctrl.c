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

typedef enum {
    SENTIDO_HORARIO = 0,
    SENTIDO_ANTIHORARIO = 1,
} sentido_t;

static sentido_t sentido_atual = SENTIDO_HORARIO;

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

    sentido_t novo_sentido = (duty_pct < 0) ? SENTIDO_ANTIHORARIO : SENTIDO_HORARIO;

    int8_t magnitude = (duty_pct < 0) ? (int8_t)(-duty_pct) : duty_pct;

    if(novo_sentido != sentido_atual) {
        ledc_set_duty(MOTOR_LEDC_MODE, MOTOR_LEDC_CHANNEL, 0);
        ledc_update_duty(MOTOR_LEDC_MODE, MOTOR_LEDC_CHANNEL);

        if(novo_sentido == SENTIDO_HORARIO) {
            gpio_set_level(BSP_MOTOR_IN1_GPIO, 1);
            gpio_set_level(BSP_MOTOR_IN2_GPIO, 0);
        } else {
            gpio_set_level(BSP_MOTOR_IN1_GPIO, 0);
            gpio_set_level(BSP_MOTOR_IN2_GPIO, 1);
        }

        sentido_atual = novo_sentido;
    }

    uint32_t duty_ledc = ((uint32_t)magnitude * MOTOR_LEDC_DUTY_MAX) /100;

    ledc_set_duty(MOTOR_LEDC_MODE, MOTOR_LEDC_CHANNEL, duty_ledc);
    ledc_update_duty(MOTOR_LEDC_MODE, MOTOR_LEDC_CHANNEL);
}