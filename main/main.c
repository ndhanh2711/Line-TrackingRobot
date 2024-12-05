#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

// Cấu hình ADC
#define ADC_CHANNEL ADC_CHANNEL_6  // GPIO34 -> ADC1 Channel 6
#define ADC_WIDTH   ADC_WIDTH_BIT_12 // Độ phân giải 12-bit (0 - 4095)
#define ADC_ATTEN   ADC_ATTEN_DB_0   // Không giảm điện áp (0-1.1V)
#define DEFAULT_VREF 1100           // Điện áp tham chiếu mặc định (mV)

// Hàm hiệu chuẩn ADC
static esp_adc_cal_characteristics_t *adc_chars;

// Hàm khởi tạo ADC
void init_adc() {
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH, DEFAULT_VREF, adc_chars);
}

// Hàm đọc giá trị ADC và chuyển sang điện áp
uint32_t read_adc_voltage() {
    int adc_raw = adc1_get_raw(ADC_CHANNEL); // Đọc giá trị ADC
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_raw, adc_chars); // Chuyển đổi sang mV
    return voltage;
}

void app_main(void) {
    init_adc(); // Khởi tạo ADC
    while (1) {
        uint32_t voltage = read_adc_voltage(); // Đọc điện áp từ quang trở
        printf("Voltage: %d mV\n", voltage); // Hiển thị giá trị điện áp
        vTaskDelay(pdMS_TO_TICKS(1000)); // Đợi 1 giây
    }
}
