#include "test_config.hpp"


TEST_CASE("config init fails when path doesn't exist",
          "[config][config-file]") {
    string fn = rand_filename();
    REQUIRE_THROWS_WITH(
        Config(Path(fn)),
        "Config file does not exist: " + fn
    );
}


TEST_CASE("config init fails when path isn't a regular file",
          "[config][config-file]") {
    REQUIRE_THROWS_WITH(
        Config(Path("/home")),
        "Config path must be a regular file: /home"
    );
}


TEST_CASE("config init fails when file has invalid json",
          "[config][config-file]") {
    string fn = rand_filename();

    MockPath mp(fn);
    mp << "data_" << rand_string();

    REQUIRE_THROWS_WITH(
        Config(mp.get()),
        StartsWith("Failed to parse config: " + fn)
    );
}


TEST_CASE("config init succeeds when file exists and has valid json",
          "[config][config-file]") {
    MockPath mp(rand_filename());
    mp << new_config_data().dump();
    REQUIRE_NOTHROW(Config(mp.get()));
}


TEST_CASE("config validation fails when schema doesn't exist",
          "[config][validation][schema-file]") {
    string fn = rand_filename();
    Config config;
    REQUIRE_THROWS_WITH(
        config.validate(Path(fn)),
        "Schema does not exist: " + fn
    );
}


TEST_CASE("config validation fails when schema isn't a regular file",
          "[config][validation][schema-file]") {
    Config config;
    REQUIRE_THROWS_WITH(
        config.validate(Path("/home")),
        "Schema path must be a regular file: /home"
    );
}


TEST_CASE("config validation fails when schema has invalid json",
          "[config][validation][schema-file]") {
    string fn = rand_filename();

    MockPath sp(fn);
    sp << "data_" << rand_string();

    Config config;
    REQUIRE_THROWS_WITH(
        config.validate(sp.get()),
        StartsWith("Failed to parse schema: " + fn)
    );
}


TEST_CASE("config validation succeeds when both config and schema exist and "
              "have valid json",
          "[config][validation]") {
    Config config(new_config_data(), rand_filename());
    REQUIRE_NOTHROW(config.validate());
}


TEST_CASE("config parsing succeeds when json is valid", "[config][parsing]") {
    json data = new_config_data();
    REQUIRE_NOTHROW(Config(data).parse());
}


TEST_CASE("serialize config", "[config][serialize]") {
    json data = new_config_data();
    string expected = data.dump();
    string actual = json(data.get<Config>()).dump();
    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}


TEST_CASE("get_assignment fails when no assignment matches the given name",
          "[config][get-assignment]") {
    auto config = Config(json::object());
    string name = rand_hw_name();
    REQUIRE_THROWS_WITH(
        config.get_assignment(name),
        "No such assignment exists: " + name
    );
}


TEST_CASE("get_assignment succeeds when there is a matching hw assignment",
          "[config][get-assignment]") {
    string expected_name = rand_hw_name();
    Config config;
    config.homeworks.emplace(expected_name, sys_seconds{});
    const auto& actual = dynamic_cast<const Hw&>(
        config.get_assignment(expected_name));
    REQUIRE(actual.name == expected_name);
}


TEST_CASE("get_assignment succeeds when there is a matching lab assignment",
          "[config][get-assignment]") {
    string expected_name = rand_lab_asgmt_name();
    Config config;
    config.lab_assignments.emplace(expected_name, year_month_day{}, get_day());
    const auto& actual = dynamic_cast<const LabAsgmt&>(
        config.get_assignment(expected_name));
    REQUIRE(actual.name == expected_name);
}
