#include "test_roster.hpp"


TEST_CASE("roster entry", "[roster]") {

    string user = rand_string(6);
    string good_id = rand_string(2);
    string good_id_upper = good_id;
    transform(good_id.begin(), good_id.end(), good_id.begin(), ::toupper);
    MockCourseConfig mock_config = {
        {"course_number", ""},
        {"admin_hash", "!"},
        {"homeworks", {}},
        {"labs", { {good_id, ""} }},
    };
    vector<string> lab_letters = { good_id };

    SECTION("type is invalid", "[roster][entry]") {
        mock_config["roster"] = { {user, rand_int(9)} };
        REQUIRE_THROWS_WITH(
            Roster(mock_config, lab_letters),
            "Roster entries must be of type string: " +
                mock_config.filename + "[\"roster\"][\"" + user + "\"]"
        );
    }

    auto error_id_unrecognized = [&mock_config,&user](string id) {
        return "Lab session letter '" + id + "' not recognized: " +
            mock_config.filename + "[\"roster\"][\"" + user + "\"]";
    };

    SECTION("has one lab letter", "[roster][entry]") {
        SECTION("that is unrecognized", "[roster][entry]") {
            string bad_id = good_id + "_";
            mock_config["roster"] = { {user, bad_id} };
            REQUIRE_THROWS_WITH(
                Roster(mock_config, lab_letters),
                error_id_unrecognized(bad_id)
            );
        }

        SECTION("that is recognized", "[roster][entry]") {
            mock_config["roster"] = { {user, good_id} };
            try {
                Roster(mock_config, lab_letters);
                SUCCEED("Successfully created Roster object");
            } catch (mucs_exception& me) {
                FAIL(me.what());
            }
        }
    }

    SECTION("has multiple lab letters", "[roster][entry]") {
        SECTION("one unrecognized", "[roster][entry]") {
            string bad_id = good_id + "_";
            string all_ids = good_id + "," + bad_id;
            mock_config["roster"] = { {user, all_ids} };
            REQUIRE_THROWS_WITH(
                Roster(mock_config, lab_letters),
                error_id_unrecognized(bad_id)
            );
        }

        SECTION("all recognized", "[roster][entry]") {
            string all_ids = good_id + "," + good_id;
            mock_config["roster"] = { {user, all_ids} };
            try {
                Roster(mock_config, lab_letters);
                SUCCEED("successfully created Roster object");
            } catch (mucs_exception& me) {
                FAIL(me.what());
            }
        }
    }

}
