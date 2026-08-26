/**
 * @file drv_adc_current.h
 * @brief API do driver do sensor de corrente baseado em ADC.
 */
#ifndef DRV_ADC_CURRENT_H
#define DRV_ADC_CURRENT_H

#include "esp_err.h"

/**
 * @brief Inicializa o driver do sensor de corrente ACS712.
 * 
 * @return ESP_OK em caso de sucesso ou um código de erro direcionado para falha na inicialização do driver.
 * @return Código de esp_err_t em caso de falha ao configurar o ADC ou a calibração.
 * 
 * @note Esta função deve ser chamada antes de qualquer outra função do driver.
 */
esp_err_t drv_adc_current_init(void);

/**
 * @brief Lê a corrente medida pelo sensor ACS712 em amperes.
 * 
 * @param[out] out_current Ponteiro para escrever o valor da corrente medida em amperes.
 * 
 * @return ESP_OK em caso de sucesso.
 * @return ESP_ERR_INVALID_ARG se o ponteiro out_current for NULL.
 * @return ESP_ERR_INVALID_STATE se o driver não estiver inicializado.
 * 
 * @note Esta função realiza a leitura do ADC e aplica a calibração para retornar o valor da corrente em amperes
 *       O valor devolvido contepla a correção do divisor de tensão(12k/20k) para o ACS712 A20.
 */
esp_err_t drv_adc_current_read_current_A(float *out_current);

#endif