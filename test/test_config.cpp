#include "test_config.hpp"


TEST_CASE("config file doesn't exist", "[config][config-file]") {
    string fn = rand_filename();
    REQUIRE_THROWS_WITH(
        Config(Path(fn)),
        "Config file does not exist: " + fn
    );
}


TEST_CASE("config path is a directory", "[config][config-file]") {
    string fn = "/tmp";
    REQUIRE_THROWS_WITH(
        Config(Path(fn)),
        "Config path must be a regular file: " + fn
    );
}


TEST_CASE("config file exists and has invalid json", "[config][config-file]") {
    string fn = rand_filename();

    MockPath mp(fn);
    mp << "data_" << rand_string();

    REQUIRE_THROWS_WITH(
        Config(mp.get()),
        StartsWith("Failed to parse config: " + fn)
    );
}


TEST_CASE("config file exists and has valid json", "[config][config-file]") {
    MockPath mp(rand_filename());
    mp << new_config_data().dump();
    REQUIRE_NOTHROW(Config(mp.get()));
}


TEST_CASE("config has no key course_id", "[config][course_id]") {
    string fn = rand_filename();
    json data = new_config_data();
    data.erase("course_id");
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(MISSING_PROPERTY)
    );
}


TEST_CASE("value for key course_id has incorrect type",
          "[config][course_id]") {
    string fn = rand_filename();
    json data = new_config_data({ {"course_id", rand_int(9)} });
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(INVALID_VALUE_TYPE)
    );
}


TEST_CASE("config has no key admin_hash", "[config][admin_hash]") {
    string fn = rand_filename();
    json data = new_config_data();
    data.erase("admin_hash");
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(MISSING_PROPERTY)
    );
}


TEST_CASE("value for key admin_hash has incorrect type",
          "[config][admin_hash]") {
    string fn = rand_filename();
    json data = new_config_data({ {"admin_hash", rand_int(9)} });
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(INVALID_VALUE_TYPE)
    );
}


TEST_CASE("config has no key homeworks", "[config][homeworks]") {
    string fn = rand_filename();
    json data = new_config_data();
    data.erase("homeworks");
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(MISSING_PROPERTY)
    );
}


TEST_CASE("value for key homeworks has incorrect type", "[config][homeworks]") {
    string fn = rand_filename();
    json data = new_config_data();
    data["homeworks"] = rand_int(9);
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(INVALID_VALUE_TYPE)
    );
}


TEST_CASE("config has no key lab-sessions", "[config][lab-sessions]") {
    string fn = rand_filename();
    json data = new_config_data();
    data.erase("lab-sessions");
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(MISSING_PROPERTY)
    );
}


TEST_CASE("value for key lab-sessions has incorrect type",
          "[config][lab-sessions]") {
    string fn = rand_filename();
    json data = new_config_data({ {"lab-sessions", rand_int(9)} });
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(INVALID_VALUE_TYPE)
    );
}


TEST_CASE("config has no key lab-assignments", "[config][lab-assignments]") {
    string fn = rand_filename();
    json data = new_config_data();
    data.erase("lab-assignments");
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(MISSING_PROPERTY)
    );
}


TEST_CASE("value for key lab-assignments has incorrect type",
          "[config][lab-assignments]") {
    string fn = rand_filename();
    json data = new_config_data({ {"lab-assignments", rand_int(9)} });
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(INVALID_VALUE_TYPE)
    );
}


TEST_CASE("config has no key roster", "[config][roster]") {
    string fn = rand_filename();
    json data = new_config_data();
    data.erase("roster");
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(MISSING_PROPERTY)
    );
}


TEST_CASE("value for key roster has incorrect type", "[config][roster]") {
    string fn = rand_filename();
    json data = new_config_data({ {"roster", rand_int(9)} });
    REQUIRE_THROWS_WITH(
        Config().parse(data, fn),
        StartsWith("Invalid config: " + fn) && Contains(INVALID_VALUE_TYPE)
    );
}


TEST_CASE("config is valid", "[config]") {
    json data = new_config_data();
    REQUIRE_NOTHROW(Config().parse(data));
}


TEST_CASE("serialize config", "[config][serialize]") {
    json data = new_config_data();
    data.erase("filename");
    string expected = data.dump();
    string actual = json(data.get<Config>()).dump();
    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}
