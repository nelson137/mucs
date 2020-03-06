#include "test_roster.hpp"


TEST_CASE("config has no key roster", "[config][roster]") {
    auto data = new_config<json>();
    data.erase("roster");
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "roster", "object")
    );
}


TEST_CASE("value for key roster has incorrect type", "[config][roster]") {
    auto data = new_config<json>({ {"roster", rand_int(9)} });
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "roster", "object")
    );
}


TEST_CASE("roster entry has incorrect type", "[roster][entry]") {
    auto& config = new_config<Config&>();
    string user = rand_string(6);
    json data = { {user, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<Roster>(),
        "Roster entries must be of type string: " +
            config.filename + "[\"roster\"][\"" + user + "\"]"
    );
}


TEST_CASE("roster entry has one lab id", "[roster][entry]") {
    auto& config = Config::get();
    string user = rand_string(6);
    string id = rand_string(2, chars_upper);
    config.lab_ids = { id };
    json data = json::object();

    SECTION("that is unrecognized") {
        string bad_id = id + "_";
        data[user] = bad_id;
        REQUIRE_THROWS_WITH(
            data.get<Roster>(),
            error_id_unrecognized(config.filename, user, bad_id)
        );
    }

    SECTION("that is recognized") {
        data[user] = id;
        REQUIRE_NOTHROW(data.get<Roster>());
    }
}


TEST_CASE("roster entry has multiple lab ids", "[roster][entry]") {
    auto& config = Config::get();
    string user = rand_string(6);
    string id = rand_string(2, chars_upper);
    config.lab_ids = { id };
    json data = json::object();

    SECTION("one recognized, other unrecognized") {
        string bad_id = id + '_';
        data[user] = id + ',' + bad_id;
        REQUIRE_THROWS_WITH(
            data.get<Roster>(),
            error_id_unrecognized(config.filename, user, bad_id)
        );
    }

    SECTION("all recognized") {
        data[user] = id + "," + id;
        REQUIRE_NOTHROW(data.get<Roster>());
    }
}


TEST_CASE("serialize roster", "[roster][serialize]") {
    string user = rand_string(6);
    string id = rand_string(2, chars_upper);
    Config::get().lab_ids = { id };
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
