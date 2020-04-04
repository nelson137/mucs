#include "test_labs.hpp"


TEST_CASE("labs entry has incorrect type", "[labs][entry]") {
    string key = rand_lab_id();
    json data = { {key, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<LabSessions>(),
        "Lab entries must be of type string: " \
            "{filename}[\"labs\"][\"" + key + "\"]"
    );
}


TEST_CASE("labs entry has incorrect format", "[labs][entry]") {
    string key = rand_lab_id();
    json data = { {key, key} };
    REQUIRE_THROWS_WITH(
        data.get<LabSessions>(),
        "Lab entries must be in the format " \
            "\"<weekday> <start_time> - <end_time>\": " \
            "{filename}[\"labs\"][\"" + key + "\"]"
    );
}


TEST_CASE("labs is valid", "[labs][entry]") {
    string key = rand_lab_id();
    json data = { {key, "mon 00:00:00 - 23:59:59"} };
    REQUIRE_NOTHROW(data.get<LabSessions>());
}


TEST_CASE("serialize labs", "[labs][serialize]") {
    string key = rand_lab_id();
    json data = { {key, "monday 00:00:00 - 23:59:59"} };
    string expected = data.dump();
    string actual = json(data.get<LabSessions>()).dump();
    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}


TEST_CASE("is_active returns false when: weekday incorrect, time incorrect",
          "[lab-sesh][is_active]") {
    const LabSesh& ls = RandLabSesh().today(false).now(false).get();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday incorrect, time correct",
          "[labs][is_active]") {
    const LabSesh& ls = RandLabSesh().today(false).now().get();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday correct, time incorrect",
          "[labs][is_active]") {
    const LabSesh& ls = RandLabSesh().today().now(false).get();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns true when: weekday correct, time correct",
          "[labs][is_active]") {
    const LabSesh& ls = RandLabSesh().today().now().get();
    REQUIRE(ls.is_active() == true);
}
