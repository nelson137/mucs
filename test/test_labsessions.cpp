#include "test_labsessions.hpp"


TEST_CASE("labs", "[courseconfig][labs]") {

    string fn = rand_string();
    json data = {
        {"filename", fn},
        {"course_id", ""},
        {"admin_hash", "!"},
        {"homeworks", json::object()},
        {"roster", json::object()}
    };

    SECTION("doesn't exist", "[courseconfig][labs]") {
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_missing_prop(fn, "labs", "object")
        );
    }

    SECTION("has incorrect type", "[courseconfig][labs]") {
        data["labs"] = rand_int(9);
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_incorrect_type(fn, "labs", "object")
        );
    }

    data["labs"] = json::object();

    SECTION("entry has incorrect type", "[courseconfig][labs]") {
        data["labs"] = rand_int(9);
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_incorrect_type(fn, "labs", "object")
        );
    }

    SECTION("type is invalid", "[courseconfig][labs][entry]") {
        string key = rand_string(1, chars_upper);
        data["labs"][key] = rand_int(9);
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            "Lab entries must be of type string: " + fn + "[\"labs\"][\"" +
                key + "\"]"
        );
    }

    SECTION("type is valid", "[courseconfig][labs][entry]") {
        string key = rand_string(1, chars_upper);
        data["labs"][key] = "";
        try {
            data.get<CourseConfig>();
            SUCCEED("Successfully created LabSessions object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }

}
