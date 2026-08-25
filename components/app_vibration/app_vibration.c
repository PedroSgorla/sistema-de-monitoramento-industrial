/**
 * @file app_vibration.c
 * @brief Implementação do módulo de amostragem de vibração usando o sensor MPU6050 com um stream buffer e criaçaõ das task leitura de vibração.
 */
#include <math.h>
#include "app_vibration.h"
#include "drv_mpu6050.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/stream_buffer.h"


static const char *TAG = "app_vibration";

#define VIBRATION_SAMPLE_RATE_HZ 500
#define VIBRATION_SAMPLE_PERIOD_TICKS pdMS_TO_TICKS(1000 / VIBRATION_SAMPLE_RATE_HZ)
#define VIBRATION_ACCEL_SCALE_G 16384.0f /*sensibilidade padrao do MPU6050*/

#define VIBRATION_STREAM_BUFFER_SIZE (1500 * sizeof(float))
#define VIBRATION_STREAM_BUFFER_TRIGGER_LEVEL (500 * sizeof(float))/* disparo da leitura do buffer pela task consumidora */

#define VIBRATION_RMS_WINDOW_SIZE 500 /* tamanho da janela para cálculo do RMS */

static StreamBufferHandle_t s_stream_buffer = NULL;

static uint32_t s_samples_lost = 0;

/**
 * @brief Task de amostragem de vibração que lê o valor bruto do acelerômetro Z do MPU6050, converte para g e envia para o stream buffer.
 * @param[in] arg Argumento da task (não utilizado).
 */
static void vibration_sample_task(void *arg)
{
    (void)arg;
    TickType_t last_wake = xTaskGetTickCount();

    for(;;){
        int16_t raw;
        if(drv_mpu6050_read_accel_z_raw(&raw) == ESP_OK){
            float sample_g = raw / VIBRATION_ACCEL_SCALE_G;
            if(xStreamBufferSend(s_stream_buffer, &sample_g, sizeof(sample_g), 0) != sizeof(sample_g)){
                s_samples_lost++; /* monitoramento de amostras perdidas */
            }
        }
        vTaskDelayUntil(&last_wake, VIBRATION_SAMPLE_PERIOD_TICKS);
    }
}

/**
 * @brief Task que lê amostras do stream buffer e calcula o RMS e registra o valor.
 * @param[in] arg Argumento da task (não utilizado).
 */
static void vibration_rms_task(void *arg)
{
    (void)arg;
    float samples[VIBRATION_RMS_WINDOW_SIZE];

    for(;;){
        size_t bytes_received = xStreamBufferReceive(s_stream_buffer, samples, sizeof(samples), portMAX_DELAY);
        if(bytes_received > 0){
            size_t num_samples = bytes_received / sizeof(float);
            float sum_squares = 0.0f;
            for(size_t i = 0; i < num_samples; i++){
                sum_squares += samples[i] * samples[i];
            }
            float rms = sqrtf(sum_squares / num_samples);
            ESP_LOGI(TAG, "RMS da vibração: %.4f g", rms);
        }
    }
}

esp_err_t app_vibration_init(void)
{
    s_stream_buffer = xStreamBufferCreate(VIBRATION_STREAM_BUFFER_SIZE, VIBRATION_STREAM_BUFFER_TRIGGER_LEVEL);

    if(s_stream_buffer == NULL){
        ESP_LOGE(TAG, "Falha ao criar o stream buffer para amostras de vibração");
        return ESP_ERR_NO_MEM;
    }

    BaseType_t vibration_sample_task_created = xTaskCreate(vibration_sample_task, "vibration_sample_task", 4096, NULL, 10, NULL);
    if(vibration_sample_task_created != pdPASS){
        ESP_LOGE(TAG, "Falha ao criar a task de amostragem de vibração");
        return ESP_FAIL;
    }
    
    BaseType_t vibration_rms_task_created = xTaskCreate(vibration_rms_task, "vibration_rms_task", 4096, NULL, 5, NULL);
    if(vibration_rms_task_created != pdPASS){
        ESP_LOGE(TAG, "Falha ao criar a task de cálculo do RMS da vibração");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Stream buffer e task para amostras de vibração criado com sucesso (%d Hz)" ,VIBRATION_SAMPLE_RATE_HZ);

    return ESP_OK;
}

