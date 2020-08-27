#include "test_lab_sessions.hpp"


TEST_CASE("lab-sessions entry has incorrect format", "[lab-sessions][entry]") {
    string key = rand_lab_sesh_id();
    json data = { {key, key} };
    REQUIRE_THROWS_WITH(
        data.get<LabSessions>(),
        "Lab sessions must be in the format " \
            "\"<weekday> <start_time> - <end_time>\": " \
            "{filename}[\"lab-sessions\"][\"" + key + "\"]"
    );
}


TEST_CASE("lab-sessions is valid", "[lab-sessions][entry]") {
    string key = rand_lab_sesh_id();
    json data = { {key, "Mon 00:00:00 - 23:59:59"} };
    REQUIRE_NOTHROW(data.get<LabSessions>());
}


TEST_CASE("serialize lab-sessions", "[lab-sessions][serialize]") {
    string id = rand_lab_sesh_id();

    string expected = json({ {id, "Monday 00:00:00 - 23:59:59"} }).dump();

    LabSesh ls(id, Monday, seconds(0), days(1) - seconds(1));
    LabSessions sessions;
    sessions.insert({ id, ls });
    string actual = json(sessions).dump();

    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}


TEST_CASE("is_active returns false when: weekday incorrect, time incorrect",
          "[lab-sesh][is_active]") {
    const LabSesh& ls = RandLabSesh().today(false).now(false).get();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday incorrect, time correct",
          "[lab-sesh][is_active]") {
    const LabSesh& ls = RandLabSesh().today(false).now().get();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns false when: weekday correct, time incorrect",
          "[lab-sesh][is_active]") {
    const LabSesh& ls = RandLabSesh().today().now(false).get();
    REQUIRE(ls.is_active() == false);
}


TEST_CASE("is_active returns true when: weekday correct, time correct",
          "[lab-sesh][is_active]") {
    const LabSesh& ls = RandLabSesh().today().now().get();
    REQUIRE(ls.is_active() == true);
}
