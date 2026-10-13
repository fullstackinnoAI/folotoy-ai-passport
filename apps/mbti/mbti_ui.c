#include "mbti_ui.h"

#include "mbti_cards.h"

#include "lvgl.h"
#include <stdio.h>

LV_FONT_DECLARE(mbti_font_16);
LV_FONT_DECLARE(mbti_font_20);

#define COL_BG 0xF3EBDD
#define COL_INK 0x1E2430
#define COL_MUTED 0x6E645A
#define COL_PAPER 0xFFFCF7
#define COL_LINE 0xE3D7C6
#define COL_PICK 0x1F4B3A
#define COL_PICK_INK 0xF7F3EA

static lv_obj_t *s_screen;

static lv_obj_t *screen_create(uint32_t bg)
{
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(bg), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_text_font(screen, &mbti_font_16, 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(COL_INK), 0);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    return screen;
}

static void load_screen(lv_obj_t *screen)
{
    lv_obj_t *previous = s_screen;
    s_screen = screen;
    lv_screen_load(screen);
    if (previous) lv_obj_delete(previous);
}

static void add_battery(lv_obj_t *parent, int soc)
{
    char text[24];
    if (soc < 0) snprintf(text, sizeof(text), "电量 --");
    else snprintf(text, sizeof(text), "电量 %d%%", soc);

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(COL_MUTED), 0);
    // 右上角要避开屏幕 30px 圆角遮罩，所以下移一截。
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -14, 28);
}

static lv_obj_t *add_text(lv_obj_t *parent, const char *text, const lv_font_t *font,
                          int y, int width, lv_text_align_t align)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_width(label, width);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(label, align, 0);
    if (font) lv_obj_set_style_text_font(label, font, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, y);
    return label;
}

