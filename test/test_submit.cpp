#include "test_submit.hpp"


TEST_CASE("submit user not in course", "[mucs][submit]") {
    TEST_USER = rand_string();
    REQUIRE_THROWS_WITH(
        Mucs().submit(Config()),
        "User not in course: " + TEST_USER
    );
}


TEST_CASE("submit lab with one lab session", "[mucs][submit]") {
    TEST_USER = rand_string();
    string lab_id = rand_string(3, chars_upper);

    Config config;
    config.roster.insert({ TEST_USER, {lab_id} });

    SECTION("that is not in session") {
        auto lab = rand_labsesh<TODAY,NOT_ACTIVE>();
        config.lab_sessions.insert({ lab_id, lab });
        REQUIRE_THROWS_WITH(
            Mucs().submit(config),
            lab.format(
                "Lab {id} is not in session: {weekday} from {start} to {end}")
        );
    }

    SECTION("that is in session") {
        config.lab_sessions.insert({ lab_id, rand_labsesh<TODAY,ACTIVE>() });
        REQUIRE_THROWS_WITH(
            Mucs().submit(config),
            "Assignment type not recognized: "
        );
    }
}


TEST_CASE("submit lab with multiple lab sessions", "[mucs][submit]") {
    TEST_USER = rand_string();
    string labA = rand_string(3, chars_upper);
    string labB = rand_string(3, chars_upper);

    Config config;
    config.roster.insert({ TEST_USER, {labA, labB} });

    SECTION("none are active") {
        config.lab_sessions.insert({
            labA, rand_labsesh<TODAY,NOT_ACTIVE>(labA)
        });
        config.lab_sessions.insert({
            labB, rand_labsesh<TODAY,NOT_ACTIVE>(labB)
        });
        REQUIRE_THROWS_WITH(
            Mucs().submit(config),
            "None of your labs are in session: " +
                stl_join<vector<string>>({labA, labB})
        );
    }

    SECTION("one is active") {
        config.lab_sessions.insert({
            labA, rand_labsesh<TODAY,NOT_ACTIVE>(labA)
        });
        config.lab_sessions.insert({
            labB, rand_labsesh<TODAY,ACTIVE>(labB)
        });
        REQUIRE_THROWS_WITH(
            Mucs().submit(config),
            "Assignment type not recognized: "
        );
    }

    SECTION("all are active") {
        config.lab_sessions.insert({
            labA, rand_labsesh<TODAY,ACTIVE>(labA)
        });
        config.lab_sessions.insert({
            labB, rand_labsesh<TODAY,ACTIVE>(labB)
        });
        REQUIRE_THROWS_WITH(
            Mucs().submit(config),
            "Assignment type not recognized: "
        );
    }
}


TEST_CASE("submit homework", "[mucs][submit]") {
    TEST_USER = rand_string();
    NOW = system_clock::now();
    string hw_name = rand_string();
    string lab_id = rand_string();
    int year = current_year();

    Config config;
    config.course_id = rand_string();
    config.roster.insert({ TEST_USER, {lab_id} });
    config.lab_sessions.insert({ lab_id, rand_labsesh<TODAY,ACTIVE>() });

    Mucs mucs;
    mucs.assignment_type = "hw";

    TEST_PROMPT_YESNO = "n";

    SECTION("when there are no homeworks") {
        REQUIRE_THROWS_WITH(
            mucs.submit(config),
            "No open homework assignments for course: " + config.course_id
        );
    }

    SECTION("when there is one past-due homework") {
        system_clock::time_point dd =
            parse_datetime(to_string(year-1) + "-01-01 00:00:00");
        config.homeworks.insert({ hw_name, Hw{hw_name, dd} });
        REQUIRE_THROWS_WITH(
            mucs.submit(config),
            "No open homework assignments for course: " + config.course_id
        );
    }

    SECTION("when there is one active homework") {
        system_clock::time_point dd =
            parse_datetime(to_string(year+1) + "-01-01 00:00:00");
        config.homeworks.insert({ hw_name, Hw{hw_name, dd} });
        REQUIRE_THROWS_WITH(mucs.submit(config), "Submission cancelled");
    }
}


TEST_CASE("attempt to submit non-compiling sources", "[mucs][submit]") {
    // TODO
}


TEST_CASE("submission cancelled by user", "[mucs][submit]") {
    TEST_USER = rand_string();
    string lab_id = rand_string(3, chars_upper);

    Config config;
    config.roster.insert({ TEST_USER, {lab_id} });
    config.lab_sessions.insert({ lab_id, rand_labsesh<TODAY,ACTIVE>() });

    Mucs mucs;
    mucs.assignment_type = "lab";

    TEST_PROMPT_YESNO = "n";

    REQUIRE_THROWS_WITH(mucs.submit(config), "Submission cancelled");
}


TEST_CASE("double submit homework too quickly", "[mucs][submit]") {
    // TODO
}
