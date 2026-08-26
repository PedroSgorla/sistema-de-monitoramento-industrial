/**
 * @file drv_ds18b20.h
 * @brief API do driver do sensor de temperatura DS18B20.
 */
#ifndef DRV_DS18B20_H
#define DRV_DS18B20_H

#include "esp_err.h"

/**
 * @brief Inicializa o driver do sensor DS18B20.
 * 
 * @return ESP_OK em caso de sucesso ou um código de erro direcionado para falha na inicialização do driver.
 * @return ESP_ERR_NOT_FOUND se o sensor não for encontrado.
 * 
 * @note Esta função deve ser chamada antes de qualquer outra função do driver.
 */
esp_err_t drv_ds18b20_init(void);

/**
 * @brief Lê a temperatura do sensor DS18B20.
 * 
 * @param[out] temperature Ponteiro para armazenar a temperatura lida.
 * 
 * @return ESP_OK em caso de sucesso ou um código de erro direcionado para falha na leitura da temperatura.
 * 
 * @note Função bloqueante que aguarda a conversão de temperatura do sensor antes de retornar o valor.
 */
esp_err_t drv_ds18b20_read_temperature_c(float *temperature);

#endif