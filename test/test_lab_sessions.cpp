#include "test_lab_sessions.hpp"


TEST_CASE("get_with_id returns an empty list when none match",
          "[lab-sessions][get-with-id]") {
    string id = rand_lab_sesh_id();
    LabSessions sessions;
    sessions.push_back(RandLabSesh().today(false).now(false).get());
    sessions.push_back(RandLabSesh().today(false).now(true).get());
    sessions.push_back(RandLabSesh().today(true).now(false).get());
    sessions.push_back(RandLabSesh().today(true).now(true).get());

    REQUIRE(sessions.get_with_id(id).size() == 0);
}


TEST_CASE("get_with_id returns a filtered list of LabSesh objects whose ids "
              "match the given id",
          "[lab-sessions][get-with-id]") {
    string id = rand_lab_sesh_id();
    LabSessions sessions;
    sessions.push_back(RandLabSesh().today(false).now(false).get());
    sessions.push_back(RandLabSesh().today(false).now(true).get());
    sessions.push_back(RandLabSesh(id).today().now().get());
    sessions.push_back(RandLabSesh().today(true).now(false).get());
    sessions.push_back(RandLabSesh(id).today().now(false).get());
    sessions.push_back(RandLabSesh().today(true).now(true).get());

    const vector<LabSesh>& actual = sessions.get_with_id(id);

    REQUIRE(actual.size() == 2);
    for (const LabSesh& ls : actual)
        REQUIRE(ls.id == id);
}


TEST_CASE("lab-sesh day is invalid", "[lab-sesh]") {
    string id = rand_lab_sesh_id();
    json data = rand_lab_sesh_data({ {"id", id}, {"day", rand_string(1)} });
    REQUIRE_THROWS_WITH(
        data.get<LabSesh>(),
        "Lab session weekday is invalid (first char must be capitalized): " \
            "{filename}[\"lab-sessions\"][\"" + id + "\"]"
    );
}


TEST_CASE("lab-sesh start time is invalid", "[lab-sesh]") {
    string id = rand_lab_sesh_id();
    json data = rand_lab_sesh_data({ {"id", id}, {"start", rand_string(1)} });
    REQUIRE_THROWS_WITH(
        data.get<LabSesh>(),
        "Lab session start time is invalid: " \
            "{filename}[\"lab-sessions\"][\"" + id + "\"]"
    );
}


TEST_CASE("lab-sesh end time is invalid", "[lab-sesh]") {
    string id = rand_lab_sesh_id();
    json data = rand_lab_sesh_data({ {"id", id}, {"end", rand_string(1)} });
    REQUIRE_THROWS_WITH(
        data.get<LabSesh>(),
        "Lab session end time is invalid: " \
            "{filename}[\"lab-sessions\"][\"" + id + "\"]"
    );
}


TEST_CASE("lab-sesh is valid", "[lab-sesh]") {
    string id = rand_lab_sesh_id();
    json data = rand_lab_sesh_data({ {"id", id} });
    REQUIRE_NOTHROW(data.get<LabSesh>());
}


TEST_CASE("serialize lab-sesh", "[lab-sesh][serialize]") {
    string id = rand_lab_sesh_id();

    string expected = rand_lab_sesh_data({ {"id", id}, {"day", "Mon"} }).dump();

    LabSesh ls(id, Monday, seconds(0), days(1) - seconds(1));
    string actual = json(ls).dump();

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
