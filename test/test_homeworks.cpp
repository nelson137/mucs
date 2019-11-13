#include "test_homeworks.hpp"


TEST_CASE("config has no key homeworks", "[config][homeworks]") {
    auto data = new_config_data();
    data.erase("homeworks");
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "homeworks", "object")
    );
}


TEST_CASE("value for key homeworks has incorrect type", "[config][homeworks]") {
    auto data = new_config_data();
    data["homeworks"] = rand_int(9);
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "homeworks", "object")
    );
}


TEST_CASE("homeworks entry has incorrect type", "[config][homeworks][entry]") {
    auto data = new_config_data();
    string fn = data["filename"].get<string>();
    string key = "hw" + to_string(rand_int(9));
    data["homeworks"][key] = rand_int(9);
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        "Homework entries must be of type string: " + fn +
            "[\"homeworks\"][\"" + key + "\"]"
    );
}


TEST_CASE("homeworks entry has incorrect format",
          "[config][homeworks][entry]") {
    auto data = new_config_data();
    string key = "hw" + to_string(rand_int(9));
    string hw = rand_string();
    data["homeworks"][key] = hw;
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        "Invalid datetime: " + hw
    );
}


TEST_CASE("deserialized homeworks entries are in sorted order",
        "[config][homeworks]") {
    auto data = new_config_data();
    data["homeworks"]["hw1"] = "2019-01-01 00:00:00";
    data["homeworks"]["hw2"] = "1970-01-01 00:00:00";
    auto config = data.get<Config>();
    REQUIRE(config.homeworks.begin()->first == "hw2");
}


TEST_CASE("homeworks is valid", "[config][homeworks][entry]") {
    auto data = new_config_data();
    string key = "hw" + to_string(rand_int(9));
    data["homeworks"][key] = "1970-01-01 00:00:00";
    try {
        data.get<Config>();
        SUCCEED("Successfully created Homeworks object");
    } catch (mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize homeworks", "[config][homeworks][serialize]") {
    auto data = new_config_data();
    data["homeworks"]["hw1"] = "1970-01-01 00:00:00";
    auto config = data.get<Config>();
    ostringstream expected, actual;
    expected << data["homeworks"];
    actual << json(config.homeworks);
    REQUIRE_THAT(
        expected.str(),
        Equals(actual.str(), Catch::CaseSensitive::No)
    );
}
