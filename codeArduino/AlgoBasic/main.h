#ifndef MAIN_H 
#define MAIN_H


#include <Arduino.h>
#include <stdio.h>

/**
 * Định nghĩa các chân của cảm biếnbiến
 */
#define INPUT_PIN_1 34
#define INPUT_PIN_2 35
#define INPUT_PIN_3 32
#define INPUT_PIN_4 33
#define INPUT_PIN_5 25
#define INPUT_PIN_6 26
#define INPUT_PIN_7 27
#define INPUT_PIN_8 14

/**
 * Định nghĩa các chân của động cơ
 */
#define MOTOR1_GPIO 15
#define MOTOR2_GPIO 4
#define PWM_CHANNEL_1 0
#define PWM_CHANNEL_2 1
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8

/**
 * Giá trị ngưỡng của cảm biến sau khi tính toán
 */
int DK_adc[8] = {2022, 2384, 2388, 2559, 2233, 2254, 2559, 2553};

// Tốc độ cơ bản
int base_speed = 25;

// Hệ số điều chỉnh tốc độ
int max_a = 120; // Hệ số "a" lớn nhất

// Mảng GPIO cảm biến
int vt_adc[] = {INPUT_PIN_1, INPUT_PIN_2, INPUT_PIN_3, INPUT_PIN_4,
                INPUT_PIN_5, INPUT_PIN_6, INPUT_PIN_7, INPUT_PIN_8};

void van_toc(int speed_motor1, int speed_motor2);

void kiem_tra_line();

void setup();

#endif // FILE_NAME_H