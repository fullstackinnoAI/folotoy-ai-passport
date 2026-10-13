// 由 tools/prepare_mbti_assets.py 生成。
#include "mbti_cards.h"

#include <string.h>

extern const uint8_t f_enfj_start[] asm("_binary_f_enfj_rgb565_start");
extern const uint8_t f_enfj_end[] asm("_binary_f_enfj_rgb565_end");
extern const uint8_t f_enfp_start[] asm("_binary_f_enfp_rgb565_start");
extern const uint8_t f_enfp_end[] asm("_binary_f_enfp_rgb565_end");
extern const uint8_t f_entj_start[] asm("_binary_f_entj_rgb565_start");
extern const uint8_t f_entj_end[] asm("_binary_f_entj_rgb565_end");
extern const uint8_t f_esfj_start[] asm("_binary_f_esfj_rgb565_start");
extern const uint8_t f_esfj_end[] asm("_binary_f_esfj_rgb565_end");
extern const uint8_t f_esfp_start[] asm("_binary_f_esfp_rgb565_start");
extern const uint8_t f_esfp_end[] asm("_binary_f_esfp_rgb565_end");
extern const uint8_t f_estj_start[] asm("_binary_f_estj_rgb565_start");
extern const uint8_t f_estj_end[] asm("_binary_f_estj_rgb565_end");
extern const uint8_t f_estp_start[] asm("_binary_f_estp_rgb565_start");
extern const uint8_t f_estp_end[] asm("_binary_f_estp_rgb565_end");
extern const uint8_t f_infj_start[] asm("_binary_f_infj_rgb565_start");
extern const uint8_t f_infj_end[] asm("_binary_f_infj_rgb565_end");
extern const uint8_t f_infp_start[] asm("_binary_f_infp_rgb565_start");
extern const uint8_t f_infp_end[] asm("_binary_f_infp_rgb565_end");
extern const uint8_t f_intj_start[] asm("_binary_f_intj_rgb565_start");
extern const uint8_t f_intj_end[] asm("_binary_f_intj_rgb565_end");
extern const uint8_t f_intp_start[] asm("_binary_f_intp_rgb565_start");
extern const uint8_t f_intp_end[] asm("_binary_f_intp_rgb565_end");
extern const uint8_t f_isfj_start[] asm("_binary_f_isfj_rgb565_start");
extern const uint8_t f_isfj_end[] asm("_binary_f_isfj_rgb565_end");
extern const uint8_t f_isfp_start[] asm("_binary_f_isfp_rgb565_start");
extern const uint8_t f_isfp_end[] asm("_binary_f_isfp_rgb565_end");
extern const uint8_t f_istj_start[] asm("_binary_f_istj_rgb565_start");
extern const uint8_t f_istj_end[] asm("_binary_f_istj_rgb565_end");
extern const uint8_t f_istp_start[] asm("_binary_f_istp_rgb565_start");
extern const uint8_t f_istp_end[] asm("_binary_f_istp_rgb565_end");
extern const uint8_t m_enfj_start[] asm("_binary_m_enfj_rgb565_start");
extern const uint8_t m_enfj_end[] asm("_binary_m_enfj_rgb565_end");
extern const uint8_t m_enfp_start[] asm("_binary_m_enfp_rgb565_start");
extern const uint8_t m_enfp_end[] asm("_binary_m_enfp_rgb565_end");
extern const uint8_t m_entj_start[] asm("_binary_m_entj_rgb565_start");
extern const uint8_t m_entj_end[] asm("_binary_m_entj_rgb565_end");
extern const uint8_t m_entp_start[] asm("_binary_m_entp_rgb565_start");
extern const uint8_t m_entp_end[] asm("_binary_m_entp_rgb565_end");
extern const uint8_t m_esfj_start[] asm("_binary_m_esfj_rgb565_start");
extern const uint8_t m_esfj_end[] asm("_binary_m_esfj_rgb565_end");
extern const uint8_t m_esfp_start[] asm("_binary_m_esfp_rgb565_start");
extern const uint8_t m_esfp_end[] asm("_binary_m_esfp_rgb565_end");
extern const uint8_t m_estj_start[] asm("_binary_m_estj_rgb565_start");
extern const uint8_t m_estj_end[] asm("_binary_m_estj_rgb565_end");
extern const uint8_t m_estp_start[] asm("_binary_m_estp_rgb565_start");
extern const uint8_t m_estp_end[] asm("_binary_m_estp_rgb565_end");
extern const uint8_t m_infj_start[] asm("_binary_m_infj_rgb565_start");
extern const uint8_t m_infj_end[] asm("_binary_m_infj_rgb565_end");
extern const uint8_t m_infp_start[] asm("_binary_m_infp_rgb565_start");
extern const uint8_t m_infp_end[] asm("_binary_m_infp_rgb565_end");
extern const uint8_t m_intj_start[] asm("_binary_m_intj_rgb565_start");
extern const uint8_t m_intj_end[] asm("_binary_m_intj_rgb565_end");
extern const uint8_t m_intp_start[] asm("_binary_m_intp_rgb565_start");
extern const uint8_t m_intp_end[] asm("_binary_m_intp_rgb565_end");
extern const uint8_t m_isfj_start[] asm("_binary_m_isfj_rgb565_start");
extern const uint8_t m_isfj_end[] asm("_binary_m_isfj_rgb565_end");
extern const uint8_t m_isfp_start[] asm("_binary_m_isfp_rgb565_start");
extern const uint8_t m_isfp_end[] asm("_binary_m_isfp_rgb565_end");
extern const uint8_t m_istj_start[] asm("_binary_m_istj_rgb565_start");
extern const uint8_t m_istj_end[] asm("_binary_m_istj_rgb565_end");
extern const uint8_t m_istp_start[] asm("_binary_m_istp_rgb565_start");
extern const uint8_t m_istp_end[] asm("_binary_m_istp_rgb565_end");