static void show_welcome(const mbti_session_t *session, int soc)
{
    (void)session;
    lv_obj_t *screen = screen_create(COL_BG);
    add_battery(screen, soc);
    add_text(screen, "性格小测", &mbti_font_20, 78, 200, LV_TEXT_ALIGN_CENTER);
    add_text(screen, "16 道小题\n先填年龄和性别", NULL, 118, 200, LV_TEXT_ALIGN_CENTER);
    lv_obj_t *hint = add_text(screen, "按确定开始", NULL, 250, 200, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_color(hint, lv_color_hex(COL_MUTED), 0);
    load_screen(screen);
}

static void show_age(const mbti_session_t *session, int soc)
{
    char number[16];
    snprintf(number, sizeof(number), "%d", session->age);

    lv_obj_t *screen = screen_create(COL_BG);
    add_battery(screen, soc);
    add_text(screen, "你的年龄", &mbti_font_20, 64, 200, LV_TEXT_ALIGN_CENTER);

    lv_obj_t *value = lv_label_create(screen);
    lv_label_set_text(value, number);
    lv_obj_set_style_text_font(value, &lv_font_montserrat_28, 0);
    lv_obj_align(value, LV_ALIGN_TOP_MID, 0, 118);

    lv_obj_t *unit = add_text(screen, "岁", NULL, 162, 80, LV_TEXT_ALIGN_CENTER);
    (void)unit;
    lv_obj_t *hint = add_text(screen, "上键加  下键减\n确定继续  长按返回", NULL, 230, 210,
                              LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_color(hint, lv_color_hex(COL_MUTED), 0);
    load_screen(screen);
}

static void show_gender(const mbti_session_t *session, int soc)
{
    lv_obj_t *screen = screen_create(COL_BG);
    add_battery(screen, soc);
    add_text(screen, "你的性别", &mbti_font_20, 58, 200, LV_TEXT_ALIGN_CENTER);

    for (int i = 0; i < 2; i++) {
        bool picked = session->gender == i;
        lv_obj_t *row = lv_obj_create(screen);
        lv_obj_set_size(row, 180, 52);
        lv_obj_align(row, LV_ALIGN_TOP_MID, 0, 112 + i * 64);
        lv_obj_set_style_radius(row, 14, 0);
        lv_obj_set_style_border_width(row, picked ? 0 : 1, 0);
        lv_obj_set_style_border_color(row, lv_color_hex(COL_LINE), 0);
        lv_obj_set_style_bg_color(row, lv_color_hex(picked ? COL_PICK : COL_PAPER), 0);
        lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
        lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label = lv_label_create(row);
        lv_label_set_text(label, mbti_gender_text(i));
        lv_obj_set_style_text_font(label, &mbti_font_20, 0);
        lv_obj_set_style_text_color(label, lv_color_hex(picked ? COL_PICK_INK : COL_INK), 0);
        lv_obj_center(label);
    }

    lv_obj_t *hint = add_text(screen, "上下切换  确定继续\n长按返回", NULL, 252, 210,
                              LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_color(hint, lv_color_hex(COL_MUTED), 0);
    load_screen(screen);
}

static void show_question(const mbti_session_t *session, int soc)
{
    char progress[32];
    snprintf(progress, sizeof(progress), "%02d / %02d", session->question + 1, MBTI_QUESTION_COUNT);

    lv_obj_t *screen = screen_create(COL_BG);
    add_battery(screen, soc);
    lv_obj_t *step = add_text(screen, progress, NULL, 28, 90, LV_TEXT_ALIGN_LEFT);
    lv_obj_align(step, LV_ALIGN_TOP_LEFT, 16, 28);
    lv_obj_set_style_text_color(step, lv_color_hex(COL_MUTED), 0);

    lv_obj_t *question = add_text(screen, mbti_question_text(session->question), NULL, 52, 208,
                                  LV_TEXT_ALIGN_LEFT);
    lv_obj_align(question, LV_ALIGN_TOP_LEFT, 16, 52);

    for (int i = 0; i < MBTI_CHOICE_COUNT; i++) {
        bool picked = session->choice == i;
        lv_obj_t *row = lv_obj_create(screen);
        lv_obj_set_size(row, 208, 34);
        lv_obj_align(row, LV_ALIGN_TOP_MID, 0, 128 + i * 36);
        lv_obj_set_style_radius(row, 10, 0);
        lv_obj_set_style_pad_left(row, 10, 0);
        lv_obj_set_style_border_width(row, picked ? 0 : 1, 0);
        lv_obj_set_style_border_color(row, lv_color_hex(COL_LINE), 0);
        lv_obj_set_style_bg_color(row, lv_color_hex(picked ? COL_PICK : COL_PAPER), 0);
        lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
        lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label = lv_label_create(row);
        lv_label_set_text(label, mbti_choice_text(i));
        lv_obj_set_style_text_color(label, lv_color_hex(picked ? COL_PICK_INK : COL_INK), 0);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
    }
    load_screen(screen);
}

static void show_summary(const mbti_session_t *session, int soc)
{
    mbti_result_t result;
    lv_obj_t *screen = screen_create(COL_BG);
    add_battery(screen, soc);

    if (!mbti_score(session->answers, &result)) {
        add_text(screen, "结果还没算出来", &mbti_font_20, 120, 200, LV_TEXT_ALIGN_CENTER);
        load_screen(screen);
        return;
    }

    lv_obj_t *code = lv_label_create(screen);
    lv_label_set_text(code, result.code);
    lv_obj_set_style_text_font(code, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(code, lv_color_hex(COL_PICK), 0);
    lv_obj_align(code, LV_ALIGN_TOP_MID, 0, 56);

    add_text(screen, result.title, &mbti_font_20, 98, 200, LV_TEXT_ALIGN_CENTER);

    char meta[32];
    snprintf(meta, sizeof(meta), "%d 岁 · %s", session->age, mbti_gender_text(session->gender));
    lv_obj_t *who = add_text(screen, meta, NULL, 132, 200, LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_color(who, lv_color_hex(COL_MUTED), 0);

    add_text(screen, result.blurb, NULL, 168, 208, LV_TEXT_ALIGN_CENTER);
    lv_obj_t *note = add_text(screen, "轻松参考，不是专业鉴定。", NULL, 230, 208,
                              LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_color(note, lv_color_hex(COL_MUTED), 0);
    lv_obj_t *hint = add_text(screen, "确定看卡片\n长按重新开始", NULL, 262, 200,
                              LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_color(hint, lv_color_hex(COL_MUTED), 0);
    load_screen(screen);
}

static void show_card(const mbti_session_t *session, int soc)
{
    mbti_result_t result;
    const uint8_t *pixels = NULL;
    size_t size = 0;
    bool ready = mbti_score(session->answers, &result) &&
                 mbti_card_pixels(result.code, session->gender, &pixels, &size) &&
                 size == (size_t)MBTI_CARD_W * MBTI_CARD_H * 2;

    lv_obj_t *screen = screen_create(0x000000);
    if (!ready) {
        add_battery(screen, soc);
        lv_obj_t *title = add_text(screen, "这张卡片还没放进来", &mbti_font_20, 120, 200,
                                   LV_TEXT_ALIGN_CENTER);
        lv_obj_set_style_text_color(title, lv_color_hex(0xF3EBDD), 0);
        lv_obj_t *hint = add_text(screen, "确定返回", NULL, 180, 160, LV_TEXT_ALIGN_CENTER);
        lv_obj_set_style_text_color(hint, lv_color_hex(0xF3EBDD), 0);
        load_screen(screen);
        return;
    }

    static lv_image_dsc_t image;
    image.header.magic = LV_IMAGE_HEADER_MAGIC;
    image.header.cf = LV_COLOR_FORMAT_RGB565;
    image.header.flags = 0;
    image.header.w = MBTI_CARD_W;
    image.header.h = MBTI_CARD_H;
    image.header.stride = MBTI_CARD_W * 2;
    image.data_size = (uint32_t)size;
    image.data = pixels;

    lv_obj_t *widget = lv_image_create(screen);
    lv_image_set_src(widget, &image);
    lv_obj_center(widget);
    load_screen(screen);
}

void mbti_ui_show(const mbti_session_t *session, int soc)
{
    switch (session->screen) {
    case MBTI_SCREEN_WELCOME: show_welcome(session, soc); break;
    case MBTI_SCREEN_AGE: show_age(session, soc); break;
    case MBTI_SCREEN_GENDER: show_gender(session, soc); break;
    case MBTI_SCREEN_QUESTION: show_question(session, soc); break;
    case MBTI_SCREEN_SUMMARY: show_summary(session, soc); break;
    case MBTI_SCREEN_CARD: show_card(session, soc); break;
    }
}
