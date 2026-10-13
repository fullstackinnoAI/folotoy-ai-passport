#!/usr/bin/env python3
"""把本地 MBTI 卡片缩成 240x320 RGB565，并写出固件嵌入清单。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
SOURCE = Path("/Volumes/MediaDesign/AIGC素材/MBTI")
OUT_DIR = ROOT / "assets" / "images" / "mbti"
CMAKE = ROOT / "apps" / "mbti" / "mbti_cards.cmake"
HEADER = ROOT / "apps" / "mbti" / "mbti_cards.h"
WIDTH = 240
HEIGHT = 320

NAME = re.compile(r"^\d+-(female|male)-([IESNTFJP]{4})\.png$")


def rgb565(red: int, green: int, blue: int) -> int:
    return ((red * 31 + 127) // 255 << 11) | ((green * 63 + 127) // 255 << 5) | (
        (blue * 31 + 127) // 255
    )


def convert(path: Path, dest: Path) -> None:
    image = Image.open(path).convert("RGB").resize((WIDTH, HEIGHT), Image.Resampling.LANCZOS)
    raw = bytearray()
    for red, green, blue in image.getdata():
        pixel = rgb565(red, green, blue)
        raw.append(pixel & 0xFF)
        raw.append(pixel >> 8)
    dest.write_bytes(raw)


def main() -> int:
    if not SOURCE.is_dir():
        print(f"ERROR: card folder not found: {SOURCE}", file=sys.stderr)
        return 1

    chosen: dict[tuple[str, str], Path] = {}
    skipped: list[str] = []
    for path in sorted(SOURCE.iterdir()):
        if path.name.startswith("._") or path.suffix.lower() != ".png":
            continue
        match = NAME.match(path.name)
        if not match:
            skipped.append(path.name)
            continue
        key = (match.group(1), match.group(2))
        if key in chosen:
            skipped.append(path.name)
            continue
        chosen[key] = path

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    bins: list[str] = []
    spans: list[tuple[str, str, str]] = []
    for (gender, code), path in sorted(chosen.items()):
        stem = f"{'f' if gender == 'female' else 'm'}_{code.lower()}"
        dest = OUT_DIR / f"{stem}.rgb565"
        convert(path, dest)
        bins.append(stem)
        spans.append((code, gender, stem))
        print(f"{path.name} -> {dest.relative_to(ROOT)}")

    if skipped:
        print("skipped:", ", ".join(skipped))

    cmake_lines = ["set(MBTI_CARD_BINS"]
    for stem in bins:
        cmake_lines.append(f'    "${{CMAKE_CURRENT_LIST_DIR}}/../../assets/images/mbti/{stem}.rgb565"')
    cmake_lines.append(")")
    CMAKE.write_text("\n".join(cmake_lines) + "\n", encoding="utf-8")

    by_code: dict[str, dict[str, str]] = {}
    for code, gender, stem in spans:
        by_code.setdefault(code, {})[gender] = stem

    header = [
        "// 由 tools/prepare_mbti_assets.py 生成。源图在本机 MBTI 卡片目录。",
        "#pragma once",
        "",
        "#include <stdbool.h>",
        "#include <stddef.h>",
        "#include <stdint.h>",
        "",
        "#define MBTI_CARD_W 240",
        "#define MBTI_CARD_H 320",
        "",
        "bool mbti_card_pixels(const char *code, int gender, const uint8_t **pixels, size_t *size);",
        "",
    ]
    HEADER.write_text("\n".join(header), encoding="utf-8")

    source = [
        "// 由 tools/prepare_mbti_assets.py 生成。",
        '#include "mbti_cards.h"',
        "",
        "#include <string.h>",
        "",
    ]
    for stem in bins:
        source.append(f"extern const uint8_t {stem}_start[] asm(\"_binary_{stem}_rgb565_start\");")
        source.append(f"extern const uint8_t {stem}_end[] asm(\"_binary_{stem}_rgb565_end\");")
    source += [
        "",
        "typedef struct {",
        "    const char *code;",
        "    const uint8_t *female;",
        "    const uint8_t *female_end;",
        "    const uint8_t *male;",
        "    const uint8_t *male_end;",
        "} mbti_card_entry_t;",
        "",
        "static const mbti_card_entry_t CARDS[] = {",
    ]
    for code in sorted(by_code):
        female = by_code[code].get("female")
        male = by_code[code].get("male")
        female_start = "NULL" if female is None else f"{female}_start"
        female_end = "NULL" if female is None else f"{female}_end"
        male_start = "NULL" if male is None else f"{male}_start"
        male_end = "NULL" if male is None else f"{male}_end"
        source.append(
            f'    {{"{code}", {female_start}, {female_end}, {male_start}, {male_end}}},'
        )
    source += [
        "};",
        "",
        "bool mbti_card_pixels(const char *code, int gender, const uint8_t **pixels, size_t *size)",
        "{",
        "    for (size_t i = 0; i < sizeof(CARDS) / sizeof(CARDS[0]); i++) {",
        "        if (strcmp(CARDS[i].code, code) != 0) continue;",
        "        const uint8_t *start = gender == 0 ? CARDS[i].female : CARDS[i].male;",
        "        const uint8_t *end = gender == 0 ? CARDS[i].female_end : CARDS[i].male_end;",
        "        if (!start || !end || end < start) return false;",
        "        *pixels = start;",
        "        *size = (size_t)(end - start);",
        "        return true;",
        "    }",
        "    return false;",
        "}",
        "",
    ]
    (ROOT / "apps" / "mbti" / "mbti_cards.c").write_text("\n".join(source), encoding="utf-8")
    print(f"cards: {len(bins)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
