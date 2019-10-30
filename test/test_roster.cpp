#include "test_roster.hpp"


TEST_CASE("roster", "[courseconfig][roster]") {

    string fn = rand_string();
    string user = rand_string(6);
    string good_id = rand_string(2);
    string good_id_upper = good_id;
    transform(good_id.begin(), good_id.end(), good_id.begin(), ::toupper);

    json data = {
        {"filename", fn},
        {"course_id", ""},
        {"admin_hash", "!"},
        {"homeworks", json::object()},
        {"labs", { {good_id, ""} }},
    };

    SECTION("doesn't exist", "[courseconfig][roster]") {
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_missing_prop(fn, "roster", "object")
        );
    }

    SECTION("has incorrect type", "[courseconfig][roster]") {
        data["roster"] = rand_int(9);
        REQUIRE_THROWS_WITH(
            data.get<CourseConfig>(),
            error_incorrect_type(fn, "roster", "object")
        );
    }

    data["roster"] = json::object();

    SECTION("has one lab id", "[courseconfig][roster][entry]") {
        SECTION("that is unrecognized", "[courseconfig][roster][entry]") {
            string bad_id = good_id + "_";
            data["roster"][user] = bad_id;
            REQUIRE_THROWS_WITH(
                data.get<CourseConfig>(),
                error_id_unrecognized(fn, user, bad_id)
            );
        }

        SECTION("that is recognized", "[courseconfig][roster][entry]") {
            data["roster"][user] = good_id;
            try {
                data.get<CourseConfig>();
                SUCCEED("Successfully created Roster object");
            } catch (mucs_exception& me) {
                FAIL(me.what());
            }
        }
    }

    SECTION("has multiple lab ids", "[courseconfig][roster][entry]") {
        SECTION("one unrecognized", "[courseconfig][roster][entry]") {
            string bad_id = good_id + "_";
            string all_ids = good_id + "," + bad_id;
            data["roster"][user] = all_ids;
            REQUIRE_THROWS_WITH(
                data.get<CourseConfig>(),
                error_id_unrecognized(fn, user, bad_id)
            );
        }

        SECTION("all recognized", "[courseconfig][roster][entry]") {
            string all_ids = good_id + "," + good_id;
            data["roster"][user] = all_ids;
            try {
                data.get<CourseConfig>();
                SUCCEED("successfully created Roster object");
            } catch (mucs_exception& me) {
                FAIL(me.what());
            }
        }
    }

}
