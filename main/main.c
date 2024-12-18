#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

// Định nghĩa các chân cảm biến
#define INPUT_PIN_1 GPIO_NUM_34
#define INPUT_PIN_2 GPIO_NUM_35
#define INPUT_PIN_3 GPIO_NUM_32
#define INPUT_PIN_4 GPIO_NUM_33
#define INPUT_PIN_5 GPIO_NUM_25
#define INPUT_PIN_6 GPIO_NUM_26
#define INPUT_PIN_7 GPIO_NUM_27
#define INPUT_PIN_8 GPIO_NUM_14

// Định nghĩa GPIO và PWM cho động cơ
#define MOTOR1_GPIO GPIO_NUM_15
#define MOTOR2_GPIO GPIO_NUM_4
#define PWM_CHANNEL_1 LEDC_CHANNEL_0
#define PWM_CHANNEL_2 LEDC_CHANNEL_1
#define PWM_FREQ 5000
#define PWM_RESOLUTION LEDC_TIMER_8_BIT

// Ngưỡng cảm biến
int DK_adc[8] = {2022, 2384, 2388, 2559, 2233, 2254, 2559, 2553};

// Tốc độ cơ bản
int base_speed = 64; // Đặt lại giá trị phù hợp với độ phân giải PWM 8-bit

// Hệ số điều chỉnh tốc độ
int max_a = 155; // Hệ số "a" lớn nhất cho độ phân giải PWM 8-bit

// Mảng GPIO cảm biến
int vt_adc[] = {INPUT_PIN_1, INPUT_PIN_2, INPUT_PIN_3, INPUT_PIN_4,
                INPUT_PIN_5, INPUT_PIN_6, INPUT_PIN_7, INPUT_PIN_8};

// Hàm cấu hình GPIO cho cảm biến
void configure_gpio() {
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << INPUT_PIN_1) | (1ULL << INPUT_PIN_2) | (1ULL << INPUT_PIN_3) |
                        (1ULL << INPUT_PIN_4) | (1ULL << INPUT_PIN_5) | (1ULL << INPUT_PIN_6) |
                        (1ULL << INPUT_PIN_7) | (1ULL << INPUT_PIN_8),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

// Hàm cấu hình PWM cho động cơ
void configure_pwm() {
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t motor1_channel = {
        .channel = PWM_CHANNEL_1,
        .duty = 0,
        .gpio_num = MOTOR1_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&motor1_channel);

    ledc_channel_config_t motor2_channel = {
        .channel = PWM_CHANNEL_2,
        .duty = 0,
        .gpio_num = MOTOR2_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&motor2_channel);
}

// Hàm điều khiển tốc độ động cơ
void van_toc(int speed_motor1, int speed_motor2) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_1, speed_motor1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_1);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_2, speed_motor2);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL_2);
}

// Hàm kiểm tra và điều chỉnh line
void kiem_tra_line() {
    int sensorState[8];
    for (int i = 0; i < 8; i++) {
        int sensorValue = adc1_get_raw(vt_adc[i]);
        sensorState[i] = (sensorValue > DK_adc[i]) ? 1 : 0; // 1: có line, 0: không có line
    }

    if (sensorState[3] == 1 && sensorState[4] == 1) {
        van_toc(base_speed, base_speed);
    } else {
        int a = 0; // Hệ số tăng tốc
        int slow_down = 0; // Hệ số giảm tốc

        if (sensorState[0] || sensorState[1] || sensorState[2]) {
            if (sensorState[2]) {
                a = max_a / 3;
                slow_down = base_speed / 3;
            } else if (sensorState[1]) {
                a = (2 * max_a) / 3;
                slow_down = base_speed / 3;
            } else if (sensorState[0]) {
                a = max_a;
                slow_down = 2 * base_speed / 3;
            }
            van_toc(base_speed - a - slow_down, base_speed + a);
        } else if (sensorState[5] || sensorState[6] || sensorState[7]) {
            if (sensorState[5]) {
                a = max_a / 3;
                slow_down = base_speed / 3;
            } else if (sensorState[6]) {
                a = (2 * max_a) / 3;
                slow_down = base_speed / 3;
            } else if (sensorState[7]) {
                a = max_a;
                slow_down = 2 * base_speed / 3;
            }
            van_toc(base_speed + a, base_speed - a - slow_down);
        } else {
            van_toc(0, 0);
        }
    }
}

void app_main() {
    configure_gpio();
    configure_pwm();

    while (1) {
        kiem_tra_line();
        vTaskDelay(pdMS_TO_TICKS(1)); // Tần số kiểm tra line
    }
}
