#include "test_labs.hpp"


TEST_CASE("config has no key labs", "[config][labs]") {
    auto data = new_config<json>();
    data.erase("labs");
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "labs", "object")
    );
}


TEST_CASE("value for key labs has incorrect type", "[config][labs]") {
    auto data = new_config<json>({ {"labs", rand_int(9)} });
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "labs", "object")
    );
}


TEST_CASE("labs entry has incorrect type", "[labs][entry]") {
    auto& config = new_config<Config&>();
    string key = rand_string(1, chars_upper);
    json data = { {key, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<LabSessions>(),
        "Lab entries must be of type string: " +
            config.filename + "[\"labs\"][\"" + key + "\"]"
    );
}


TEST_CASE("labs entry has incorrect format", "[labs][entry]") {
    auto& config = new_config<Config&>();
    string key = rand_string(1, chars_upper);
    json data = { {key, rand_string()} };
    REQUIRE_THROWS_WITH(
        data.get<LabSessions>(),
        "Lab entries must be in the format " \
            "\"<weekday> <start_time> - <end_time>\": " + config.filename +
            "[\"labs\"][\"" + key + "\"]"
    );
}


TEST_CASE("labs is valid", "[labs][entry]") {
    string key = rand_string(1, chars_upper);
    json data = { {key, "mon 00:00:00 - 23:59:59"} };
    try {
        data.get<LabSessions>();
        SUCCEED("Successfully created LabSessions object");
    } catch (const mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize labs", "[labs][serialize]") {
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
          "[lab-sesh][is_active]") {
    auto ls = rand_labsesh<NOT_TODAY,NOT_ACTIVE>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday incorrect, time correct",
          "[labs][is_active]") {
    auto ls = rand_labsesh<NOT_TODAY,ACTIVE>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday correct, time incorrect",
          "[labs][is_active]") {
    auto ls = rand_labsesh<TODAY,NOT_ACTIVE>();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns true when: weekday correct, time correct",
          "[labs][is_active]") {
    auto ls = rand_labsesh<TODAY,ACTIVE>();
    REQUIRE(ls.is_active() == true);
}
