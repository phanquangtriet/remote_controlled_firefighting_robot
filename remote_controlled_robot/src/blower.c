//blower control
#include <blower.h>

typedef enum {
    BLOWER_ON,
    BLOWER_OFF,
    BLOWER_STOP,
    BLOWER_MOVE_UP,
    BLOWER_MOVE_DOWN,
    BLOWER_MOVE_LEFT,
    BLOWER_MOVE_RIGHT
} blower_movement_t;



static blower_movement_t blower_on = BLOWER_ON;
static blower_movement_t blower_off = BLOWER_OFF;
static blower_movement_t blower_stop = BLOWER_STOP;
static blower_movement_t blower_up = BLOWER_MOVE_UP;
static blower_movement_t blower_down = BLOWER_MOVE_DOWN;
static blower_movement_t blower_left = BLOWER_MOVE_LEFT;
static blower_movement_t blower_right = BLOWER_MOVE_RIGHT;


esp_err_t blower_control_handler(httpd_req_t *req)
{
    blower_movement_t move = *(blower_movement_t *)req->user_ctx;

    switch (move)
    {
    case BLOWER_ON:
        gpio_set_level(BLOWER_POWER_PIN, 1); 
        break;
    case BLOWER_OFF:
        gpio_set_level(BLOWER_POWER_PIN, 0); 
        break;
    case BLOWER_STOP:
        servo_set_angle(90,BLOWER_UD_LEDC_CHANNEL);
        servo_set_angle(90,BLOWER_LR_LEDC_CHANNEL);
        break;
    case BLOWER_MOVE_UP:
        servo_set_angle(SLOW_RIGHT_SPEED,BLOWER_UD_LEDC_CHANNEL);
        break;

    case BLOWER_MOVE_DOWN:
        servo_set_angle(SLOW_LEFT_SPEED,BLOWER_UD_LEDC_CHANNEL);
        break;

    case BLOWER_MOVE_LEFT:
        servo_set_angle(SLOW_RIGHT_SPEED,BLOWER_LR_LEDC_CHANNEL);
        break;

    case BLOWER_MOVE_RIGHT:
        servo_set_angle(SLOW_LEFT_SPEED,BLOWER_LR_LEDC_CHANNEL);
        break;
    }
    ESP_LOGI(TAG2, "Blower movement control handler called: %d", move);
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}


httpd_uri_t blower_on_uri = {
    .uri = "/blower/on",
    .method = HTTP_GET,
    .handler = blower_control_handler,
    .user_ctx = &blower_on
};
httpd_uri_t blower_off_uri = {
    .uri = "/blower/off",
    .method = HTTP_GET,
    .handler = blower_control_handler,
    .user_ctx = &blower_off
};
httpd_uri_t blower_stop_uri = {
    .uri = "/blower/stop",
    .method = HTTP_GET,
    .handler = blower_control_handler,
    .user_ctx = &blower_stop
};
httpd_uri_t blower_up_uri = {
    .uri = "/blower/up",
    .method = HTTP_GET,
    .handler = blower_control_handler,
    .user_ctx = &blower_up
};
httpd_uri_t blower_down_uri = {
    .uri = "/blower/down",
    .method = HTTP_GET,
    .handler = blower_control_handler,
    .user_ctx = &blower_down
};
httpd_uri_t blower_left_uri = {
    .uri = "/blower/left",
    .method = HTTP_GET,
    .handler = blower_control_handler,
    .user_ctx = &blower_left
};
httpd_uri_t blower_right_uri = {
    .uri = "/blower/right",
    .method = HTTP_GET,
    .handler = blower_control_handler,
    .user_ctx = &blower_right
};
