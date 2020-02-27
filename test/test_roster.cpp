#include "test_roster.hpp"


TEST_CASE("config has no key roster", "[config][roster]") {
    auto data = new_config_data();
    data.erase("roster");
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "roster", "object")
    );
}


TEST_CASE("value for key roster has incorrect type", "[config][roster]") {
    auto data = new_config_data({ {"roster", rand_int(9)} });
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "roster", "object")
    );
}


TEST_CASE("roster entry has incorrect type", "[roster][entry]") {
    auto& config = new_config();
    string user = rand_string(6);
    json data = { {user, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<Roster>(),
        "Roster entries must be of type string: " +
            config.filename + "[\"roster\"][\"" + user + "\"]"
    );
}


TEST_CASE("roster entry has one lab id", "[roster][entry]") {
    string user = rand_string(6);
    string id = rand_string(2, chars_lower);
    auto data = new_config_data();
    data["labs"][id] = "mon 00:00:00 - 23:59:59";
    auto& config = Config::get();

    SECTION("that is unrecognized") {
        string bad_id = id + "_";
        data["roster"][user] = bad_id;
        REQUIRE_THROWS_WITH(
            config.parse(data),
            error_id_unrecognized(config.filename, user, bad_id)
        );
    }

    SECTION("that is recognized") {
        data["roster"][user] = id;
        try {
            config.parse(data);
            SUCCEED("Successfully created Roster object");
        } catch (const mucs_exception& me) {
            FAIL(me.what());
        }
    }
}


TEST_CASE("roster entry has multiple lab ids", "[roster][entry]") {
    string user = rand_string(6);
    string id = rand_string(2, chars_lower);
    auto data = new_config_data();
    data["labs"][id] = "mon 00:00:00 - 23:59:59";
    auto& config = Config::get();

    SECTION("one recognized, other unrecognized") {
        string bad_id = id + '_';
        string all_ids = id + ',' + bad_id;
        data["roster"][user] = all_ids;
        REQUIRE_THROWS_WITH(
            config.parse(data),
            error_id_unrecognized(config.filename, user, bad_id)
        );
    }

    SECTION("all recognized") {
        string all_ids = id + "," + id;
        data["roster"][user] = all_ids;
        try {
            data.get_to(Config::get());
            SUCCEED("Successfully created Roster object");
        } catch (const mucs_exception& me) {
            FAIL(me.what());
        }
    }
}


TEST_CASE("serialize roster", "[roster][serialize]") {
    string user = rand_string(6);
    string id = rand_string(2, chars_upper);
    Config::get().lab_ids = { id };
    json data = {};

    SECTION("with one id") {
        data[user] = id;
        ostringstream expected, actual;
        expected << data;
        actual << json(data.get<Roster>());
        REQUIRE_THAT(
            expected.str(),
            Equals(actual.str(), Catch::CaseSensitive::No)
        );
    }

    SECTION("with multiple ids") {
        data[user] = id + ',' + id;
        ostringstream expected, actual;
        expected << data;
        actual << json(data.get<Roster>());
        REQUIRE_THAT(
            expected.str(),
            Equals(actual.str(), Catch::CaseSensitive::No)
        );
    }
}
