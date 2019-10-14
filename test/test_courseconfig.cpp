#include "test_courseconfig.hpp"


TEST_CASE("courseconfig", "[courseconfig]") {

    string fn = rand_string();
    json data;

    auto missing_prop = [&fn](string k, string t) {
        return "Config must specify a '" + k + "' " + t + ": " + fn;
    };
    auto incorrect_type = [&fn](string k, string t) {
        return "Config expected type '" + t + "' for key \"" + k + "\": " + fn;
    };
    int ri = rand_int(10);
    string rs = rand_string();

    SECTION("has no course_number", "[courseconfig][course_number]") {
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            missing_prop("course_number", "string")
        );
    }

    SECTION("has a course_number with incorrect type",
            "[courseconfig][course_number]") {
        data["course_number"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            incorrect_type("course_number", "string")
        );
    }

    SECTION("has no admin_hash", "[courseconfig][admin_hash]") {
        data["course_number"] = rs;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            missing_prop("admin_hash", "string")
        );
    }

    SECTION("has an admin_hash with incorrect type",
            "[courseconfig][admin_hash]") {
        data["course_number"] = rs;
        data["admin_hash"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            incorrect_type("admin_hash", "string")
        );
    }

    SECTION("has no homeworks", "[courseconfig][homeworks]") {
        data["course_number"] = rs;
        data["admin_hash"] = rs;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            missing_prop("homeworks", "object")
        );
    }

    SECTION("has a homeworks with incorrect type",
            "[courseconfig][homeworks]") {
        data["course_number"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            incorrect_type("homeworks", "object")
        );
    }

    SECTION("has no labs", "[courseconfig][labs]") {
        data["course_number"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            missing_prop("labs", "object")
        );
    }

    SECTION("has a labs with incorrect type", "[courseconfig][labs]") {
        data["course_number"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        data["labs"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            incorrect_type("labs", "object")
        );
    }

    SECTION("has no roster", "[courseconfig][roster]") {
        data["course_number"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        data["labs"] = json({});
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            missing_prop("roster", "object")
        );
    }

    SECTION("has a roster with incorrect type", "[courseconfig][roster]") {
        data["course_number"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        data["labs"] = json({});
        data["roster"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            incorrect_type("roster", "object")
        );
    }

    SECTION("is valid", "[courseconfig]") {
        data["course_number"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        data["labs"] = json({});
        data["roster"] = json({});
        try {
            CourseConfig(fn, data);
            SUCCEED("Successfully created CourseConfig object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }

}
