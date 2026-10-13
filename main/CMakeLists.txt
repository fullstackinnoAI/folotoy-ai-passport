include(${CMAKE_CURRENT_LIST_DIR}/../apps/mbti/mbti_cards.cmake)

idf_component_register(
    SRCS "../apps/mbti/main.c"
         "../apps/mbti/mbti_logic.c"
         "../apps/mbti/mbti_ui.c"
         "../apps/mbti/mbti_cards.c"
         "../assets/fonts/mbti_font_16.c"
         "../assets/fonts/mbti_font_20.c"
    INCLUDE_DIRS "." "../apps/mbti"
    REQUIRES bsp esp_hw_support esp_timer
    EMBED_FILES ${MBTI_CARD_BINS}
)
