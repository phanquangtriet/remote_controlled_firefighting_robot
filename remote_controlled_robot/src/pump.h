#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_mac.h"
#include <esp_http_server.h>
#include "web_UI.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "driver/gpio.h"

#include "servo.h"

#define PUMP_LR_GPIO 13
#define PUMP_UD_GPIO 27
#define PUMP_POWER_PIN 32 //relay IN1
#define PUMP_LR_LEDC_CHANNEL LEDC_CHANNEL_3
#define PUMP_UD_LEDC_CHANNEL LEDC_CHANNEL_2

extern const char *TAG2;
extern httpd_uri_t pump_on_uri;
extern httpd_uri_t pump_off_uri;
extern httpd_uri_t pump_stop_uri;
extern httpd_uri_t pump_right_uri;
extern httpd_uri_t pump_left_uri;
extern httpd_uri_t pump_up_uri;
extern httpd_uri_t pump_down_uri;
esp_err_t pump_control_handler(httpd_req_t *req);

#endif