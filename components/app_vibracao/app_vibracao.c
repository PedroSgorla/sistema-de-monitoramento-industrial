/**
 * @file app_vibracao.c
 * @brief Implementação do módulo de amostragem de vibração usando o sensor MPU6050 com um stream buffer e criaçaõ das task leitura de vibração.
 */
#include "app_vibracao.h"
#include "drv_mpu6050.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/stream_buffer.h"

static const char *TAG = "app_vibracao";

#define VIBRACAO_SAMPLE_RATE_HZ 500
#define VIBRACAO_SAMPLE_PERIOD_TICKS pdMS_TO_TICKS(1000 / VIBRACAO_SAMPLE_RATE_HZ)
#define VIBRACAO_ACCEL_SCALE_G 16384.0f /*sensibilidade padrao do MPU6050*/

#define VIBRACAO_STREAM_BUFFER_SIZE (1500 * sizeof(float))
#define VIBRACAO_STREAM_BUFFER_TRIGGER_LEVEL (500 * sizeof(float))/* disparo da leitura do buffer pela task consumidora */

static StreamBufferHandle_t s_stream_buffer = NULL;

static uint32_t s_amostras_perdidas = 0;

/**
 * @brief Task de amostragem de vibração que lê o valor bruto do acelerômetro Z do MPU6050, converte para g e envia para o stream buffer.
 * @param[in] arg Argumento da task (não utilizado).
 */
static void vibracao_sample_task(void *arg)
{
    (void)arg;
    TickType_t last_wake = xTaskGetTickCount();

    for(;;){
        int16_t raw;
        if(drv_mpu6050_read_accel_z_raw(&raw) == ESP_OK){
            float amostragem_g = raw / VIBRACAO_ACCEL_SCALE_G;
            if(xStreamBufferSend(s_stream_buffer, &amostragem_g, sizeof(amostragem_g), 0) != sizeof(amostragem_g)){
                s_amostras_perdidas++; /* monitoramento de amostras perdidas */
            }
        }
        vTaskDelayUntil(&last_wake, VIBRACAO_SAMPLE_PERIOD_TICKS);
    }
}

esp_err_t app_vibracao_init(void)
{
    s_stream_buffer = xStreamBufferCreate(VIBRACAO_STREAM_BUFFER_SIZE, VIBRACAO_STREAM_BUFFER_TRIGGER_LEVEL);

    if(s_stream_buffer == NULL){
        ESP_LOGE(TAG, "Falha ao criar o stream buffer para amostras de vibração");
        return ESP_ERR_NO_MEM;
    }

    BaseType_t task_created = xTaskCreate(vibracao_sample_task, "vibracao_sample_task", 4096, NULL, 10, NULL);
    if(task_created != pdPASS){
        ESP_LOGE(TAG, "Falha ao criar a task de amostragem de vibração");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Stream buffer e task para amostras de vibração criado com sucesso (%d Hz)" ,VIBRACAO_SAMPLE_RATE_HZ);

    return ESP_OK;
}