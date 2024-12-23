#include "main.h"

// Hàm điều khiển tốc độ động cơ
void van_toc(int speed_motor1, int speed_motor2) {
    ledcWrite(PWM_CHANNEL_1, constrain(speed_motor1, 0, 255));
    ledcWrite(PWM_CHANNEL_2, constrain(speed_motor2, 0, 255));
}

// Hàm kiểm tra và điều chỉnh line
void kiem_tra_line() {
    int sensorState[8];
    for (int i = 0; i < 8; i++) {
        int sensorValue = analogRead(vt_adc[i]);
        sensorState[i] = (sensorValue > DK_adc[i]) ? 1 : 0; // 1: có line, 0: không có line
    }

    // Nếu cảm biến 4 và 5 bắt được line -> chạy thẳng
    if (sensorState[3] == 1 && sensorState[4] == 1) {
        van_toc(base_speed, base_speed);
    }
    // Nếu cảm biến khác bắt được line, điều chỉnh theo hệ số "a"
    else {
        int a = 0; // Hệ số tăng tốc
        int slow_down = 0; // Hệ số giảm tốc

        if (sensorState[0] || sensorState[1] || sensorState[2]) {
            // Line nằm ở bên trái
            if (sensorState[2]) {
                a = max_a / 3;
                slow_down = base_speed / 3; // Giảm nhẹ bánh không rẽ
            } else if (sensorState[1]) {
                a = (1 * max_a) / 2;
                slow_down = base_speed / 3;
            } else if (sensorState[0]) {
                a = max_a;
                slow_down = base_speed; // Giảm nhiều hơn để hỗ trợ rẽ gấp
            }
            van_toc(base_speed - a - slow_down, base_speed + a); // Quay trái
        } else if (sensorState[5] || sensorState[6] || sensorState[7]) {
            // Line nằm ở bên phải
            if (sensorState[5]) {
                a = max_a / 3;
                slow_down = base_speed / 3; // Giảm nhẹ bánh không rẽ
            } else if (sensorState[6]) {
                a = (1 * max_a) / 2;
                slow_down = base_speed / 3;
            } else if (sensorState[7]) {
                a = max_a;
                slow_down = base_speed; // Giảm nhiều hơn để hỗ trợ rẽ gấp
            }
            van_toc(base_speed + a, base_speed - a - slow_down); // Quay phải
        } else {
            // Không có line -> dừng xe
            van_toc(0, 50);
        }
    }
}

void setup() {
    Serial.begin(9600);

    // Cài đặt GPIO cảm biến
    for (int i = 0; i < 8; i++) {
        pinMode(vt_adc[i], INPUT);
    }

    // Cài đặt PWM cho động cơ
    ledcSetup(PWM_CHANNEL_1, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(MOTOR1_GPIO, PWM_CHANNEL_1);
    ledcSetup(PWM_CHANNEL_2, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(MOTOR2_GPIO, PWM_CHANNEL_2);

    Serial.println("Hệ thống sẵn sàng.");
}

void loop() {
    kiem_tra_line(); // Kiểm tra và điều chỉnh theo line
    delay(2);       // Tần số kiểm tra line
}