/**
 * @file drv_adc_current.c
 * @brief Implementação do driver do sensor de corrente baseado em ADC (ACS712).
 */
#include "drv_adc_current.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "bsp_motor_node.h"

static const char *TAG = "drv_adc_current";

#define ACS712_SENSITIVITY_MV_A 100.0f
#define ACS712_OFFSET_ZERO_MV 2500.0f /*Vcc/2 norminal*/

/*Divisor resistivo entre a saída do sensor e o ADC*/
#define DIVIDER_R1 12000.0f
#define DIVIDER_R2 20000.0f
#define DIVIDER_RATIO (DIVIDER_R2 / (DIVIDER_R1 + DIVIDER_R2))

static adc_oneshot_unit_handle_t s_adc_handle = NULL;
static adc_cali_handle_t s_adc_cali_handle = NULL;
static adc_unit_t s_adc_unit;
static adc_channel_t s_adc_channel;

esp_err_t drv_adc_current_init(void)
{
    esp_err_t err = adc_oneshot_io_to_channel(BSP_ADC_CURRENT_GPIO, &s_adc_unit, &s_adc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO %d não mapeado para canal ADC: %s", BSP_ADC_CURRENT_GPIO, esp_err_to_name(err));
        return err;
    }

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = s_adc_unit,
    };
    err = adc_oneshot_new_unit(&init_config, &s_adc_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao criar unidade ADC: %s", esp_err_to_name(err));
        return err;
    }

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    err = adc_oneshot_config_channel(s_adc_handle, s_adc_channel, &channel_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao configurar canal ADC: %s", esp_err_to_name(err));
        return err;
    }

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = s_adc_unit,
        .chan = s_adc_channel,
        .atten = channel_config.atten,
        .bitwidth = channel_config.bitwidth,
    };
    err = adc_cali_create_scheme_curve_fitting(&cali_config, &s_adc_cali_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao criar o esquema de calibração do ADC: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Driver do sensor de corrente ACS712 inicializado com sucesso.");
    return ESP_OK;
}

esp_err_t drv_adc_current_read_current_A(float *out_current)
{
    if(out_current == NULL) {
        ESP_LOGE(TAG, "Parametro passado não está instanciado.");
        return ESP_ERR_INVALID_ARG;
    }
    
    if(s_adc_handle == NULL || s_adc_cali_handle == NULL) {
        ESP_LOGE(TAG, "handle do sensor de corrente não inicializado.");
        return ESP_ERR_INVALID_STATE;
    }

    int raw_adc = 0;
    esp_err_t err = adc_oneshot_read(s_adc_handle, s_adc_channel, &raw_adc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao ler valor bruto do ADC: %s", esp_err_to_name(err));
        return err;
    }

    int voltage_adc_mv = 0;
    err = adc_cali_raw_to_voltage(s_adc_cali_handle, raw_adc, &voltage_adc_mv);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao converter valor bruto do ADC para tensão em mV: %s", esp_err_to_name(err));
        return err;
    }

    float voltage_sensor_mv = (float)voltage_adc_mv / DIVIDER_RATIO; /*Compensa do dividor de tensão */
    *out_current = (voltage_sensor_mv - ACS712_OFFSET_ZERO_MV) / ACS712_SENSITIVITY_MV_A;

    return ESP_OK;
}
