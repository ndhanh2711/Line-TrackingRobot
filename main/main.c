#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define NUM_LDRS 8
int ldr_pins[NUM_LDRS] = {ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_4, ADC_CHANNEL_5,
                          ADC_CHANNEL_0, ADC_CHANNEL_3, ADC_CHANNEL_1, ADC_CHANNEL_2};
#define PWM_LEFT GPIO_NUM_4
#define PWM_RIGHT GPIO_NUM_15

void setup_pwm(ledc_channel_config_t *pwm_left, ledc_channel_config_t *pwm_right) {
    ledc_timer_config_t pwm_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&pwm_timer);

    pwm_left->gpio_num = PWM_LEFT;
    pwm_left->speed_mode = LEDC_LOW_SPEED_MODE;
    pwm_left->channel = LEDC_CHANNEL_0;
    pwm_left->timer_sel = LEDC_TIMER_0;
    pwm_left->duty = 0;
    ledc_channel_config(pwm_left);

    pwm_right->gpio_num = PWM_RIGHT;
    pwm_right->speed_mode = LEDC_LOW_SPEED_MODE;
    pwm_right->channel = LEDC_CHANNEL_1;
    pwm_right->timer_sel = LEDC_TIMER_0;
    pwm_right->duty = 0;
    ledc_channel_config(pwm_right);
}

void app_main() {
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t adc1_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&adc1_config, &adc1_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    for (int i = 0; i < NUM_LDRS; i++) {
        adc_oneshot_config_channel(adc1_handle, ldr_pins[i], &channel_config);
    }

    ledc_channel_config_t pwm_left, pwm_right;
    setup_pwm(&pwm_left, &pwm_right);

    while (1) {
        int ldr_values[NUM_LDRS];
        int left_sum = 0, right_sum = 0;

        for (int i = 0; i < NUM_LDRS; i++) {
            adc_oneshot_read(adc1_handle, ldr_pins[i], &ldr_values[i]);
            if (i < NUM_LDRS / 2) {
                left_sum += ldr_values[i];
            } else {
                right_sum += ldr_values[i];
            }
        }

        if (left_sum > right_sum + 1000) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2000);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 4000);
        } else if (right_sum > left_sum + 1000) {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4000);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 2000);
        } else {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4000);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 4000);
        }

        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