typedef struct {
    const char *code;
    const uint8_t *female;
    const uint8_t *female_end;
    const uint8_t *male;
    const uint8_t *male_end;
} mbti_card_entry_t;

static const mbti_card_entry_t CARDS[] = {
    {"ENFJ", f_enfj_start, f_enfj_end, m_enfj_start, m_enfj_end},
    {"ENFP", f_enfp_start, f_enfp_end, m_enfp_start, m_enfp_end},
    {"ENTJ", f_entj_start, f_entj_end, m_entj_start, m_entj_end},
    {"ENTP", NULL, NULL, m_entp_start, m_entp_end},
    {"ESFJ", f_esfj_start, f_esfj_end, m_esfj_start, m_esfj_end},
    {"ESFP", f_esfp_start, f_esfp_end, m_esfp_start, m_esfp_end},
    {"ESTJ", f_estj_start, f_estj_end, m_estj_start, m_estj_end},
    {"ESTP", f_estp_start, f_estp_end, m_estp_start, m_estp_end},
    {"INFJ", f_infj_start, f_infj_end, m_infj_start, m_infj_end},
    {"INFP", f_infp_start, f_infp_end, m_infp_start, m_infp_end},
    {"INTJ", f_intj_start, f_intj_end, m_intj_start, m_intj_end},
    {"INTP", f_intp_start, f_intp_end, m_intp_start, m_intp_end},
    {"ISFJ", f_isfj_start, f_isfj_end, m_isfj_start, m_isfj_end},
    {"ISFP", f_isfp_start, f_isfp_end, m_isfp_start, m_isfp_end},
    {"ISTJ", f_istj_start, f_istj_end, m_istj_start, m_istj_end},
    {"ISTP", f_istp_start, f_istp_end, m_istp_start, m_istp_end},
};

bool mbti_card_pixels(const char *code, int gender, const uint8_t **pixels, size_t *size)
{
    for (size_t i = 0; i < sizeof(CARDS) / sizeof(CARDS[0]); i++) {
        if (strcmp(CARDS[i].code, code) != 0) continue;
        const uint8_t *start = gender == 0 ? CARDS[i].female : CARDS[i].male;
        const uint8_t *end = gender == 0 ? CARDS[i].female_end : CARDS[i].male_end;
        if (!start || !end || end < start) return false;
        *pixels = start;
        *size = (size_t)(end - start);
        return true;
    }
    return false;
}
