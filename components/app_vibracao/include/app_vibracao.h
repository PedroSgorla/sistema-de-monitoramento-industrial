/**
 * @file app_vibracao.h
 * @brief API do módulo de amostragem de vibração usando o sensor MPU6050 com um stream buffer e criação da task de leitura de vibração.
 */
#ifndef APP_VIBRACAO_H
#define APP_VIBRACAO_H

#include "esp_err.h"

/**
 * @brief Inicializa o módulo de amostragem de vibração criando o stream buffer e a task de leitura.
 * @return ESP_OK em caso de sucesso, ou um código de erro direcionado para falha na criação do stream buffer ou da task.
 */
esp_err_t app_vibracao_init(void);

#endif 