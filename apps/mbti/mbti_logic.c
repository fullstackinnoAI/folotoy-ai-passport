#include "mbti_logic.h"

#include <string.h>

typedef enum {
    AXIS_EI = 0,
    AXIS_SN,
    AXIS_TF,
    AXIS_JP,
} mbti_axis_t;

typedef struct {
    const char *text;
    mbti_axis_t axis;
    bool positive;
} mbti_question_t;

typedef struct {
    const char *code;
    const char *title;
    const char *blurb;
} mbti_type_info_t;

// 同意正向题会推向 E/S/T/J；positive 为 false 的题反过来计。
static const mbti_question_t QUESTIONS[MBTI_QUESTION_COUNT] = {
    {"在社交聚会中，我通常主动与他人交谈并认识新朋友", AXIS_EI, true},
    {"参加派对或社交活动后，我感觉精力充沛", AXIS_EI, true},
    {"我更喜欢独自一人思考或阅读，而不是参加社交活动", AXIS_EI, false},
    {"在一整天与人互动后，我需要独处时间来恢复精力", AXIS_EI, false},
    {"我更关注具体的事实和细节，而不是抽象的概念", AXIS_SN, true},
    {"我喜欢处理实际、具体的问题，而不是理论性问题", AXIS_SN, true},
    {"我经常思考未来的可能性和潜在的发展", AXIS_SN, false},
    {"我喜欢探索新想法和概念，即使它们没有实际应用", AXIS_SN, false},
    {"做决定时，我更多基于逻辑分析而非个人感受", AXIS_TF, true},
    {"我认为公平和客观比同情和和谐更重要", AXIS_TF, true},
    {"在做决定时，我会考虑他人的感受和人际关系", AXIS_TF, false},
    {"我倾向于根据价值观和个人信念做决定", AXIS_TF, false},
    {"我喜欢提前计划并按计划行事，而不是随性而为", AXIS_JP, true},
    {"我更喜欢有组织、有结构的工作环境", AXIS_JP, true},
    {"我喜欢保持选择的开放性，而不是过早做决定", AXIS_JP, false},
    {"我能够灵活应对变化和意外情况", AXIS_JP, false},
};

static const char *CHOICES[MBTI_CHOICE_COUNT] = {
    "非常不赞同",
    "比较不赞同",
    "一般",
    "比较赞同",
    "非常赞同",
};

static const mbti_type_info_t TYPES[MBTI_TYPE_COUNT] = {
    {"ISTJ", "物流师", "靠谱务实，喜欢把事情按计划做完。"},
    {"ISFJ", "守卫者", "细心顾人，会把身边人照顾得很稳。"},
    {"INFJ", "提倡者", "想得深，也在意这件事对别人的意义。"},
    {"INTJ", "建筑师", "目标清楚，习惯先想通再动手。"},
    {"ISTP", "鉴赏家", "动手快，遇到问题先拆开看。"},
    {"ISFP", "探险家", "在意感受和当下，做事有自己的节奏。"},
    {"INFP", "调停者", "心里有价值观，希望事情和它一致。"},
    {"INTP", "逻辑学家", "爱琢磨原理，答案要先说得通。"},
    {"ESTP", "企业家", "行动派，现场反应快，喜欢直接试。"},
    {"ESFP", "表演者", "在人群里来劲，把当下过得热闹。"},
    {"ENFP", "竞选者", "点子多，人和新可能都会让你兴奋。"},
    {"ENTP", "辩论家", "喜欢变通和交锋，越有挑战越有劲。"},
    {"ESTJ", "总经理", "做事利落，习惯把人和事安排清楚。"},
    {"ESFJ", "执政官", "在意气氛和别人的需要，愿意张罗。"},
    {"ENFJ", "主人公", "会带动别人，也看重一起往前走。"},
    {"ENTJ", "指挥官", "敢拍板，喜欢把目标推进成结果。"},
};

static const char AXIS_LETTERS[4][2] = {
    {'E', 'I'},
    {'S', 'N'},
    {'T', 'F'},
    {'J', 'P'},
};

void mbti_session_init(mbti_session_t *session)
{
    session->screen = MBTI_SCREEN_WELCOME;
    session->age = 18;
    session->gender = 0;
    session->question = 0;
    session->choice = 2;
    for (int i = 0; i < MBTI_QUESTION_COUNT; i++) {
        session->answers[i] = -1;
    }
}

