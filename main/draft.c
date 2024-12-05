#include <stdio.h>
#include "esp_adc/adc_cali.h"
#include "driver/mcpwm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Định nghĩa các chân quang trở (ADC)
#define NUM_SENSORS 8
int sensor_pins[NUM_SENSORS] = {34, 35, 32, 33, 25, 26, 27, 14};

// Định nghĩa các chân PWM cho động cơ
#define MOTOR_LEFT_FORWARD_PIN 18
#define MOTOR_LEFT_BACKWARD_PIN 19
#define MOTOR_RIGHT_FORWARD_PIN 21
#define MOTOR_RIGHT_BACKWARD_PIN 22

// Giá trị ngưỡng để nhận biết đường line (thay đổi tùy vào quang trở và ánh sáng)
#define LINE_THRESHOLD 2000

// Hàm khởi tạo ADC cho quang trở
void init_adc() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        adc1_config_width(ADC_WIDTH_BIT_12);  // Cài đặt độ rộng của ADC
        adc1_config_channel_atten(sensor_pins[i], ADC_ATTEN_DB_0); // Cài đặt độ giảm của ADC (0dB là điện áp tối đa 1V)
    }
}

// Hàm khởi tạo PWM cho động cơ
void init_pwm() {
    mcpwm_config_t pwm_config = {
        .frequency = 1000,    // Tần số PWM: 1kHz
        .cmpr_a = 0,          // Duty cycle của motor trái
        .cmpr_b = 0,          // Duty cycle của motor phải
        .duty_mode = MCPWM_DUTY_MODE_0,
    };

    // Cấu hình PWM cho động cơ trái
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR_LEFT_FORWARD_PIN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MOTOR_LEFT_BACKWARD_PIN);

    // Cấu hình PWM cho động cơ phải
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, MOTOR_RIGHT_FORWARD_PIN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, MOTOR_RIGHT_BACKWARD_PIN);
}

// Hàm điều khiển động cơ: giá trị pwm từ 0-100 (tỉ lệ phần trăm)
void motor_control(int left_speed, int right_speed) {
    // Điều khiển động cơ trái
    if (left_speed > 0) {
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, left_speed);  // Quay về phía trước
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0B, 0);           // Dừng quay ngược lại
    } else if (left_speed < 0) {
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, 0);           // Dừng quay về phía trước
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0B, -left_speed); // Quay ngược lại
    } else {
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, 0);  // Dừng động cơ trái
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0B, 0);  // Dừng động cơ trái
    }

    // Điều khiển động cơ phải
    if (right_speed > 0) {
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM1A, right_speed);  // Quay về phía trước
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM1B, 0);            // Dừng quay ngược lại
    } else if (right_speed < 0) {
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM1A, 0);            // Dừng quay về phía trước
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM1B, -right_speed);  // Quay ngược lại
    } else {
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM1A, 0);  // Dừng động cơ phải
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM1B, 0);  // Dừng động cơ phải
    }
}

// Hàm đọc giá trị quang trở
int read_line_sensors() {
    int sensor_values[NUM_SENSORS];
    int sum = 0;

    for (int i = 0; i < NUM_SENSORS; i++) {
        sensor_values[i] = adc1_get_raw(sensor_pins[i]);  // Đọc giá trị từ quang trở
        sum += sensor_values[i];
    }

    // Tính giá trị trung bình
    int avg_value = sum / NUM_SENSORS;
    return avg_value;
}

// Hàm điều khiển robot dựa trên giá trị quang trở
void line_following_task(void *pvParameter) {
    while (1) {
        int avg_value = read_line_sensors();
        ESP_LOGI("LineFollower", "Average Sensor Value: %d", avg_value);

        if (avg_value > LINE_THRESHOLD) {
            // Nếu giá trị trung bình lớn hơn ngưỡng, di chuyển về phía trước
            motor_control(50, 50);  // Cả hai bánh xe đi tới
        } else {
            // Nếu không nhận diện được đường, dừng lại
            motor_control(0, 0);  // Dừng động cơ
        }

        vTaskDelay(pdMS_TO_TICKS(100));  // Delay 100ms trước khi tiếp tục vòng lặp
    }
}

void app_main() {
    // Khởi tạo ADC và PWM
    init_adc();
    init_pwm();

    // Tạo task theo dõi đường line
    xTaskCreate(line_following_task, "Line Following", 2048, NULL, 5, NULL);
}
