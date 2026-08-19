
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


static const char *TAG = "motor_node";

void app_main(void)
{
    ESP_LOGI(TAG, "motor_node iniciado");
    
    while(1)
    {
        ESP_LOGI(TAG, "heartbeat de teste");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
