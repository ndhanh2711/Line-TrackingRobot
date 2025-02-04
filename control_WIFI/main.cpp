#include <Arduino.h>


// Thông tin Blynk
#define BLYNK_TEMPLATE_ID "TMPL6KfJw9Rvl"
#define BLYNK_TEMPLATE_NAME "car control"
#define BLYNK_AUTH_TOKEN "UedgK1LIs3lQHV8QquRQKZ12hfc2mLej"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
// Thông tin WiFi
char ssid[] = "Quang Hai T3";
char pass[] = "19741975";

// Cấu hình PWM
#define PWM1_CHANNEL  0   // Kênh PWM 0
#define PWM2_CHANNEL  1   // Kênh PWM 1
#define PWM1_PIN      4   // Chân GPIO4 (Bánh trái)
#define PWM2_PIN      15  // Chân GPIO15 (Bánh phải)
#define PWM_FREQ      5000  // Tần số PWM 5kHz
#define PWM_RES       8     // Độ phân giải 8-bit (0 - 255)

void setup() {
    Serial.begin(115200);

    // Cấu hình PWM
    ledcSetup(PWM1_CHANNEL, PWM_FREQ, PWM_RES);
    ledcSetup(PWM2_CHANNEL, PWM_FREQ, PWM_RES);
    
    // Gán PWM vào chân GPIO
    ledcAttachPin(PWM1_PIN, PWM1_CHANNEL);
    ledcAttachPin(PWM2_PIN, PWM2_CHANNEL);

    // Kết nối WiFi thủ công để kiểm tra lỗi
    WiFi.begin(ssid, pass);
    Serial.print("Đang kết nối WiFi...");
    
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
        retry++;
        if (retry > 20) {  // Thử 20 lần (~20 giây)
            Serial.println("\nKhông thể kết nối WiFi!");
            return;
        }
    }
    Serial.println("\nWiFi đã kết nối!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // Kết nối Blynk
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect();
}

void loop() {
    if (Blynk.connected()) {
        Blynk.run();  
    } else {
        Serial.println("Mất kết nối Blynk!");
    }
}
// Biến lưu giá trị PWM của 2 bánh
int pwm_left = 0;
int pwm_right = 0;
// Hàm cập nhật động cơ
void updateMotors() {
    ledcWrite(PWM1_CHANNEL, pwm_left);
    ledcWrite(PWM2_CHANNEL, pwm_right);
    Serial.print("Bánh trái: ");
    Serial.print(pwm_left);
    Serial.print(" | Bánh phải: ");
    Serial.println(pwm_right);
}
BLYNK_WRITE(V0) {
    pwm_left = constrain(param.asInt(), 0, 255);
    updateMotors();  // Gọi hàm cập nhật động cơ
}

// Điều khiển bánh phải (V1)
BLYNK_WRITE(V1) {
    pwm_right = constrain(param.asInt(), 0, 255);
    updateMotors();  // Gọi hàm cập nhật động cơ
}

