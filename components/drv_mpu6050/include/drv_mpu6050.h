/**
 * @file drv_mpu6050.h
 * @brief API do sensor MPU6050 (acelerômetro e giroscópio) via I2C.
 */
#ifndef DRV_MPU6050_H
#define DRV_MPU6050_H

#include <stdint.h>
#include "esp_err.h"

/**
 * @brief Inicializa o sensor MPU6050.
 * @return ESP_OK se a inicialização for bem-sucedida, caso contrário um código de erro.
 */
esp_err_t drv_mpu6050_init(void);

/**
 * @brief Lê o valor bruto do eixo Z do acelerômetro.
 * @param[out] out_raw Ponteiro para escrever o valor lido.
 * @return ESP_OK se a leitura for bem-sucedida, caso contrário um código de erro.
 */
esp_err_t drv_mpu6050_read_accel_z_raw(int16_t *out_raw);

#endif