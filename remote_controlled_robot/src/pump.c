//pump control
#include <pump.h>



typedef enum {
    PUMP_ON,
    PUMP_OFF,
    PUMP_STOP,
    PUMP_MOVE_UP,
    PUMP_MOVE_DOWN,
    PUMP_MOVE_LEFT,
    PUMP_MOVE_RIGHT
} pump_movement_t;


static pump_movement_t pump_on = PUMP_ON;
static pump_movement_t pump_off = PUMP_OFF;
static pump_movement_t pump_stop = PUMP_STOP;
static pump_movement_t pump_up = PUMP_MOVE_UP;
static pump_movement_t pump_down = PUMP_MOVE_DOWN;
static pump_movement_t pump_left = PUMP_MOVE_LEFT;
static pump_movement_t pump_right = PUMP_MOVE_RIGHT;



esp_err_t pump_control_handler(httpd_req_t *req)
{
    pump_movement_t move = *(pump_movement_t *)req->user_ctx;

    switch (move)
    {
    case PUMP_ON:
        gpio_set_level(PUMP_POWER_PIN, 1); 
        break;
    case PUMP_OFF:
        gpio_set_level(PUMP_POWER_PIN, 0); 
        break;
    case PUMP_STOP:
        servo_set_angle(90,PUMP_UD_LEDC_CHANNEL);
        servo_set_angle(90,PUMP_LR_LEDC_CHANNEL);
        break;
    case PUMP_MOVE_UP:
        servo_set_angle(SLOW_RIGHT_SPEED, PUMP_UD_LEDC_CHANNEL);
        break;

    case PUMP_MOVE_DOWN:
        servo_set_angle(SLOW_LEFT_SPEED, PUMP_UD_LEDC_CHANNEL);
        break;

    case PUMP_MOVE_LEFT:
        servo_set_angle(SLOW_RIGHT_SPEED,PUMP_LR_LEDC_CHANNEL);
        break;

    case PUMP_MOVE_RIGHT:
        servo_set_angle(SLOW_LEFT_SPEED,PUMP_LR_LEDC_CHANNEL);
        break;
    }
    ESP_LOGI(TAG2, "Pump movement control handler called: %d", move);
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}



httpd_uri_t pump_on_uri = {
    .uri = "/pump/on",
    .method = HTTP_GET,
    .handler = pump_control_handler,
    .user_ctx = &pump_on
};
httpd_uri_t pump_off_uri = {
    .uri = "/pump/off",
    .method = HTTP_GET,
    .handler = pump_control_handler,
    .user_ctx = &pump_off
};
httpd_uri_t pump_stop_uri = {
    .uri = "/pump/stop",
    .method = HTTP_GET,
    .handler = pump_control_handler,
    .user_ctx = &pump_stop
};
httpd_uri_t pump_up_uri = {
    .uri = "/pump/up",
    .method = HTTP_GET,
    .handler = pump_control_handler,
    .user_ctx = &pump_up
};
httpd_uri_t pump_down_uri = {
    .uri = "/pump/down",
    .method = HTTP_GET,
    .handler = pump_control_handler,
    .user_ctx = &pump_down
};
httpd_uri_t pump_left_uri = {
    .uri = "/pump/left",
    .method = HTTP_GET,
    .handler = pump_control_handler,
    .user_ctx = &pump_left
};
httpd_uri_t pump_right_uri = {
    .uri = "/pump/right",
    .method = HTTP_GET,
    .handler = pump_control_handler,
    .user_ctx = &pump_right
};