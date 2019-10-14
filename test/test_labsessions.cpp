#include "test_labsessions.hpp"


TEST_CASE("labsession entry", "[labs]") {

    int ri = rand_int(9);
    MockCourseConfig mock_config = {
        {"course_id", ri},
        {"admin_hash", "!"},
        {"homeworks", {}},
        {"roster", {}}
    };

    SECTION("type is invalid", "[labs][entry]") {
        string key = "A";
        mock_config["labs"] = { {key, ri} };
        REQUIRE_THROWS_WITH(
            LabSessions(mock_config),
            "Lab entries must be of type string: " +
                mock_config.filename + "[\"labs\"][\"" + key + "\"]"
        );
    }

    SECTION("type is valid", "[labs][entry]") {
        string key = "A";
        mock_config["labs"] = { {key, ""} };
        try {
            LabSessions(mock_config);
            SUCCEED("Successfully created LabSessions object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }

}
