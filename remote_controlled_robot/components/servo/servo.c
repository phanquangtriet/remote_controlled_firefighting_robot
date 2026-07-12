#include "servo.h"

void servo_setup_pwm(uint8_t servo_pin, ledc_channel_t channel) {
    // Configure the LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = SPEED_MODE,
        .timer_num        = TIMER_NUM,
        .duty_resolution  = DUTY_RESOLUTION,
        .freq_hz          = FREQUENCY,
        .clk_cfg          = CLK_CONFIG
    };
    ledc_timer_config(&ledc_timer);

    // Configure the LEDC channel
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = SPEED_MODE,
        .channel        = channel,
        .timer_sel      = TIMER_NUM,
        .intr_type      = INTR_TYPE,
        .gpio_num       = servo_pin,
        .duty           = 0, // Initial duty cycle
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void servo_set_angle(int angle, ledc_channel_t channel) 
{
    // Convert angle to duty cycle
    int pulsewidth = SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * angle) / SERVO_MAX_DEGREE;
    int duty = (pulsewidth * 8192) / 20000; 
    ledc_set_duty(SPEED_MODE, channel, duty);
    ledc_update_duty(SPEED_MODE, channel);
}