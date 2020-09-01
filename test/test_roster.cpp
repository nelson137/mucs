#include "test_roster.hpp"


TEST_CASE("roster entry has incorrect type", "[roster][entry]") {
    string fn = rand_filename();
    string user = rand_user();
    json data = new_config_data({
        {"roster", { {user, rand_int(9)} }}
    });
    REQUIRE_THROWS_WITH(
        Config(data, fn).validate(),
        StartsWith("Invalid config: " + fn) && Contains(INVALID_VALUE_TYPE)
    );
}


TEST_CASE("roster entry has one lab id", "[roster][entry]") {
    string user = rand_user();
    vector<string> expected_ids = { rand_lab_sesh_id() };
    json data = new_config_data({
        {"lab-sessions", {
            rand_lab_sesh_data({ {"id", expected_ids[0]} })
        }},
        {"roster", { {user, expected_ids[0]} }},
    });

    Config config = Config(data).parse();

    REQUIRE(config.roster.count(user) == 1);
    auto actual_ids = config.roster.at(user);
    REQUIRE_THAT(actual_ids, Vector::EqualsMatcher<string>(expected_ids));
}


TEST_CASE("roster entry has multiple lab ids", "[roster][entry]") {
    string user = rand_user();
    vector<string> expected_ids = {
        rand_lab_sesh_id(), rand_lab_sesh_id(), "", rand_lab_sesh_id()
    };
    ostringstream ids;
    ids << expected_ids[0];
    for (auto it=begin(expected_ids)+1; it!=end(expected_ids); it++)
        ids << ',' << *it;

    json data = new_config_data({
        {"lab-sessions", {
            rand_lab_sesh_data({ {"id", expected_ids[1]} })
        }},
        {"roster", { {user, ids.str()} }}
    });

    Config config = Config(data).parse();

    REQUIRE(config.roster.count(user) == 1);
    auto actual_ids = config.roster.at(user);
    REQUIRE_THAT(actual_ids, Vector::EqualsMatcher<string>(expected_ids));
}


TEST_CASE("serialize roster", "[roster][serialize]") {
    string user = rand_user();
    string id = rand_lab_sesh_id();
    json data = json::object();

    SECTION("with one id") {
        data[user] = id;
        string expected = data.dump();
        string actual = json(data.get<Roster>()).dump();
        REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
    }

    SECTION("with multiple ids") {
        data[user] = id + ',' + id;
        string expected = data.dump();
        string actual = json(data.get<Roster>()).dump();
        REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
    }
}
