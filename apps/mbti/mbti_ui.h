// 性格小测的界面。调用方在 LVGL 任务内，或已持有 bsp_lvgl_lock()。
#pragma once

#include "mbti_logic.h"

// soc 为 0..100；读不到电量时传 -1，界面显示占位而不是数字。
void mbti_ui_show(const mbti_session_t *session, int soc);
