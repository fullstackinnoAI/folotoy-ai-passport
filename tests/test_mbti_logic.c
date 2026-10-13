#include <assert.h>
#include <string.h>
#include "mbti_logic.h"

static void answer_all(int answers[MBTI_QUESTION_COUNT], int value)
{
    for (int i = 0; i < MBTI_QUESTION_COUNT; i++) answers[i] = value;
}

static void assert_code(const int answers[MBTI_QUESTION_COUNT], const char *code)
{
    mbti_result_t result;
    assert(mbti_score(answers, &result));
    assert(strcmp(result.code, code) == 0);
    assert(result.title != NULL && result.title[0] != '\0');
    assert(result.blurb != NULL && result.blurb[0] != '\0');
}

int main(void)
{
    assert(strcmp(mbti_question_text(0),
                  "在社交聚会中，我通常主动与他人交谈并认识新朋友") == 0);
    assert(strcmp(mbti_question_text(15),
                  "我能够灵活应对变化和意外情况") == 0);
    assert(strcmp(mbti_choice_text(0), "非常不赞同") == 0);
    assert(strcmp(mbti_choice_text(4), "非常赞同") == 0);

    int answers[MBTI_QUESTION_COUNT];
    answer_all(answers, 4);
    assert_code(answers, "INFP");
    answer_all(answers, 0);
    assert_code(answers, "INFP");

    // 每维前两题强烈同意、后两题强烈反对，四极都取前一极。
    for (int axis = 0; axis < 4; axis++) {
        answers[axis * 4] = 4;
        answers[axis * 4 + 1] = 4;
        answers[axis * 4 + 2] = 0;
        answers[axis * 4 + 3] = 0;
    }
    assert_code(answers, "ESTJ");

    // 只把外向两极拉满，其余保持中点，得到 ENFP。
    answer_all(answers, 2);
    answers[0] = 4;
    answers[1] = 4;
    answers[2] = 0;
    answers[3] = 0;
    assert_code(answers, "ENFP");

    answers[0] = 5;
    mbti_result_t invalid;
    assert(!mbti_score(answers, &invalid));

    mbti_session_t session;
    mbti_session_init(&session);
    assert(session.screen == MBTI_SCREEN_WELCOME);
    assert(session.age == 18);
    assert(session.gender == 0);

    mbti_session_apply(&session, MBTI_EVT_UP);
    assert(session.screen == MBTI_SCREEN_WELCOME);
    mbti_session_apply(&session, MBTI_EVT_OK);
    assert(session.screen == MBTI_SCREEN_AGE);

    mbti_session_apply(&session, MBTI_EVT_UP);
    assert(session.age == 19);
    mbti_session_apply(&session, MBTI_EVT_DOWN);
    mbti_session_apply(&session, MBTI_EVT_DOWN);
    assert(session.age == 17);
    session.age = MBTI_AGE_MAX;
    mbti_session_apply(&session, MBTI_EVT_UP);
    assert(session.age == MBTI_AGE_MAX);
    session.age = MBTI_AGE_MIN;
    mbti_session_apply(&session, MBTI_EVT_DOWN);
    assert(session.age == MBTI_AGE_MIN);

    mbti_session_apply(&session, MBTI_EVT_OK);
    assert(session.screen == MBTI_SCREEN_GENDER);
    assert(session.gender == 0);
    mbti_session_apply(&session, MBTI_EVT_DOWN);
    assert(session.gender == 1);
    mbti_session_apply(&session, MBTI_EVT_OK);
    assert(session.screen == MBTI_SCREEN_QUESTION);
    assert(session.question == 0);
    assert(session.choice == 2);

    mbti_session_apply(&session, MBTI_EVT_UP);
    mbti_session_apply(&session, MBTI_EVT_UP);
    mbti_session_apply(&session, MBTI_EVT_UP);
    assert(session.choice == 0);
    mbti_session_apply(&session, MBTI_EVT_OK_LONG);
    assert(session.screen == MBTI_SCREEN_GENDER);

    mbti_session_apply(&session, MBTI_EVT_OK);
    for (int i = 0; i < MBTI_QUESTION_COUNT; i++) {
        assert(session.screen == MBTI_SCREEN_QUESTION);
        assert(session.question == i);
        mbti_session_apply(&session, MBTI_EVT_OK);
    }
    assert(session.screen == MBTI_SCREEN_SUMMARY);
    for (int i = 0; i < MBTI_QUESTION_COUNT; i++) assert(session.answers[i] == 2);

    mbti_session_apply(&session, MBTI_EVT_OK);
    assert(session.screen == MBTI_SCREEN_CARD);
    mbti_session_apply(&session, MBTI_EVT_OK);
    assert(session.screen == MBTI_SCREEN_SUMMARY);
    mbti_session_apply(&session, MBTI_EVT_OK_LONG);
    assert(session.screen == MBTI_SCREEN_WELCOME);
    assert(session.age == 18);
    return 0;
}
