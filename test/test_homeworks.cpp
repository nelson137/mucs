#include "test_homeworks.hpp"


TEST_CASE("entry", "[homeworks]") {
    int ri = rand_int(9);
    MockCourseConfig mock_config = {
        {"course_number", ri},
        {"admin_hash", "!"},
        {"labs", {}},
        {"roster", {}}
    };

    SECTION("has invalid type", "[homeworks][entry]") {
        string key = "hw1";
        mock_config["homeworks"] = { {key, ri} };
        REQUIRE_THROWS_WITH(
            Homeworks(mock_config),
            "Homework entries must be of type string: " +
                mock_config.filename + "[\"homeworks\"][\"" + key + "\"]"
        );
    }

    SECTION("has valid type", "[homeworks][entry]") {
        string key = "hw1";
        mock_config["homeworks"] = { {key, ""} };
        try {
            Homeworks h(mock_config);
            SUCCEED("Successfully created Homeworks object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }
}
