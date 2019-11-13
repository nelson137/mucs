#include "test_roster.hpp"


TEST_CASE("config has no key roster", "[config][roster]") {
    auto data = new_config_data();
    data.erase("roster");
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "roster", "object")
    );
}


TEST_CASE("value for key roster has incorrect type", "[config][roster]") {
    auto data = new_config_data();
    data["roster"] = rand_int(9);
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "roster", "object")
    );
}


TEST_CASE("roster entry has one lab id", "[config][roster][entry]") {
    auto data = new_config_data();
    string fn = data["filename"].get<string>();
    string user = rand_string(6);

    string id = rand_string(2, chars_lower);
    data["labs"][id] = "mon 00:00:00 - 23:59:59";

    SECTION("that is unrecognized") {
        string bad_id = id + "_";
        data["roster"][user] = bad_id;
        REQUIRE_THROWS_WITH(
            data.get<Config>(),
            error_id_unrecognized(fn, user, bad_id)
        );
    }

    SECTION("that is recognized") {
        data["roster"][user] = id;
        try {
            data.get<Config>();
            SUCCEED("Successfully created Roster object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }
}


TEST_CASE("roster entry has multiple lab ids", "[config][roster][entry]") {
    auto data = new_config_data();
    string fn = data["filename"].get<string>();
    string user = rand_string(6);

    string id = rand_string(2, chars_lower);
    data["labs"][id] = "mon 00:00:00 - 23:59:59";

    SECTION("one recognized, other unrecognized", "[config][roster][entry]") {
        string bad_id = id + '_';
        string all_ids = id + ',' + bad_id;
        data["roster"][user] = all_ids;
        REQUIRE_THROWS_WITH(
            data.get<Config>(),
            error_id_unrecognized(fn, user, bad_id)
        );
    }

    SECTION("all recognized", "[config][roster][entry]") {
        string all_ids = id + "," + id;
        data["roster"][user] = all_ids;
        try {
            data.get<Config>();
            SUCCEED("Successfully created Roster object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }
}
