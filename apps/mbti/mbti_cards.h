// 由 tools/prepare_mbti_assets.py 生成。源图在本机 MBTI 卡片目录。
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MBTI_CARD_W 240
#define MBTI_CARD_H 320

bool mbti_card_pixels(const char *code, int gender, const uint8_t **pixels, size_t *size);
