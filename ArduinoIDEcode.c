#include <Arduino.h>

// Định nghĩa các chân cảm biến
#define INPUT_PIN_1 34
#define INPUT_PIN_2 35
#define INPUT_PIN_3 32
#define INPUT_PIN_4 33
#define INPUT_PIN_5 25
#define INPUT_PIN_6 26
#define INPUT_PIN_7 27
#define INPUT_PIN_8 14

int vt_adc[] = {INPUT_PIN_1, INPUT_PIN_2, INPUT_PIN_3, INPUT_PIN_4,
                INPUT_PIN_5, INPUT_PIN_6, INPUT_PIN_7, INPUT_PIN_8};

int sensorValues[8]; // Mảng lưu giá trị cảm biến

// Định nghĩa GPIO cho động cơ và PWM
#define MOTOR1_GPIO 4      // Động cơ 1
#define MOTOR2_GPIO 15     // Động cơ 2
#define PWM_CHANNEL_1 0    // PWM kênh 1
#define PWM_CHANNEL_2 1    // PWM kênh 2
#define PWM_FREQ 5000      // Tần số PWM
#define PWM_RESOLUTION 8   // Độ phân giải PWM

// Định nghĩa nút nhấn
#define BUTTON_PIN 22      // Nút nhấn nối với GPIO 22

bool buttonState = false;   // Trạng thái nút nhấn (lần 1 / lần 2)
int DK_adc[8];              // Mảng lưu giá trị trung bình cho từng cảm biến

// Khởi tạo PWM động cơ
void van_toc_init() {
    ledcSetup(PWM_CHANNEL_1, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(MOTOR1_GPIO, PWM_CHANNEL_1);
    ledcSetup(PWM_CHANNEL_2, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(MOTOR2_GPIO, PWM_CHANNEL_2);
}

void van_toc(int speed_motor1, int speed_motor2) {
    speed_motor1 = constrain(speed_motor1, 0, 255);
    speed_motor2 = constrain(speed_motor2, 0, 255);
    ledcWrite(PWM_CHANNEL_1, speed_motor1);
    ledcWrite(PWM_CHANNEL_2, speed_motor2);
}


void setup() {
    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // nút nhấn
    for (int i = 0; i < 8; i++) pinMode(vt_adc[i], INPUT);
    van_toc_init();
}

void dieu_kien_line() {
    // Cho xe chạy trước 2 giây để lướt qua vạch line
    van_toc(200, 200);  // Đặt tốc độ động cơ
    delay(2000);         // Chạy trong 2 giây

    // Sau 2 giây, dừng xe
    van_toc(0, 0);      // Dừng động cơ

    // Khởi tạo dữ liệu max và min
    int MAX_DATA[8] = {0};
    int MIN_DATA[8] = {0};

    for (int i = 0; i < 8; i++) {
        MAX_DATA[i] = analogRead(vt_adc[i]);
        MIN_DATA[i] = analogRead(vt_adc[i]);
    }

    // Ghi lại max và min trong vòng lặp
    for (int i = 0; i < 200; i++) {
        for (int j = 0; j < 8; j++) {
            int x = analogRead(vt_adc[j]);
            if (x > MAX_DATA[j]) MAX_DATA[j] = x;
            if (x < MIN_DATA[j]) MIN_DATA[j] = x;
        }
        delay(2);
    }

    // Tính trung bình và in kết quả
    for (int i = 0; i < 8; i++) {
        DK_adc[i] = (MAX_DATA[i] + MIN_DATA[i]) / 2;
        Serial.print("Cảm biến ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(DK_adc[i]);
    }

    van_toc(0, 0);    // Dừng động cơ sau khi nhận diện xong
}

void bat_line_logic() {
    int sensor[8];
    bool sensor_line[8];

    // Đọc giá trị cảm biến và so sánh với DK_adc
    for (int i = 0; i < 8; i++) {
        sensor[i] = analogRead(vt_adc[i]);
        sensor_line[i] = sensor[i] > DK_adc[i];
    }

    // Điều kiện điều khiển xe
    if (sensor_line[3] && sensor_line[4]) {
        // Đi thẳng khi 2 cảm biến ở giữa phát hiện line
        van_toc(200, 200);
    } 
    else if (sensor_line[0] || sensor_line[1] || sensor_line[2]) {
        // Rẽ phải khi cảm biến bên trái phát hiện line
        van_toc(120, 200);
    } 
    else if (sensor_line[5] || sensor_line[6] || sensor_line[7]) {
        // Rẽ trái khi cảm biến bên phải phát hiện line
        van_toc(200, 120);
    } 
    else {
        // Tìm line nhẹ nhàng (lùi nhẹ hoặc quay đầu)
        van_toc(80, 80); // Xe chạy chậm tìm lại line
    }
}

void dieu_kien_chay_xe() {
    Serial.println("Chế độ điều khiển xe chạy đã được kích hoạt!");
    while (true) {
        bat_line_logic();
        delay(10);
    }
}

unsigned long lastButtonPress = 0; // Lưu thời gian nhấn nút trước đó
unsigned long debounceDelay = 200; // Thời gian chống dội phím (200ms)

void loop() {
    // Kiểm tra nút nhấn với chống dội phím
    if (digitalRead(BUTTON_PIN) == LOW && (millis() - lastButtonPress > debounceDelay)) {
        lastButtonPress = millis();  // Cập nhật thời gian nhấn nút

        // Chờ nhả nút
        while (digitalRead(BUTTON_PIN) == LOW);

        // Đổi trạng thái nhấn nút
        buttonState = !buttonState;

        if (buttonState) {
            Serial.println("Khởi tạo điều kiện line...");
            dieu_kien_line();
        } else {
            Serial.println("Bắt đầu điều khiển xe chạy...");
            dieu_kien_chay_xe();
        }
    }
}

/*  +  Bấm nút nhấn lần 1: Xe chạy lướt qua line và môi trường không có line
 Cảm biến thực hiện quét nhanh trong 2s, với chu kì 2ms -> Thực hiện 1000 lần
 --->> Tính giá trị trung bình giữa các ngưỡng có line và không có line, từ đó suy ra điều kiện bắt line của xe
    +  Bấm nút nhấn lần 2: Xe bắt đầu chạy và dò theo line
    - Thực hiện hàm điều kiện giữa các cảm biến và điều khiển 2 bánh để xe chạy theo line
    +  Lưu í set tốc độ cho xe ở các điều kiện xe chạy khác nhau
     
*/