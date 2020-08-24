#include "test_homeworks.hpp"


TEST_CASE("hw has incorrect type", "[hw]") {
    string key = rand_hw_name();
    json data = rand_int(9);
    REQUIRE_THROWS_WITH(
        data.get<Hw>(),
        "Homework objects must be of type object"
    );
}


TEST_CASE("hw missing key name", "[hw]") {
    json data = { {"duedate", ""} };
    REQUIRE_THROWS_WITH(
        data.get<Hw>(),
        error_prop("Homework", "name", "string")
    );
}


TEST_CASE("hw missing key duedate", "[hw]") {
    json data = { {"name", ""} };
    REQUIRE_THROWS_WITH(
        data.get<Hw>(),
        error_prop("Homework", "duedate", "string")
    );
}


TEST_CASE("hw duedate has incorrect format", "[hw]") {
    string name = rand_hw_name();
    json data = { {"name", name}, {"duedate", name} };
    REQUIRE_THROWS_WITH(
        data.get<Hw>(),
        "Invalid datetime: " + name
    );
}


TEST_CASE("hw is valid", "[hw]") {
    string name = rand_hw_name();
    json data = { {"name", name}, {"duedate", "1970-01-01 00:00:00"} };
    REQUIRE_NOTHROW(data.get<Hw>());
}


TEST_CASE("serialize hw", "[homeworks][serialize]") {
    string name = rand_hw_name();
    json data = { {"name", name}, {"duedate", "1970-01-01 00:00:00"} };
    string expected = data.dump();
    string actual = json(data.get<Hw>()).dump();
    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}


TEST_CASE("deserialized homeworks entries are in sorted order",
          "[homeworks]") {
    string name1 = rand_hw_name();
    string name2 = rand_hw_name();
    json data = {
        { {"name", name1}, {"duedate", "2019-01-01 00:00:00"} },
        { {"name", name2}, {"duedate", "1970-01-01 00:00:00"} }
    };
    auto homeworks = data.get<Homeworks>();
    REQUIRE(homeworks.begin()->name == name2);
}
