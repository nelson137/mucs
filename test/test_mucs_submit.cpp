#include "test_mucs_submit.hpp"


TEST_CASE("submit throws when user not in course", "[mucs][submit]") {
    Mucs mucs;
    mucs.user = rand_user();
    mucs.config = Config();
    REQUIRE_THROWS_WITH(
        mucs.submit(),
        "Student not in course: " + mucs.user
    );
}


TEST_CASE("submit throws when the submission window is closed",
          "[mucs][submit]") {
    string user = rand_user();
    string hw_name = rand_hw_name();
    string lab_id = rand_lab_sesh_id();

    Mucs mucs;
    mucs.user = user;
    mucs.asgmt_name = hw_name;
    mucs.config.course_id = rand_course();
    mucs.config.roster.safe_insert(user, lab_id);
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh().today().now().get()
    });

    sys_seconds dd{};
    mucs.config.homeworks.insert({ hw_name, dd });
    REQUIRE_THROWS_WITH(
        mucs.submit(),
        "Submission window is closed for assignment: " + mucs.asgmt_name
    );
}


TEST_CASE("submit throws when a given source path", "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    string lab_name = rand_lab_asgmt_name();

    Mucs mucs;
    mucs.user = user;
    mucs.asgmt_name = lab_name;
    mucs.config.course_id = rand_course();
    mucs.config.roster.safe_insert(user, lab_id);
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh().today().now().get()
    });
    mucs.config.lab_assignments.insert(
        RandLabAsgmt(lab_name).this_week(true).get());

    Mock<Mucs> spy(mucs);
    When(Method(spy, prompt_yesno)).AlwaysReturn(true);
    Fake(
        Method(spy, submit_summary),
        Method(spy, copy_submission_files)
    );

    SECTION("doesn't exist") {
        string src_fn = rand_filename();
        mucs.sources = { Path(src_fn) };
        When(Method(spy, compile_sources)).AlwaysReturn(true);
        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            "Source file does not exist: " + src_fn
        );
    }

    SECTION("is a directory") {
        string src_fn = "/home";
        mucs.sources = { Path(src_fn) };
        When(Method(spy, compile_sources)).AlwaysReturn(true);
        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            "Cannot submit directories: /home"
        );
    }

    SECTION("doesn't compile") {
        mucs.sources = { Path("/usr/bin/sudo") };
        When(Method(spy, compile_sources)).AlwaysReturn(false);
        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            "Program doesn't compile"
        );
    }
}


TEST_CASE("submit for homework succeeds while the lab session",
          "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    string hw_name = rand_hw_name();
    sys_seconds dd{};
    dd += years((int) get_day().year() - 1970 + 3);

    Mucs mucs;
    mucs.user = user;
    mucs.asgmt_name = hw_name;
    mucs.config.course_id = rand_course();
    mucs.config.homeworks.insert({ hw_name, dd });
    mucs.config.roster.safe_insert(user, lab_id);

    auto finish_test_from_section = [&] (const string& section) {
        Mock<Mucs> spy(mucs);
        When(Method(spy, prompt_yesno)).AlwaysReturn(true);
        When(Method(spy, compile_sources)).AlwaysReturn(true);
        Fake(
            Method(spy, submit_summary),
            Method(spy, copy_submission_files)
        );

        INFO("\rFROM SECTION " + section + ":");

        REQUIRE_NOTHROW(spy.get().submit());

        Verify(
            Method(spy, compile_sources)
            + Method(spy, submit_summary)
            + Method(spy, prompt_yesno)
            + Method(spy, copy_submission_files)
        ).Once();
        VerifyNoOtherInvocations(spy);
    };

    SECTION("is active") {
        mucs.config.lab_sessions.insert({
            lab_id, RandLabSesh().today().now().get()
        });
        finish_test_from_section("1");
    }

    SECTION("is not active") {
        mucs.config.lab_sessions.insert({
            lab_id, RandLabSesh().today(false).now().get()
        });
        finish_test_from_section("2");
    }
}


TEST_CASE("submit for lab assignment", "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    string lab_name = rand_lab_asgmt_name();

    Mucs mucs;
    mucs.user = user;
    mucs.asgmt_name = lab_name;
    mucs.config.course_id = rand_course();
    mucs.config.lab_assignments.insert(
        RandLabAsgmt(lab_name).this_week().get());
    mucs.config.roster.safe_insert(user, lab_id);

    Mock<Mucs> spy(mucs);
    When(Method(spy, prompt_yesno)).AlwaysReturn(true);
    When(Method(spy, compile_sources)).AlwaysReturn(true);
    Fake(
        Method(spy, submit_summary),
        Method(spy, copy_submission_files)
    );

    SECTION("succeeds when the lab session is active") {
        mucs.config.lab_sessions.insert({
            lab_id, RandLabSesh().today().now().get()
        });

        REQUIRE_NOTHROW(spy.get().submit());

        Verify(
            Method(spy, compile_sources)
            + Method(spy, submit_summary)
            + Method(spy, prompt_yesno)
            + Method(spy, copy_submission_files)
        ).Once();
        VerifyNoOtherInvocations(spy);
    }

    SECTION("throws when the lab session is not active") {
        LabSesh ls = RandLabSesh().today(false).now().get();
        mucs.config.lab_sessions.insert({ lab_id, ls });

        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            ls.format(
                "Lab {id} is not in session: {weekday} from {start} to {end}")
        );
    }
}


TEST_CASE("submission cancelled by user", "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    string lab_name = rand_lab_asgmt_name();

    Mucs mucs;
    mucs.user = user;
    mucs.asgmt_name = lab_name;
    mucs.config.roster.safe_insert(user, lab_id);
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh(lab_id).today().now().get() });
    mucs.config.lab_assignments.insert(
        RandLabAsgmt(lab_name).this_week().get());

    Mock<Mucs> spy(mucs);
    When(Method(spy, prompt_yesno)).AlwaysReturn(false);
    When(Method(spy, compile_sources)).AlwaysReturn(true);
    Fake(
        Method(spy, submit_summary),
        Method(spy, copy_submission_files)
    );

    REQUIRE_THROWS_WITH(spy.get().submit(), "Submission cancelled");

    Verify(
        Method(spy, compile_sources)
        + Method(spy, submit_summary)
        + Method(spy, prompt_yesno)
    ).Once();
    VerifyNoOtherInvocations(spy);
}

TEST_CASE("double submit homework too quickly", "[mucs][submit]") {
    NOW = sys_seconds{};
    string user = "user1";
    string lab_id = "X";
    string hw_name = "hw1";
    sys_seconds dd{};
    dd += years((int) get_day().year() - 1970 + 3);

    Mucs mucs;
    mucs.user = user;
    mucs.asgmt_name = hw_name;
    mucs.course = mucs.config.course_id = "1050";
    mucs.config.roster.safe_insert(user, lab_id);
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh(lab_id).today().now().get()
    });
    mucs.config.homeworks.insert({ hw_name, dd });

    Mock<Mucs> spy(mucs);
    When(Method(spy, prompt_yesno)).AlwaysReturn(true);
    When(Method(spy, compile_sources)).AlwaysReturn(true);
    Fake(
        Method(spy, submit_summary),
        Method(spy, copy_submission_files)
    );

    REQUIRE_THROWS_WITH(
        spy.get().submit(),
        "Attempted successive submissions too quickly, please try again"
    );

    Verify(
        Method(spy, compile_sources)
        + Method(spy, submit_summary)
        + Method(spy, prompt_yesno)
    ).Once();
    VerifyNoOtherInvocations(spy);
}
