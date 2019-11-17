#include "test_config.hpp"


string error_prop(const string& fn, const string& k, const string& t) {
    return "Config requires key \"" + k + "\" with type " + t + ": " + fn;
}


string error_id_unrecognized(
    const string& fn,
    const string& user,
    const string& id
) {
    return "Lab id not recognized: " + fn + "[\"roster\"][\"" + user + "\"]";
}


json new_config_data() {
    return {
        {"filename", rand_string(4)},
        {"course_id", rand_string()},
        {"admin_hash", "!"},
        {"homeworks", json::object()},
        {"labs", json::object()},
        {"current_lab", ""},
        {"roster", json::object()}
    };
}


TEST_CASE("config file doesn't exist", "[config][config-file]") {
    string fn = rand_string();
    REQUIRE_THROWS_WITH(
        Config(fn),
        "Config file does not exist: " + fn
    );
}


TEST_CASE("config path is a directory", "[config][config-file]") {
    string fn = "/tmp";
    REQUIRE_THROWS_WITH(
        Config(fn),
        "Config path must be a regular file: " + fn
    );
}


TEST_CASE("config file exists and has invalid json", "[config][config-file]") {
    temp_file tf{};
    REQUIRE_THROWS_WITH(
        Config(tf.name),
        "Invalid json: " + tf.name
    );
}


TEST_CASE("config file exists and has valid json", "[config][config-file]") {
    ostringstream data;
    data << new_config_data();
    temp_file tf{};
    tf << data.str();
    try {
        Config(tf.name);
    } catch (mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("config has no key course_id", "[config][course_id]") {
    auto data = new_config_data();
    data.erase("course_id");
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "course_id", "string")
    );
}


TEST_CASE("value for key course_id has incorrect type",
          "[config][course_id]") {
    auto data = new_config_data();
    data["course_id"] = rand_int(9);
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "course_id", "string")
    );
}


TEST_CASE("config has no key admin_hash", "[config][admin_hash]") {
    auto data = new_config_data();
    data.erase("admin_hash");
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "admin_hash", "string")
    );
}


TEST_CASE("value for key admin_hash has incorrect type",
          "[config][admin_hash]") {
    auto data = new_config_data();
    data["admin_hash"] = rand_int(9);
    REQUIRE_THROWS_WITH(
        data.get<Config>(),
        error_prop(data["filename"], "admin_hash", "string")
    );
}


TEST_CASE("config is valid", "[config]") {
    auto data = new_config_data();
    try {
        data.get<Config>();
        SUCCEED("Successfully created Config object");
    } catch (mucs_exception& me) {
        FAIL(me.what());
    }
}


TEST_CASE("serialize config", "[config][serialize]") {
    auto data = new_config_data();
    ostringstream expected, actual;
    expected << data;
    actual << json(data.get<Config>());
    REQUIRE_THAT(
        expected.str(),
        Equals(actual.str(), Catch::CaseSensitive::No)
    );
}
