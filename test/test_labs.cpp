#include "test_labs.hpp"


TEST_CASE("config has no key lab_sessions", "[config][lab_sessions]") {
    auto data = new_config_data();
    data.erase("lab_sessions");
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "lab_sessions", "object")
    );
}


TEST_CASE("value for key lab_sessions has incorrect type",
          "[config][lab_sessions]") {
    auto data = new_config_data({ {"lab_sessions", rand_int(9)} });
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "lab_sessions", "object")
    );
}


TEST_CASE("lab_sessions entry has incorrect type", "[lab_sessions][entry]") {
    auto& config = new_config();
    string key = rand_string(1, chars_upper);
    json data = { {key, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<LabSessions>(),
        "lab_sessions entries must be of type string: " +
            config.filename + "[\"lab_sessions\"][\"" + key + "\"]"
    );
}


TEST_CASE("lab_sessions entry has incorrect format", "[lab_sessions][entry]") {
    auto& config = new_config();
    string key = rand_string(1, chars_upper);
    json data = { {key, rand_string()} };
    REQUIRE_THROWS_WITH(
        data.get<LabSessions>(),
        "lab_sessions entries must be in the format " \
            "\"<weekday> <start_time> - <end_time>\": " + config.filename +
            "[\"lab_sessions\"][\"" + key + "\"]"
    );
}


TEST_CASE("lab_sessions is valid", "[lab_sessions][entry]") {
    string key = rand_string(1, chars_upper);
    json data = { {key, "mon 00:00:00 - 23:59:59"} };
    try {
        data.get<LabSessions>();
        SUCCEED("Successfully created LabSessions object");
    } catch (const mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize lab_sessions", "[lab_sessions][serialize]") {
    string key = rand_string(1, chars_upper);
    json data = { {key, "monday 00:00:00 - 23:59:59"} };
    ostringstream expected, actual;
    expected << data;
    actual << json(data.get<LabSessions>());
    REQUIRE_THAT(
        expected.str(),
        Equals(actual.str(), Catch::CaseSensitive::No)
    );
}


TEST_CASE("is_active returns false when: weekday incorrect, time incorrect",
          "[labsesh][is_active]") {
    auto ls = rand_labsesh<NOT_TODAY,NOT_ACTIVE>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday incorrect, time correct",
          "[labsesh][is_active]") {
    auto ls = rand_labsesh<NOT_TODAY,ACTIVE>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday correct, time incorrect",
          "[labsesh][is_active]") {
    auto ls = rand_labsesh<TODAY,NOT_ACTIVE>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns true when: weekday correct, time correct",
          "[labsesh][is_active]") {
    auto ls = rand_labsesh<TODAY,ACTIVE>();
    REQUIRE(ls.is_active() == true);
}


TEST_CASE("config has no key lab_assignments", "[config][lab_assignments]") {
    auto data = new_config_data();
    data.erase("lab_assignments");
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "lab_assignments", "object")
    );
}


TEST_CASE("value for key lab_assignments has incorrect type",
          "[config][lab_assignments]") {
    auto data = new_config_data({ {"lab_assignments", rand_int(9)} });
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "lab_assignments", "object")
    );
}


TEST_CASE("lab_assignments entry has incorrect type",
          "[lab_assignments][entry]") {
    auto& config = new_config();
    string key = rand_string(1, chars_upper);
    json data = { {key, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<LabAssignments>(),
        "lab_assignments entries must be of type string: " +
            config.filename + "[\"lab_assignments\"][\"" + key + "\"]"
    );
}


TEST_CASE("lab_assignments entry has incorrect format",
          "[lab_assignments][entry]") {
    auto& config = new_config();
    string key = rand_string(1, chars_upper);
    json data = { {key, rand_string()} };
    REQUIRE_THROWS_WITH(
        data.get<LabAssignments>(),
        "lab_assignments entries must be in the format \"yyyy-mm-dd\": " +
            config.filename + "[\"lab_assignments\"][\"" + key + "\"]"
    );
}


TEST_CASE("lab_assignments is valid", "[lab_assignments][entry]") {
    string key = "lab" + to_string(rand_int(999));
    json data = { {key, "1970-01-01"} };
    try {
        data.get<LabAssignments>();
        SUCCEED("Successfully created LabSessions object");
    } catch (const mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize lab_assignments", "[lab_assignments][serialize]") {
    string key = "lab" + to_string(rand_int(999));
    json data = { {key, "1970-01-01"} };
    ostringstream expected, actual;
    expected << data;
    actual << json(data.get<LabAssignments>());
    REQUIRE_THAT(
        expected.str(),
        Equals(actual.str())
    );
}
