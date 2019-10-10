#include "test_configs.hpp"


TEST_CASE("config directory doesn't exist", "[configs]") {
    string p = rand_string();
    REQUIRE_THROWS_WITH(
        Configs(p),
        "Config directory does not exist: " + p
    );
}


TEST_CASE("config directory exists but isn't directory", "[configs]") {
    temp_file tf{};
    REQUIRE_THROWS_WITH(
        Configs(tf.name),
        "Config path must be a directory: " + tf.name
    );
}


TEST_CASE("config directory", "[configs]") {
    temp_dir td{};

    SECTION("has no files") {
        REQUIRE_THROWS_WITH(
            Configs(td.name),
            "Directory contains no json files: " + td.name
        );
    }

    SECTION("has 1 file") {
        temp_file tf{td.name, "config.json"};

        SECTION("with invalid json") {
            REQUIRE_THROWS_WITH(
                Configs(td.name),
                "Invalid json: " + tf.name
            );
        }

        SECTION("with valid json") {
            tf.write(
                "{"
                    "\"course_number\":\"1050\","
                    "\"admin_hash\":\"!\","
                    "\"homeworks\":{"
                        "\"hw1\":\"\""
                    "},"
                    "\"labs\":{"
                        "\"x\":\"\""
                    "},"
                    "\"roster\":{"
                        "\"nelson\":\"x\""
                    "}"
                "}\n"
            );
            try{
                Configs(td.name);
                SUCCEED("Sucessfully created Configs object");
            } catch (mucs_exception& me) {
                FAIL(me.what());
            }
        }
    }
}
