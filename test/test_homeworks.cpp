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


TEST_CASE("is_active returns true when the current datetime is before duedate",
          "[hw][is-active]") {
    Hw hw;
    NOW = sys_seconds{} + seconds(rand_int(100));
    hw.duedate = NOW + seconds(rand_int(1, 100));
    REQUIRE(hw.is_active() == true);
}


TEST_CASE("is_active returns false when the current datetime is after duedate",
          "[hw][is-active]") {
    Hw hw;
    hw.duedate = sys_seconds{} + seconds(rand_int(100));
    NOW = hw.duedate + seconds(rand_int(1, 100));
    REQUIRE(hw.is_active() == false);
}


TEST_CASE("is_active returns false when the current datetime is the same as "
              "duedate",
          "[hw][is-active]") {
    Hw hw;
    NOW = hw.duedate = sys_seconds{} + seconds(rand_int(100));
    REQUIRE(hw.is_active() == false);
}


TEST_CASE("is_active always returns true when overridden", "[hw][is-active]") {
    Hw hw;
    hw.duedate = sys_seconds{} + seconds(rand_int(100));
    NOW = hw.duedate + seconds(rand_int(1, 100));

    REQUIRE(hw.is_active() == false);
    hw.override();
    REQUIRE(hw.is_active() == true);
}


TEST_CASE("find_name returns nullptr when no matching Hw is found",
          "[homeworks][find-name]") {
    Homeworks homeworks;
    homeworks.emplace(rand_hw_name());
    homeworks.emplace(rand_hw_name());
    REQUIRE(homeworks.find_name(rand_hw_name()) == nullptr);
}


TEST_CASE("find_name returns a pointer to the first Hw with a matching name",
          "[homeworks][find-name]") {
    string expected_name = rand_hw_name();

    Homeworks homeworks;
    homeworks.emplace(rand_hw_name());
    homeworks.emplace(expected_name);
    homeworks.emplace(rand_hw_name());

    const Hw *found = homeworks.find_name(expected_name);

    REQUIRE(found != nullptr);
    REQUIRE(found->name == expected_name);
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
