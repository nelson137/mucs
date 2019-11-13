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


TEST_CASE("labs entry has incorrect type", "[config][labs][entry]") {
    auto data = new_config_data();
    string fn = data["filename"].get<string>();
    string key = rand_string(1, chars_upper);
    data["labs"][key] = rand_int(9);
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        "Lab entries must be of type string: " +
            fn + "[\"labs\"][\"" + key + "\"]"
    );
}


TEST_CASE("labs entry has incorrect format", "[config][labs][entry]") {
    auto data = new_config_data();
    string fn = data["filename"].get<string>();
    string key = rand_string(1, chars_upper);
    data["labs"][key] = rand_string();
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        "Lab entries must be in the format " \
            "\"<weekday> <start_time> - <end_time>\": " + fn +
            "[\"labs\"][\"" + key + "\"]"
    );
}


TEST_CASE("labs is valid", "[config][labs][entry]") {
    auto data = new_config_data();
    string key = rand_string(1, chars_upper);
    data["labs"][key] = "mon 00:00:00 - 23:59:59";
    try {
        data.get<Config>();
        SUCCEED("Successfully created LabSessions object");
    } catch (mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize labs", "[config][labs][serialize]") {
    auto data = new_config_data();
    data["labs"]["A"] = "monday 00:00:00 - 23:59:59";
    auto config = data.get<Config>();
    ostringstream expected, actual;
    expected << data["labs"];
    actual << json(config.lab_sessions);
    REQUIRE_THAT(
        expected.str(),
        Equals(actual.str(), Catch::CaseSensitive::No)
    );
}
