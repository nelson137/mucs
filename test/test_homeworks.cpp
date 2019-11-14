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
    string fn = rand_string();
    string key = "hw" + to_string(rand_int(9));
    json data = { {key, rand_int(9)} };
    Homeworks homeworks(fn);
    REQUIRE_THROWS_WITH(
        data.get_to(homeworks),
        "Homework entries must be of type string: " + fn +
            "[\"homeworks\"][\"" + key + "\"]"
    );
}


TEST_CASE("homeworks entry has incorrect format",
          "[config][homeworks][entry]") {
    string key = "hw" + to_string(rand_int(9));
    string hw = rand_string();
    json data = { {key, hw} };
    REQUIRE_THROWS_WITH(
        data.get<Homeworks>(),
        "Invalid datetime: " + hw
    );
}


TEST_CASE("deserialized homeworks entries are in sorted order",
        "[config][homeworks]") {
    json data = {
        {"hw1", "2019-01-01 00:00:00"},
        {"hw2", "1970-01-01 00:00:00"}
    };
    auto homeworks = data.get<Homeworks>();
    REQUIRE(homeworks.begin()->first == "hw2");
}


TEST_CASE("homeworks is valid", "[config][homeworks][entry]") {
    string key = "hw" + to_string(rand_int(9));
    json data = { {key, "1970-01-01 00:00:00"} };
    try {
        data.get<Homeworks>();
        SUCCEED("Successfully created Homeworks object");
    } catch (mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize homeworks", "[config][homeworks][serialize]") {
    json data = { {"hw1", "1970-01-01 00:00:00"} };
    ostringstream expected, actual;
    expected << data;
    actual << json(data.get<Homeworks>());
    REQUIRE_THAT(
        expected.str(),
        Equals(actual.str(), Catch::CaseSensitive::No)
    );
}
