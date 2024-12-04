#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

// GPIO định nghĩa cho PWM
#define PWM_GPIO_1 4
#define PWM_GPIO_2 15

// Tần số và độ phân giải PWM
#define PWM_FREQ_HZ 1000       // 1 kHz
#define PWM_RESOLUTION LEDC_TIMER_10_BIT // Độ phân giải 10 bit
#define PWM_DUTY_MAX 1023      // Duty cycle 100% (2^10 - 1)

void app_main(void) {
    // 1. Cấu hình LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // 2. Cấu hình kênh PWM cho GPIO 4
    ledc_channel_config_t ledc_channel1 = {
        .gpio_num = PWM_GPIO_1,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0, // Ban đầu duty cycle = 0
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel1);

    // 3. Cấu hình kênh PWM cho GPIO 15
    ledc_channel_config_t ledc_channel2 = {
        .gpio_num = PWM_GPIO_2,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0, // Ban đầu duty cycle = 0
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel2);

    // Vòng lặp điều khiển PWM
    while (1) {
        // Bật PWM trên GPIO 4
        printf("PWM ON GPIO 4\n");
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, PWM_DUTY_MAX); // Duty 100%
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Chạy trong 5 giây

        // Tắt PWM trên GPIO 4
        printf("PWM OFF GPIO 4\n");
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0); // Duty 0%
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(2000)); // Nghỉ 2 giây

        // Bật PWM trên GPIO 15
        printf("PWM ON GPIO 15\n");
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, PWM_DUTY_MAX); // Duty 100%
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Chạy trong 5 giây

        // Tắt PWM trên GPIO 15
        printf("PWM OFF GPIO 15\n");
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0); // Duty 0%
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
        vTaskDelay(pdMS_TO_TICKS(2000)); // Nghỉ 2 giây
    }
}
