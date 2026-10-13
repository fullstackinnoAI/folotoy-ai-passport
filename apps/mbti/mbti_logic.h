// 性格小测的纯逻辑：页面流转、四维计分、类型文案。不依赖 ESP-IDF / LVGL。
#pragma once

#include <stdbool.h>

#define MBTI_AGE_MIN 6
#define MBTI_AGE_MAX 99
#define MBTI_QUESTION_COUNT 16
#define MBTI_CHOICE_COUNT 5
#define MBTI_TYPE_COUNT 16

typedef enum {
    MBTI_SCREEN_WELCOME = 0,
    MBTI_SCREEN_AGE,
    MBTI_SCREEN_GENDER,
    MBTI_SCREEN_QUESTION,
    MBTI_SCREEN_SUMMARY,
    MBTI_SCREEN_CARD,
} mbti_screen_t;

typedef enum {
    MBTI_EVT_UP = 0,
    MBTI_EVT_DOWN,
    MBTI_EVT_OK,
    MBTI_EVT_OK_LONG,
} mbti_event_t;

// gender: 0 女，1 男。choice / answers: 0 非常不赞同 … 4 非常赞同，未答为 -1。
typedef struct {
    mbti_screen_t screen;
    int age;
    int gender;
    int question;
    int choice;
    int answers[MBTI_QUESTION_COUNT];
} mbti_session_t;

typedef struct {
    char code[5];
    const char *title;
    const char *blurb;
} mbti_result_t;

void mbti_session_init(mbti_session_t *session);
void mbti_session_apply(mbti_session_t *session, mbti_event_t event);

const char *mbti_question_text(int index);
const char *mbti_choice_text(int choice);
const char *mbti_gender_text(int gender);

// 四题一维，每题 1..5。正向题累加原分，反向题累加 6-原分。
// 总分大于 12 取该维的前一极（E/S/T/J），否则取后一极（I/N/F/P）。
// 全部答案必须是 0..4。返回 false 时 result 不变。
bool mbti_score(const int answers[MBTI_QUESTION_COUNT], mbti_result_t *result);
