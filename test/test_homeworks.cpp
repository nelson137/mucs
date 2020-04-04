#include "test_homeworks.hpp"


TEST_CASE("homeworks entry has incorrect type", "[homeworks][entry]") {
    string key = rand_hw_name();
    json data = { {key, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<Homeworks>(),
        "Homework entries must be of type string: " \
            "{filename}[\"homeworks\"][\"" + key + "\"]"
    );
}


TEST_CASE("homeworks entry has incorrect format", "[homeworks][entry]") {
    string key = rand_hw_name();
    json data = { {key, key} };
    REQUIRE_THROWS_WITH(
        data.get<Homeworks>(),
        "Invalid datetime: " + key
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
    string key = rand_hw_name();
    json data = { {key, "1970-01-01 00:00:00"} };
    REQUIRE_NOTHROW(data.get<Homeworks>());
}


TEST_CASE("serialize homeworks", "[homeworks][serialize]") {
    json data = { {"hw1", "1970-01-01 00:00:00"} };
    string expected = data.dump();
    string actual = json(data.get<Homeworks>()).dump();
    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}
