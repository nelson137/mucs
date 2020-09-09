#include "test_mucs_submit.hpp"


TEST_CASE("submit fails when user not in course", "[mucs][submit]") {
    Mucs mucs;
    mucs.user = rand_user();
    mucs.config = Config();
    REQUIRE_THROWS_WITH(
        mucs.submit(),
        "User not in course: " + mucs.user
    );
}


TEST_CASE("submission fails when the submission window is closed",
          "[mucs][submit]") {
    string user = rand_user();
    string hw_name = rand_hw_name();
    string lab_id = rand_lab_sesh_id();

    Mucs mucs;
    mucs.user = user;
    mucs.assignment = hw_name;
    mucs.config.course_id = rand_course();
    mucs.config.roster.insert({ user, {lab_id} });
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh().today().now().get()
    });

    sys_seconds dd{};
    mucs.config.homeworks.insert({ hw_name, dd });
    REQUIRE_THROWS_WITH(
        mucs.submit(),
        "Submission window is closed for assignment: " + mucs.assignment
    );
}


TEST_CASE("submission succeeds", "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();

    Mucs mucs;
    mucs.user = user;
    mucs.config.course_id = rand_course();
    mucs.config.roster.insert({ user, {lab_id} });
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh().today().now().get()
    });

    SECTION("for homeworks") {
        string hw_name = rand_hw_name();
        sys_seconds dd{};
        dd += years((int) get_day().year() - 1970 + 3);
        mucs.config.homeworks.insert({ hw_name, dd });
        mucs.assignment = hw_name;
    }

    SECTION("for lab assignments") {
        string lab_name = rand_lab_asgmt_name();
        mucs.config.lab_assignments.insert(
            RandLabAsgmt(lab_name).this_week().get());
        mucs.assignment = lab_name;
    }

    Mock<Mucs> spy(mucs);
    When(Method(spy, try_compile_sources)).Return(true);
    Fake(Method(spy, submit_summary));
    When(Method(spy, prompt_yesno)).Return(true);
    Fake(Method(spy, copy_submission_files));

    spy.get().submit();
    Verify(Method(spy, copy_submission_files));
}


TEST_CASE("attempt to submit non-compiling sources", "[mucs][submit]") {
    // TODO
}


TEST_CASE("submission cancelled by user", "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    string lab_name = rand_lab_asgmt_name();

    Mucs mucs;
    mucs.user = user;
    mucs.assignment = lab_name;
    mucs.config.roster.insert({ user, {lab_id} });
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh(lab_id).today().now().get() });
    mucs.config.lab_assignments.insert(
        RandLabAsgmt(lab_name).this_week().get());

    Mock<Mucs> spy(mucs);
    When(Method(spy, try_compile_sources)).Return(true);
    Fake(Method(spy, submit_summary));
    When(Method(spy, prompt_yesno)).Return(false);

    REQUIRE_THROWS_WITH(spy.get().submit(), "Submission cancelled");
}


TEST_CASE("double submit homework too quickly", "[mucs][submit]") {
    // TODO
}
