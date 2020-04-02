#include "test_submit.hpp"


TEST_CASE("submit user not in course", "[mucs][submit]") {
    Mucs mucs;
    mucs.user = rand_user();
    mucs.config = Config();
    REQUIRE_THROWS_WITH(
        mucs.submit(),
        "User not in course: " + mucs.user
    );
}


TEST_CASE("submit lab with one lab session", "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_id();

    Mucs mucs;
    mucs.user = user;
    mucs.config.roster.insert({ user, {lab_id} });

    SECTION("that is not in session") {
        // TODO: why can't lab be a const&
        LabSesh lab = RandLabSesh().today().now(false).get();
        mucs.config.lab_sessions.insert({ lab_id, lab });
        REQUIRE_THROWS_WITH(
            mucs.submit(),
            lab.format(
                "Lab {id} is not in session: {weekday} from {start} to {end}")
        );
    }

    SECTION("that is in session") {
        mucs.config.lab_sessions.insert({
            lab_id, RandLabSesh().today().now().get()
        });
        REQUIRE_THROWS_WITH(
            mucs.submit(),
            "Assignment type not recognized: "
        );
    }
}


TEST_CASE("submit lab with multiple lab sessions", "[mucs][submit]") {
    string user = rand_user();
    string labA = rand_lab_id();
    string labB = rand_lab_id();

    Mucs mucs;
    mucs.user = user;
    mucs.config.roster.insert({ user, {labA, labB} });

    SECTION("none are active") {
        mucs.config.lab_sessions.insert({
            labA, RandLabSesh(labA).today().now(false).get()
        });
        mucs.config.lab_sessions.insert({
            labB, RandLabSesh(labB).today().now(false).get()
        });
        REQUIRE_THROWS_WITH(
            mucs.submit(),
            "None of your labs are in session: " +
                stl_join(vector<string>{labA, labB})
        );
    }

    SECTION("one is active") {
        mucs.config.lab_sessions.insert({
            labA, RandLabSesh(labA).today().now(false).get()
        });
        mucs.config.lab_sessions.insert({
            labB, RandLabSesh(labB).today().now().get()
        });
        REQUIRE_THROWS_WITH(
            mucs.submit(),
            "Assignment type not recognized: "
        );
    }

    SECTION("all are active") {
        mucs.config.lab_sessions.insert({
            labA, RandLabSesh(labA).today().now().get()
        });
        mucs.config.lab_sessions.insert({
            labB, RandLabSesh(labB).today().now().get()
        });
        REQUIRE_THROWS_WITH(
            mucs.submit(),
            "Assignment type not recognized: "
        );
    }
}


TEST_CASE("submit homework", "[mucs][submit]") {
    string user = rand_user();
    NOW = system_clock::now();
    string hw_name = rand_hw_name();
    string lab_id = rand_lab_id();
    int year = current_year();

    Mucs mucs;
    mucs.user = user;
    mucs.assignment_type = "hw";
    mucs.config.course_id = rand_course();
    mucs.config.roster.insert({ user, {lab_id} });
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh().today().now().get()
    });

    Mock<Mucs> spy(mucs);
    When(Method(spy, try_compile_sources)).Return(true);
    Fake(Method(spy, submit_summary));
    When(Method(spy, prompt_yesno)).Return(false);

    SECTION("when there are no homeworks") {
        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            "No open homework assignments for course: " + mucs.config.course_id
        );
    }

    SECTION("when there is one past-due homework") {
        system_clock::time_point dd =
            parse_datetime(to_string(year-1) + "-01-01 00:00:00");
        mucs.config.homeworks.insert({ hw_name, Hw{hw_name, dd} });
        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            "No open homework assignments for course: " + mucs.config.course_id
        );
    }

    SECTION("when there is one active homework") {
        system_clock::time_point dd =
            parse_datetime(to_string(year+1) + "-01-01 00:00:00");
        mucs.config.homeworks.insert({ hw_name, Hw{hw_name, dd} });
        REQUIRE_THROWS_WITH(mucs.submit(), "Submission cancelled");
    }
}


TEST_CASE("attempt to submit non-compiling sources", "[mucs][submit]") {
    // TODO
}


TEST_CASE("submission cancelled by user", "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_id();

    Mucs mucs;
    mucs.user = user;
    mucs.assignment_type = "lab";
    mucs.config.roster.insert({ user, {lab_id} });
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh().today().now().get() });

    Mock<Mucs> spy(mucs);
    When(Method(spy, prompt_yesno)).Return(false);
    Fake(Method(spy, submit_summary));
    When(Method(spy, try_compile_sources)).Return(true);

    REQUIRE_THROWS_WITH(spy.get().submit(), "Submission cancelled");
}


TEST_CASE("double submit homework too quickly", "[mucs][submit]") {
    // TODO
}
