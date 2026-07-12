#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include <esp_http_server.h>
#include "web_UI.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "driver/gpio.h"

#include "servo.h"
#include "pump.h"
#include "blower.h"


#define ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define ESP_MAX_STA_CONN   CONFIG_ESP_MAX_STA_CONN


static const char *TAG1 = "wifi access point";
const char *TAG2 = "http server";


//wifi initialization 

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t * event = (wifi_event_ap_staconnected_t *) event_data;
        ESP_LOGI(TAG1,"station " MACSTR " join,AID=%d",MAC2STR(event->mac),event->aid);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t * event = (wifi_event_ap_stadisconnected_t *) event_data;
        ESP_LOGI(TAG1,"station " MACSTR " leave,AID=%d",MAC2STR(event->mac),event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();


    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));


    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = ESP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }};
    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();


    ESP_LOGI(TAG1, "wifi_init_softap finished.SSID:%s password:%s channel:%d",ESP_WIFI_SSID,ESP_WIFI_PASS,ESP_WIFI_CHANNEL);
}


// movement cotrol

#define MD_IN1 4   //MD stands for motor driver
#define MD_IN2 16
#define MD_IN3 17
#define MD_IN4 5


typedef enum {
    MOVE_STOP,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT
} movement_t;

static movement_t stop = MOVE_STOP;
static movement_t up = MOVE_UP;
static movement_t down = MOVE_DOWN;
static movement_t left = MOVE_LEFT;
static movement_t right = MOVE_RIGHT;




esp_err_t movement_control_handler(httpd_req_t *req)
{
    movement_t move = *(movement_t *)req->user_ctx;

    switch (move)
{
    case MOVE_STOP:
    // Left motor stop
    gpio_set_level(MD_IN1, 0);
    gpio_set_level(MD_IN2, 0);

    // Right motor stop
    gpio_set_level(MD_IN3, 0);
    gpio_set_level(MD_IN4, 0);
    break;

    case MOVE_UP:      
    // Left motor forward
    gpio_set_level(MD_IN1, 0);
    gpio_set_level(MD_IN2, 1);

    // Right motor forward
    gpio_set_level(MD_IN3, 0);
    gpio_set_level(MD_IN4, 1);
    break;

    case MOVE_DOWN:    
    // Left motor reverse
    gpio_set_level(MD_IN1, 1);
    gpio_set_level(MD_IN2, 0);

    // Right motor reverse
    gpio_set_level(MD_IN3, 1);
    gpio_set_level(MD_IN4, 0);
    break;

    case MOVE_LEFT:    
    // Left motor reverse
    gpio_set_level(MD_IN1, 1);
    gpio_set_level(MD_IN2, 0);

    // Right motor forward
    gpio_set_level(MD_IN3, 0);
    gpio_set_level(MD_IN4, 1);
    break;

    case MOVE_RIGHT:   // Pivot right
    // Left motor forward
    gpio_set_level(MD_IN1, 0);
    gpio_set_level(MD_IN2, 1);

    // Right motor reverse
    gpio_set_level(MD_IN3, 1);
    gpio_set_level(MD_IN4, 0);
    break;
}
    ESP_LOGI(TAG2, "Movement control handler called: %d", move);
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}


httpd_uri_t stop_uri = {
    .uri = "/movement/stop",
    .method = HTTP_GET,
    .handler = movement_control_handler,
    .user_ctx = &stop
};

httpd_uri_t up_uri = {
    .uri = "/movement/up",
    .method = HTTP_GET,
    .handler = movement_control_handler,
    .user_ctx = &up
};

httpd_uri_t down_uri = {
    .uri = "/movement/down",
    .method = HTTP_GET,
    .handler = movement_control_handler,
    .user_ctx = &down
};

httpd_uri_t left_uri = {
    .uri = "/movement/left",
    .method = HTTP_GET,
    .handler = movement_control_handler,
    .user_ctx = &left
};

httpd_uri_t right_uri = {
    .uri = "/movement/right",
    .method = HTTP_GET,
    .handler = movement_control_handler,
    .user_ctx = &right
};


// server set up

esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG2, "GET handler called");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)web_UI, web_UI_LEN);
    return ESP_OK;
}

httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL
};

esp_err_t http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 32;
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG1, "HTTP server started");
        if (httpd_register_uri_handler(server,&uri_get) == ESP_OK)
        {
            ESP_LOGI(TAG1, "URI handler registered successfully");
        }
        else {
            ESP_LOGI(TAG1, "Failed to register URI handler");
        }
        // Movement
        httpd_register_uri_handler(server, &stop_uri);
        httpd_register_uri_handler(server, &up_uri);
        httpd_register_uri_handler(server, &down_uri);
        httpd_register_uri_handler(server, &left_uri);
        httpd_register_uri_handler(server, &right_uri);

        // Pump
        httpd_register_uri_handler(server, &pump_on_uri);
        httpd_register_uri_handler(server, &pump_off_uri);
        httpd_register_uri_handler(server, &pump_stop_uri);
        httpd_register_uri_handler(server, &pump_up_uri);
        httpd_register_uri_handler(server, &pump_down_uri);
        httpd_register_uri_handler(server, &pump_left_uri);
        httpd_register_uri_handler(server, &pump_right_uri);

        // Blower
        httpd_register_uri_handler(server, &blower_on_uri);
        httpd_register_uri_handler(server, &blower_off_uri);
        httpd_register_uri_handler(server,&blower_stop_uri);
        httpd_register_uri_handler(server, &blower_up_uri);
        httpd_register_uri_handler(server, &blower_down_uri);
        httpd_register_uri_handler(server, &blower_left_uri);
        httpd_register_uri_handler(server, &blower_right_uri);
        return ESP_OK;

    }
    ESP_LOGI(TAG1, "Failed to start HTTP server");
    return ESP_FAIL;
}


void app_main(void)
{
    //server set up
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG1, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
    ESP_LOGI(TAG1, "Web Server is running ... ...\n");
    http_server();
    
    //motor setup
    gpio_config_t motor_config = {
    .pin_bit_mask =
        (1ULL << MD_IN1) |
        (1ULL << MD_IN2) |
        (1ULL << MD_IN3) |
        (1ULL << MD_IN4),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&motor_config);
    gpio_set_level(MD_IN1, 0);
    gpio_set_level(MD_IN2, 0);
    gpio_set_level(MD_IN3, 0);
    gpio_set_level(MD_IN4, 0);
    
    //pump and blower setup
    gpio_config_t pump_power_config = {
        .pin_bit_mask = 1ULL << PUMP_POWER_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&pump_power_config);
    gpio_set_level(PUMP_POWER_PIN,0);
    gpio_config_t blower_power_config = {
        .pin_bit_mask = 1ULL << BLOWER_POWER_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&blower_power_config);
    gpio_set_level(BLOWER_POWER_PIN,0);
    servo_setup_pwm(BLOWER_UD_GPIO,BLOWER_UD_LEDC_CHANNEL);
    servo_setup_pwm(BLOWER_LR_GPIO, BLOWER_LR_LEDC_CHANNEL);
    servo_setup_pwm(PUMP_UD_GPIO, PUMP_UD_LEDC_CHANNEL);
    servo_setup_pwm(PUMP_LR_GPIO, PUMP_LR_LEDC_CHANNEL);
}
