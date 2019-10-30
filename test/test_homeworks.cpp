#include "test_homeworks.hpp"


TEST_CASE("homeworks", "[homeworks]") {

    string fn = rand_string();
    json data = {
        {"filename", fn},
        {"course_id", ""},
        {"admin_hash", "!"},
        {"labs", json::object()},
        {"roster", json::object()}
    };

    SECTION("doesn't exist", "[courseconfig][homeworks]") {
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_missing_prop(fn, "homeworks", "object")
        );
    }

    SECTION("has incorrect type", "[courseconfig][homeworks]") {
        data["homeworks"] = rand_int(9);
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_incorrect_type(fn, "homeworks", "object")
        );
    }

    data["homeworks"] = json::object();

    SECTION("entry has incorrect type", "[courseconfig][homeworks][entry]") {
        string key = "hw" + to_string(rand_int(9));
        data["homeworks"][key] = rand_int(9);
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            "Homework entries must be of type string: " + fn +
                "[\"homeworks\"][\"" + key + "\"]"
        );
    }

    SECTION("is valid", "[courseconfig][homeworks][entry]") {
        string key = "hw" + to_string(rand_int(9));
        data["homeworks"][key] = rand_string();
        try {
            data.get<CourseConfig>();
            SUCCEED("Successfully created Homeworks object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }

}
