#include "test_courseconfig.hpp"


TEST_CASE("courseconfig", "[courseconfig]") {

    string fn = rand_string();
    json data;

    auto error_missing_prop = [&fn](string k, string t) {
        return "Config must specify a '" + k + "' " + t + ": " + fn;
    };
    auto error_incorrect_type = [&fn](string k, string t) {
        return "Config expected type '" + t + "' for key \"" + k + "\": " + fn;
    };
    int ri = rand_int(10);
    string rs = rand_string();

    SECTION("has no course_id", "[courseconfig][course_id]") {
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_missing_prop("course_id", "string")
        );
    }

    SECTION("has a course_id with incorrect type",
            "[courseconfig][course_id]") {
        data["course_id"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_incorrect_type("course_id", "string")
        );
    }

    SECTION("has no admin_hash", "[courseconfig][admin_hash]") {
        data["course_id"] = rs;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_missing_prop("admin_hash", "string")
        );
    }

    SECTION("has an admin_hash with incorrect type",
            "[courseconfig][admin_hash]") {
        data["course_id"] = rs;
        data["admin_hash"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_incorrect_type("admin_hash", "string")
        );
    }

    SECTION("has no homeworks", "[courseconfig][homeworks]") {
        data["course_id"] = rs;
        data["admin_hash"] = rs;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_missing_prop("homeworks", "object")
        );
    }

    SECTION("has a homeworks with incorrect type",
            "[courseconfig][homeworks]") {
        data["course_id"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_incorrect_type("homeworks", "object")
        );
    }

    SECTION("has no labs", "[courseconfig][labs]") {
        data["course_id"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_missing_prop("labs", "object")
        );
    }

    SECTION("has a labs with incorrect type", "[courseconfig][labs]") {
        data["course_id"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        data["labs"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_incorrect_type("labs", "object")
        );
    }

    SECTION("has no roster", "[courseconfig][roster]") {
        data["course_id"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        data["labs"] = json({});
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_missing_prop("roster", "object")
        );
    }

    SECTION("has a roster with incorrect type", "[courseconfig][roster]") {
        data["course_id"] = rs;
        data["admin_hash"] = rs;
        data["homeworks"] = json({});
        data["labs"] = json({});
        data["roster"] = ri;
        REQUIRE_THROWS_WITH(
            CourseConfig(fn, data),
            error_incorrect_type("roster", "object")
        );
    }

    SECTION("is valid", "[courseconfig]") {
        data["course_id"] = rs;
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
