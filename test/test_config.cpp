#include "test_config.hpp"


TEST_CASE("config file doesn't exist", "[config][config-file]") {
    string fn = rand_string();
    REQUIRE_THROWS_WITH(
        Config::get().parse_file(fn),
        "Config file does not exist: " + fn
    );
}


TEST_CASE("config path is a directory", "[config][config-file]") {
    string fn = "/tmp";
    REQUIRE_THROWS_WITH(
        Config::get().parse_file(fn),
        "Config path must be a regular file: " + fn
    );
}


TEST_CASE("config file exists and has invalid json", "[config][config-file]") {
    temp_file tf{};
    REQUIRE_THROWS_WITH(
        Config::get().parse_file(tf.name),
        "Invalid json: " + tf.name
    );
}


TEST_CASE("config file exists and has valid json", "[config][config-file]") {
    temp_file tf{};
    tf << new_config<json>().dump();
    try {
        Config::get().parse_file(tf.name);
    } catch (const mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("config has no key course_id", "[config][course_id]") {
    auto data = new_config<json>();
    data.erase("course_id");
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "course_id", "string")
    );
}


TEST_CASE("value for key course_id has incorrect type",
          "[config][course_id]") {
    auto data = new_config<json>({ {"course_id", rand_int(9)} });
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "course_id", "string")
    );
}


TEST_CASE("config has no key admin_hash", "[config][admin_hash]") {
    auto data = new_config<json>();
    data.erase("admin_hash");
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        Config::get().parse(data),
        error_prop(config.filename, "admin_hash", "string")
    );
}


TEST_CASE("value for key admin_hash has incorrect type",
          "[config][admin_hash]") {
    auto data = new_config<json>({ {"admin_hash", rand_int(9)} });
    auto& config = Config::get();
    REQUIRE_THROWS_WITH(
        config.parse(data),
        error_prop(config.filename, "admin_hash", "string")
    );
}


TEST_CASE("config is valid", "[config]") {
    auto data = new_config<json>();
    try {
        Config::get().parse(data);
        SUCCEED("Successfully created Config object");
    } catch (const mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize config", "[config][serialize]") {
    auto data = new_config<json>();
    string expected = data.dump();
    string actual = json(data.get_to(Config::get())).dump();
    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}
