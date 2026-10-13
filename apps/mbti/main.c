// 性格小测。三键完成年龄、性别、16 道题，再看类型短评和卡片。
#include "bsp_battery.h"
#include "bsp_button.h"
#include "bsp_display.h"
#include "bsp_i2c.h"
#include "bsp_pins.h"
#include "key_guard.h"
#include "mbti_logic.h"
#include "mbti_ui.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static const char *TAG = "mbti";

#define INPUT_QUEUE_DEPTH 8
#define KEY_GUARD_MAX_US (10LL * 1000 * 1000)

typedef struct {
    bsp_btn_t btn;
    bsp_btn_ev_t event;
} input_event_t;

static mbti_session_t s_session;
static bool s_battery_ok;
static QueueHandle_t s_input_queue;
static TaskHandle_t s_input_task;
static volatile bool s_input_ready;
static key_guard_t s_key_guard;

static int current_soc(void)
{
    if (!s_battery_ok) return -1;
    return bsp_battery_soc();
}

static void render(void)
{
    mbti_ui_show(&s_session, current_soc());
}

static mbti_event_t *map_event(bsp_btn_t btn, bsp_btn_ev_t event, mbti_event_t *out)
{
    if (event == BSP_BTN_LONG && btn == BSP_BTN_OK) {
        *out = MBTI_EVT_OK_LONG;
        return out;
    }
    if (event != BSP_BTN_CLICK) return NULL;
    if (btn == BSP_BTN_UP) *out = MBTI_EVT_UP;
    else if (btn == BSP_BTN_DOWN) *out = MBTI_EVT_DOWN;
    else if (btn == BSP_BTN_OK) *out = MBTI_EVT_OK;
    else return NULL;
    return out;
}

static void process_input(const input_event_t *input)
{
    mbti_event_t event;
    if (!map_event(input->btn, input->event, &event)) return;
    mbti_session_apply(&s_session, event);
    if (!bsp_lvgl_lock(500)) return;
    render();
    bsp_lvgl_unlock();
}

static void input_task(void *arg)
{
    (void)arg;
    input_event_t input;
    for (;;) {
        if (xQueueReceive(s_input_queue, &input, portMAX_DELAY) == pdTRUE) {
            process_input(&input);
        }
    }
}

static void on_key(bsp_btn_t btn, bsp_btn_ev_t ev, void *user)
{
    (void)user;
    const bool was_armed = s_key_guard.armed;
    if (key_guard_consume(&s_key_guard, ev == BSP_BTN_RELEASE, esp_timer_get_time())) {
        if (was_armed && !s_key_guard.armed) {
            ESP_LOGI(TAG, "唤醒键已松手,按键恢复响应");
        }
        return;
    }
    if (!s_input_ready || !s_input_queue) return;
    const input_event_t input = {.btn = btn, .event = ev};
    (void)xQueueSend(s_input_queue, &input, 0);
}

void app_main(void)
{
    ESP_LOGI(TAG, "性格小测启动");
    const uint32_t wake_causes = esp_sleep_get_wakeup_causes();
    if (wake_causes & (1u << ESP_SLEEP_WAKEUP_GPIO)) {
        key_guard_arm(&s_key_guard, esp_timer_get_time(), KEY_GUARD_MAX_US);
        ESP_LOGI(TAG, "按键唤醒:先屏蔽按键动作,直到那颗键松手");
    }

    bsp_i2c_init();
    if (bsp_display_init() != ESP_OK || !bsp_lvgl_init()) {
        ESP_LOGE(TAG, "显示/LVGL 初始化失败。"
                      "检查 SPI 接线(MOSI=%d SCLK=%d CS=%d DC=%d BL=%d)",
                 BSP_LCD_MOSI, BSP_LCD_SCLK, BSP_LCD_CS, BSP_LCD_DC, BSP_LCD_BL);
        return;
    }
    bsp_display_backlight(100);

    mbti_session_init(&s_session);
    s_input_queue = xQueueCreate(INPUT_QUEUE_DEPTH, sizeof(input_event_t));
    if (!s_input_queue ||
        xTaskCreate(input_task, "mbti_input", 8192, NULL, 5, &s_input_task) != pdPASS) {
        ESP_LOGE(TAG, "按键事件任务创建失败");
        return;
    }
    if (bsp_button_init(on_key, NULL) != ESP_OK) {
        ESP_LOGE(TAG, "按键初始化失败");
        return;
    }
    if (s_key_guard.armed) {
        const bool released = bsp_button_read_mv() >= BSP_BTN_MV_RELEASED_MIN;
        key_guard_sync_released(&s_key_guard, released);
    }

    s_battery_ok = bsp_battery_init() == ESP_OK;
    if (!s_battery_ok) ESP_LOGW(TAG, "电量计不可用");

    if (bsp_lvgl_lock(1000)) {
        render();
        bsp_lvgl_unlock();
        s_input_ready = true;
    }
}
