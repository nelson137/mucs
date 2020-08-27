#include "test_homeworks.hpp"


TEST_CASE("hw duedate has incorrect format", "[hw]") {
    string name = rand_hw_name();
    json data = { {"name", name}, {"duedate", name} };
    REQUIRE_THROWS_WITH(
        data.get<Hw>(),
        "Invalid duedate: {filename}[\"homeworks\"][\"" + name + "\"]"
    );
}


TEST_CASE("hw is valid", "[hw]") {
    string name = rand_hw_name();
    json data = { {"name", name}, {"duedate", "1970-01-01 00:00:00"} };
    REQUIRE_NOTHROW(data.get<Hw>());
}


TEST_CASE("serialize hw", "[homeworks][serialize]") {
    string name = rand_hw_name();

    json j;
    j["name"] = name;
    j["duedate"] = "1970-01-01 00:00:00";
    string expected = j.dump();

    Hw hw(name, sys_seconds{});
    string actual = json(hw).dump();

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
