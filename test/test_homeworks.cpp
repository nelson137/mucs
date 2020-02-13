#include "test_homeworks.hpp"


TEST_CASE("config has no key homeworks", "[config][homeworks]") {
    auto data = new_config_data();
    data.erase("homeworks");
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "homeworks", "object")
    );
}


TEST_CASE("value for key homeworks has incorrect type", "[config][homeworks]") {
    auto data = new_config_data();
    data["homeworks"] = rand_int(9);
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "homeworks", "object")
    );
}


TEST_CASE("homeworks entry has incorrect type", "[homeworks][entry]") {
    auto& config = new_config();
    string key = "hw" + to_string(rand_int(9));
    json data = { {key, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<Homeworks>(),
        "Homework entries must be of type string: " + config.filename +
            "[\"homeworks\"][\"" + key + "\"]"
    );
}


TEST_CASE("homeworks entry has incorrect format", "[homeworks][entry]") {
    string key = "hw" + to_string(rand_int(9));
    string hw = rand_string();
    json data = { {key, hw} };
    REQUIRE_THROWS_WITH(
        data.get<Homeworks>(),
        "Invalid datetime: " + hw
    );
}


TEST_CASE("deserialized homeworks entries are in sorted order",
          "[homeworks]") {
    json data = {
        {"hw1", "2019-01-01 00:00:00"},
        {"hw2", "1970-01-01 00:00:00"}
    };
    auto homeworks = data.get<Homeworks>();
    REQUIRE(homeworks.begin()->first == "hw2");
}


TEST_CASE("homeworks is valid", "[homeworks][entry]") {
    string key = "hw" + to_string(rand_int(9));
    json data = { {key, "1970-01-01 00:00:00"} };
    try {
        data.get<Homeworks>();
        SUCCEED("Successfully created Homeworks object");
    } catch (const mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize homeworks", "[homeworks][serialize]") {
    json data = { {"hw1", "1970-01-01 00:00:00"} };
    ostringstream expected, actual;
    expected << data;
    actual << json(data.get<Homeworks>());
    REQUIRE_THAT(
        expected.str(),
        Equals(actual.str(), Catch::CaseSensitive::No)
    );
}