static int clamp_age(int age)
{
    if (age < MBTI_AGE_MIN) return MBTI_AGE_MIN;
    if (age > MBTI_AGE_MAX) return MBTI_AGE_MAX;
    return age;
}

static void enter_question(mbti_session_t *session, int index)
{
    session->screen = MBTI_SCREEN_QUESTION;
    session->question = index;
    session->choice = session->answers[index] >= 0 ? session->answers[index] : 2;
}

void mbti_session_apply(mbti_session_t *session, mbti_event_t event)
{
    switch (session->screen) {
    case MBTI_SCREEN_WELCOME:
        if (event == MBTI_EVT_OK) session->screen = MBTI_SCREEN_AGE;
        break;
    case MBTI_SCREEN_AGE:
        if (event == MBTI_EVT_UP) session->age = clamp_age(session->age + 1);
        else if (event == MBTI_EVT_DOWN) session->age = clamp_age(session->age - 1);
        else if (event == MBTI_EVT_OK) session->screen = MBTI_SCREEN_GENDER;
        else if (event == MBTI_EVT_OK_LONG) session->screen = MBTI_SCREEN_WELCOME;
        break;
    case MBTI_SCREEN_GENDER:
        if (event == MBTI_EVT_UP || event == MBTI_EVT_DOWN) session->gender ^= 1;
        else if (event == MBTI_EVT_OK) enter_question(session, 0);
        else if (event == MBTI_EVT_OK_LONG) session->screen = MBTI_SCREEN_AGE;
        break;
    case MBTI_SCREEN_QUESTION:
        if (event == MBTI_EVT_UP) {
            if (session->choice > 0) session->choice--;
        } else if (event == MBTI_EVT_DOWN) {
            if (session->choice + 1 < MBTI_CHOICE_COUNT) session->choice++;
        } else if (event == MBTI_EVT_OK) {
            session->answers[session->question] = session->choice;
            if (session->question + 1 >= MBTI_QUESTION_COUNT) {
                session->screen = MBTI_SCREEN_SUMMARY;
            } else {
                enter_question(session, session->question + 1);
            }
        } else if (event == MBTI_EVT_OK_LONG) {
            if (session->question == 0) session->screen = MBTI_SCREEN_GENDER;
            else enter_question(session, session->question - 1);
        }
        break;
    case MBTI_SCREEN_SUMMARY:
        if (event == MBTI_EVT_OK) session->screen = MBTI_SCREEN_CARD;
        else if (event == MBTI_EVT_OK_LONG) mbti_session_init(session);
        break;
    case MBTI_SCREEN_CARD:
        if (event == MBTI_EVT_OK) session->screen = MBTI_SCREEN_SUMMARY;
        else if (event == MBTI_EVT_OK_LONG) mbti_session_init(session);
        break;
    }
}

const char *mbti_question_text(int index)
{
    if (index < 0 || index >= MBTI_QUESTION_COUNT) return "";
    return QUESTIONS[index].text;
}

const char *mbti_choice_text(int choice)
{
    if (choice < 0 || choice >= MBTI_CHOICE_COUNT) return "";
    return CHOICES[choice];
}

const char *mbti_gender_text(int gender)
{
    return gender == 0 ? "女" : "男";
}

bool mbti_score(const int answers[MBTI_QUESTION_COUNT], mbti_result_t *result)
{
    int totals[4] = {0, 0, 0, 0};
    for (int i = 0; i < MBTI_QUESTION_COUNT; i++) {
        int answer = answers[i];
        if (answer < 0 || answer >= MBTI_CHOICE_COUNT) return false;
        int value = answer + 1;
        int contribution = QUESTIONS[i].positive ? value : (6 - value);
        totals[QUESTIONS[i].axis] += contribution;
    }

    char code[5];
    for (int axis = 0; axis < 4; axis++) {
        code[axis] = AXIS_LETTERS[axis][totals[axis] > 12 ? 0 : 1];
    }
    code[4] = '\0';

    for (int i = 0; i < MBTI_TYPE_COUNT; i++) {
        if (strcmp(TYPES[i].code, code) == 0) {
            memcpy(result->code, code, sizeof(result->code));
            result->title = TYPES[i].title;
            result->blurb = TYPES[i].blurb;
            return true;
        }
    }
    return false;
}
