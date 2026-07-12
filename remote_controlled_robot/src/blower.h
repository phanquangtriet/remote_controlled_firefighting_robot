#ifndef BLOWER_CONTROL_H
#define BLOWER_CONTROL_H

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

#define BLOWER_UD_GPIO 12
#define BLOWER_LR_GPIO 14
#define BLOWER_POWER_PIN 33 //relay IN2
#define BLOWER_UD_LEDC_CHANNEL LEDC_CHANNEL_0
#define BLOWER_LR_LEDC_CHANNEL LEDC_CHANNEL_1

extern const char *TAG2;
extern httpd_uri_t blower_on_uri;
extern httpd_uri_t blower_off_uri;
extern httpd_uri_t blower_stop_uri;
extern httpd_uri_t blower_left_uri;
extern httpd_uri_t blower_right_uri;
extern httpd_uri_t blower_up_uri;
extern httpd_uri_t blower_down_uri;
esp_err_t blower_control_handler(httpd_req_t *req);

#endif