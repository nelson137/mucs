#include "test_labs.hpp"


TEST_CASE("config has no key labs", "[config][labs]") {
    auto data = new_config_data();
    data.erase("labs");
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "labs", "object")
    );
}


TEST_CASE("value for key labs has incorrect type", "[config][labs]") {
    auto data = new_config_data();
    data["labs"] = rand_int(9);
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "labs", "object")
    );
}


TEST_CASE("labs entry has incorrect type", "[labs][entry]") {
    string fn = rand_string();
    string key = rand_string(1, chars_upper);
    json data = { {key, rand_int(9)} };
    LabSessions lab_sessions(fn);
    REQUIRE_THROWS_WITH(
        data.get_to(lab_sessions),
        "Lab entries must be of type string: " +
            fn + "[\"labs\"][\"" + key + "\"]"
    );
}


TEST_CASE("labs entry has incorrect format", "[labs][entry]") {
    string fn = rand_string();
    string key = rand_string(1, chars_upper);
    json data = { {key, rand_string()} };
    LabSessions lab_sessions(fn);
    REQUIRE_THROWS_WITH(
        data.get_to(lab_sessions),
        "Lab entries must be in the format " \
            "\"<weekday> <start_time> - <end_time>\": " + fn +
            "[\"labs\"][\"" + key + "\"]"
    );
}


TEST_CASE("labs is valid", "[labs][entry]") {
    string key = rand_string(1, chars_upper);
    json data = { {key, "mon 00:00:00 - 23:59:59"} };
    LabSessions lab_sessions(rand_string());
    try {
        data.get_to(lab_sessions);
        SUCCEED("Successfully created LabSessions object");
    } catch (const mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize labs", "[labs][serialize]") {
    string key = rand_string(1, chars_upper);
    json data = { {key, "monday 00:00:00 - 23:59:59"} };
    LabSessions lab_sessions(rand_string());
    data.get_to(lab_sessions);
    ostringstream expected, actual;
    expected << data;
    actual << json(lab_sessions);
    REQUIRE_THAT(
        expected.str(),
        Equals(actual.str(), Catch::CaseSensitive::No)
    );
}


TEST_CASE("is_active returns false when: weekday incorrect, time incorrect",
          "[lab-sesh][is_active]") {
    auto ls = rand_labsesh<NOT_TODAY,T_OUT_BOUND>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday incorrect, time correct",
          "[labs][is_active]") {
    auto ls = rand_labsesh<NOT_TODAY,T_IN_BOUND>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday correct, time incorrect",
          "[labs][is_active]") {
    auto ls = rand_labsesh<IS_TODAY,T_OUT_BOUND>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns true when: weekday correct, time correct",
          "[labs][is_active]") {
    auto ls = rand_labsesh<IS_TODAY,T_IN_BOUND>();
    REQUIRE(ls.is_active() == true);
}
