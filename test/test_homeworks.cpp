#include "test_homeworks.hpp"


TEST_CASE("homework entry", "[homeworks]") {

    int ri = rand_int(9);
    MockCourseConfig mock_config = {
        {"course_id", ri},
        {"admin_hash", "!"},
        {"labs", {}},
        {"roster", {}}
    };

    SECTION("type is invalid", "[homeworks][entry]") {
        string key = "hw1";
        mock_config["homeworks"] = { {key, ri} };
        REQUIRE_THROWS_WITH(
            Homeworks(mock_config),
            "Homework entries must be of type string: " +
                mock_config.filename + "[\"homeworks\"][\"" + key + "\"]"
        );
    }

    SECTION("type is valid", "[homeworks][entry]") {
        string key = "hw1";
        mock_config["homeworks"] = { {key, ""} };
        try {
            Homeworks(mock_config);
            SUCCEED("Successfully created Homeworks object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }

}
