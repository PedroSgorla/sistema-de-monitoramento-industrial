/**
 * @file drv_mpu6050.c
 * @brief Driver para o sensor MPU6050 (acelerômetro e giroscópio) via I2C.
 */
#include "drv_mpu6050.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "drv_mpu6050";

#define MPU6050_I2C_ADDR 0x68 /* endereço I2C do MPU6050 */
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_ACCEL_ZOUT_H 0x47
#define MPU6050_I2C_TIMEOUT_MS 100
#define MPU6050_I2C_FREQ_HZ 400000 /* frequencia do sinal SCL */

static i2c_master_bus_handle_t s_bus_handle = NULL; /* manipulador do barramento I2C */
static i2c_master_dev_handle_t s_device_handle = NULL; /* manipulador do dispositivo I2C */

esp_err_t drv_mpu6050_init(void) 
{
    esp_err_t err;

    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = BSP_I2C_SDA_GPIO,
        .scl_io_num = BSP_I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,   /* filtro de ruido por hardware */
        .flags.enable_internal_pullup = true, /* habilita pull-ups internos */
    };

    err = i2c_new_master_bus(&bus_config, &s_bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao criar o barramento I2C: %s", esp_err_to_name(err));
        return err;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MPU6050_I2C_ADDR,
        .scl_speed_hz = MPU6050_I2C_FREQ_HZ, 
    };

    err = i2c_master_bus_add_device(s_bus_handle, &dev_cfg, &s_device_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao adicionar o dispositivo MPU6050 ao I2C: %s", esp_err_to_name(err));
        return err;
    }

    uint8_t wake_cmd[2] = {MPU6050_REG_PWR_MGMT_1, 0x00}; /* comando para acordar o MPU6050 */
    err = i2c_master_transmit(s_device_handle, wake_cmd, sizeof(wake_cmd), MPU6050_I2C_TIMEOUT_MS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao acordar ao MPU6050: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "MPU6050 inicializado com sucesso");
    return ESP_OK;
}

esp_err_t drv_mpu6050_read_accel_z_raw(int16_t *out_raw) 
{
    if (out_raw == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reg = MPU6050_REG_ACCEL_ZOUT_H;
    uint8_t data[2] = {0};/* Buffer para armazenar os dados lidos */

    esp_err_t err = i2c_master_transmit_receive(s_device_handle, &reg, 1, data, sizeof(data), MPU6050_I2C_TIMEOUT_MS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao ler o registrador ACCEL_ZOUT_H: %s", esp_err_to_name(err));
        return err;
    }

    *out_raw = (int16_t)((data[0] << 8) | data[1]);
    return ESP_OK;
}