#include "test_roster.hpp"


TEST_CASE("config has no key roster", "[config][roster]") {
    json data = new_config_data();
    data.erase("roster");
    REQUIRE_THROWS_WITH(
        Config::parse(data),
        error_prop("roster", "object")
    );
}


TEST_CASE("value for key roster has incorrect type", "[config][roster]") {
    json data = new_config_data({ {"roster", rand_int(9)} });
    REQUIRE_THROWS_WITH(
        Config::parse(data),
        error_prop("roster", "object")
    );
}


TEST_CASE("roster entry has incorrect type", "[roster][entry]") {
    string user = rand_user();
    json data = { {user, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<Roster>(),
        "Roster entries must be of type string: " \
            "{filename}[\"roster\"][\"" + user + "\"]"
    );
}


TEST_CASE("roster entry has one lab id", "[roster][entry]") {
    string user = rand_user();
    string id = rand_lab_id();
    json data = new_config_data({
        {"labs", { {id, "mon 00:00:00 - 23:59:59"} }},
        {"roster", {}},
    });

    SECTION("that is unrecognized") {
        string bad_id = id + "_";
        data["roster"][user] = bad_id;
        REQUIRE_THROWS_WITH(
            Config::parse(data),
            error_id_unrecognized(user, bad_id)
        );
    }

    SECTION("that is recognized") {
        data["roster"][user] = id;
        REQUIRE_NOTHROW(Config::parse(data));
    }
}


TEST_CASE("roster entry has multiple lab ids", "[roster][entry]") {
    string user = rand_user();
    string id = rand_lab_id();
    json data = new_config_data({
        {"labs", { {id, "mon 00:00:00 - 23:59:59"} }},
        {"roster", {}}
    });

    SECTION("one recognized, other unrecognized") {
        string bad_id = id + '_';
        data["roster"][user] = id + ',' + bad_id;
        REQUIRE_THROWS_WITH(
            Config::parse(data),
            error_id_unrecognized(user, bad_id)
        );
    }

    SECTION("all recognized") {
        data["roster"][user] = id + "," + id;
        REQUIRE_NOTHROW(Config::parse(data));
    }
}


TEST_CASE("serialize roster", "[roster][serialize]") {
    string user = rand_user();
    string id = rand_lab_id();
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
