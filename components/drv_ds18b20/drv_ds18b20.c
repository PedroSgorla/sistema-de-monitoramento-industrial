/**
 * @file drv_ds18b20.c
 * @brief Implementação do driver do sensor de temperatura DS18B20.
 */
#include "drv_ds18b20.h"
#include "onewire_bus.h"
#include "ds18b20.h"
#include "esp_log.h"
#include "bsp_motor_node.h"

static const char *TAG = "drv_ds18b20";

static onewire_bus_handle_t s_onewire_bus_handle = NULL; /* Handle do barramento OneWire */
static ds18b20_device_handle_t s_ds18b20_handle = NULL; /* Handle do sensor DS18B20 */

esp_err_t drv_ds18b20_init(void)
{
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = BSP_DS18B20_DATA_GPIO,
        .flags = {
            .en_pull_up = true,
        },
    };

    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10, 
    };

    esp_err_t err = onewire_new_bus_rmt(&bus_config, &rmt_config, &s_onewire_bus_handle);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao criar o barramento OneWire: %s", esp_err_to_name(err));
        return err;
    }

    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t next_device;
    esp_err_t search_result;
    bool device_found = false;

    ESP_ERROR_CHECK(onewire_new_device_iter(s_onewire_bus_handle, &iter));

    do{
        search_result = onewire_device_iter_get_next(iter, &next_device);
        if(search_result == ESP_OK) {
            ds18b20_config_t ds_cfg = {0};
            if(ds18b20_new_device_from_enumeration(&next_device, &ds_cfg, &s_ds18b20_handle) == ESP_OK) {
                device_found = true;
                break;
            }
        }
    } while(search_result != ESP_ERR_NOT_FOUND);

    onewire_del_device_iter(iter);

    if(!device_found) {
        ESP_LOGE(TAG, "Nenhum sensor DS18B20 encontrado no barramento OneWire.");
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "Driver do sensor DS18B20 inicializado com sucesso.");
    return ESP_OK;
}

esp_err_t drv_ds18b20_read_temperature_c(float *temperature)
{
    if(temperature == NULL) {
        ESP_LOGE(TAG, "Ponteiro de temperatura não instanciado.");
        return ESP_ERR_INVALID_ARG;
    }

    if(s_ds18b20_handle == NULL) {
        ESP_LOGE(TAG, "Driver do sensor DS18B20 não inicializado.");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ds18b20_trigger_temperature_conversion_for_all(s_onewire_bus_handle);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao iniciar a conversão de temperatura: %s", esp_err_to_name(err));
        return err;
    }

    err = ds18b20_get_temperature(s_ds18b20_handle, temperature);
    if(err == ESP_OK) {
        ESP_LOGI(TAG, "Temperatura lida com sucesso: %.2f°C", *temperature);
    } else {
        ESP_LOGE(TAG, "Falha ao ler a temperatura: %s", esp_err_to_name(err));
    }
    return err;
}