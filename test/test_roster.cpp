#include "test_roster.hpp"


TEST_CASE("roster entry", "[roster]") {

    int ri = rand_int(9);
    MockCourseConfig mock_config = {
        {"course_number", ri},
        {"admin_hash", "!"},
        {"homeworks", {}},
        {"labs", { {"X", ""} }},
    };
    vector<string> lab_letters = { "X" };

    SECTION("type is invalid", "[roster][entry]") {
        string user = "test-user";
        mock_config["roster"] = { {user, ri} };
        REQUIRE_THROWS_WITH(
            Roster(mock_config, lab_letters),
            "Roster entries must be of type string: " +
                mock_config.filename + "[\"roster\"][\"" + user + "\"]"
        );
    }

    SECTION("type is valid but has unrecognized lab letter",
            "[roster][entry]") {
        string user = "test-user";
        string letter = rand_string();
        mock_config["roster"] = { {user, letter} };
        REQUIRE_THROWS_WITH(
            Roster(mock_config, lab_letters),
            "Lab session letter '" + letter + "' not recognized: " +
                mock_config.filename + "[\"roster\"][\"" + user + "\"]"
        );
    }

    SECTION("type is valid and has recognized lab letter", "[roster][entry]") {
        string user = "test-user";
        mock_config["roster"] = { {user, "X"} };
        try {
            Roster(mock_config, lab_letters);
            SUCCEED("Successfully created Homeworks object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }

}
