#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_adc_cal.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_log.h"

#define INPUT_PIN_1 34
#define INPUT_PIN_2 35
#define INPUT_PIN_3 32
#define INPUT_PIN_4 33
#define INPUT_PIN_5 25
#define INPUT_PIN_6 26
#define INPUT_PIN_7 27
#define INPUT_PIN_8 14

static const char *TAG = "ADC_INPUT";

// Cấu hình kênh ADC cho các chân
#define ADC_WIDTH ADC_WIDTH_BIT_12    // Độ phân giải 12 bit (dải từ 0-4095)
#define ADC_ATTEN ADC_ATTEN_DB_0     // Attenuation, mức độ giảm tín hiệu vào

void app_main() {
    // Cấu hình các chân GPIO làm đầu vào ADC
    adc1_config_width(ADC_WIDTH); // Cấu hình độ phân giải
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN); // Cấu hình kênh cho chân GPIO 34
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN); // Chân GPIO 35
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN); // Chân GPIO 32
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN); // Chân GPIO 33
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN); // Chân GPIO 25
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN); // Chân GPIO 26
    adc1_config_channel_atten(ADC2_CHANNEL_8, ADC_ATTEN); // Chân GPIO 27
    adc1_config_channel_atten(ADC2_CHANNEL_9, ADC_ATTEN); // Chân GPIO 14

    while (1) {
        // Đọc giá trị từ các chân ADC (dải từ 0 đến 4095)
        int value_1 = adc1_get_raw(ADC1_CHANNEL_0); // GPIO 34
        int value_2 = adc1_get_raw(ADC1_CHANNEL_3); // GPIO 35
        int value_3 = adc1_get_raw(ADC1_CHANNEL_4); // GPIO 32
        int value_4 = adc1_get_raw(ADC1_CHANNEL_5); // GPIO 33
        int value_5 = adc1_get_raw(ADC1_CHANNEL_6); // GPIO 25
        int value_6 = adc1_get_raw(ADC1_CHANNEL_7); // GPIO 26
        int value_7 = adc1_get_raw(ADC2_CHANNEL_8); // GPIO 27
        int value_8 = adc1_get_raw(ADC2_CHANNEL_9); // GPIO 14

        // In ra giá trị của từng chân ADC
        ESP_LOGI(TAG, "Pin 1: %d\n, Pin 2: %d\n, Pin 3: %d\n, Pin 4: %d\n, Pin 5: %d\n, Pin 6: %d\n, Pin 7: %d\n, Pin 8: %d\n\n\n",
                 value_1, value_2, value_3, value_4, value_5, value_6, value_7, value_8);

        // Delay cho lần đọc tiếp theo
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
