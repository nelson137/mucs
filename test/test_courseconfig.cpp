#include "test_courseconfig.hpp"


string error_missing_prop(const string& fn, const string& k, const string& t) {
    return "Config must specify a '" + k + "' " + t + ": " + fn;
}


string error_incorrect_type(
    const string& fn,
    const string& k,
    const string& t
) {
    return "Config expected type '" + t + "' for key \"" + k + "\": " + fn;
}


string error_id_unrecognized(
    const string& fn,
    const string& user,
    const string& id
) {
    return "Lab id '" + id + "' not recognized: " + fn + "[\"roster\"][\"" +
        user + "\"]";
}


TEST_CASE("config file doesn't exist", "[courseconfig]") {
    string fn = rand_string();
    REQUIRE_THROWS_WITH(
        CourseConfig(fn),
        "Config file does not exist: " + fn
    );
}


TEST_CASE("config path is a directory", "[courseconfig]") {
    string fn = "/tmp";
    REQUIRE_THROWS_WITH(
        CourseConfig(fn),
        "Config path must be a regular file: " + fn
    );
}


TEST_CASE("config file exists and has invalid json", "[courseconfig]") {
    temp_file tf{};
    REQUIRE_THROWS_WITH(
        CourseConfig(tf.name),
        "Invalid json: " + tf.name
    );
}


TEST_CASE("courseconfig", "[courseconfig]") {

    string fn = rand_string(4);
    json data = {
        {"filename", fn},
        {"homeworks", json::object()},
        {"labs", json::object()},
        {"roster", json::object()}
    };

    SECTION("has no course_id", "[courseconfig][course_id]") {
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_missing_prop(fn, "course_id", "string")
        );
    }

    SECTION("has a course_id with incorrect type",
            "[courseconfig][course_id]") {
        data["course_id"] = rand_int(9);
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_incorrect_type(fn, "course_id", "string")
        );
    }

    data["course_id"] = rand_string();

    SECTION("has no admin_hash", "[courseconfig][admin_hash]") {
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_missing_prop(fn, "admin_hash", "string")
        );
    }

    SECTION("has an admin_hash with incorrect type",
            "[courseconfig][admin_hash]") {
        data["admin_hash"] = rand_int(9);
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_incorrect_type(fn, "admin_hash", "string")
        );
    }

    data["admin_hash"] = "!";

    SECTION("is valid", "[courseconfig]") {
        try {
            data.get<CourseConfig>();
            SUCCEED("Successfully created CourseConfig object");
        } catch (mucs_exception& me) {
            FAIL(me.what());
        }
    }

}
