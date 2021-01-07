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


TEST_CASE("config validation fails when config has invalid json",
          "[config][validation]") {
    string fn = rand_filename();
    Config config(json::array(), fn);
    REQUIRE_THROWS_WITH(
        config.validate(),
        StartsWith("Invalid config: " + fn)
    );
}


TEST_CASE("config validation succeeds when both config and schema exist and "
              "have valid json",
          "[config][validation]") {
    Config config(new_config_data(), rand_filename());
    REQUIRE_NOTHROW(config.validate());
}


TEST_CASE("config parsing succeeds when json is valid",
          "[config][deserialize]") {
    json data = new_config_data();
    REQUIRE_NOTHROW(Config(data).deserialize());
}


TEST_CASE("serialize config", "[config][serialize]") {
    json data = new_config_data();
    string expected = data.dump();
    string actual = json(data.get<Config>()).dump();
    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}


TEST_CASE("load_roster fails when the given path doesn't exist",
          "[config][load-roster]") {
    string fn = rand_filename();
    Config config;
    REQUIRE_THROWS_WITH(
        config.load_roster(Path(fn)),
        "Roster directory does not exist: " + fn
    );
}


TEST_CASE("load_roster fails when the given path isn't a directory",
          "[config][load-roster]") {
    Config config;
    REQUIRE_THROWS_WITH(
        config.load_roster(Path("/usr/bin/sudo")),
        "Roster path must be a directory: /usr/bin/sudo"
    );
}


TEST_CASE("load_roster loads the files of the given directory",
          "[config][load-roster]") {
    Config config;
    config.load_roster(Path(ROSTER_DIR));

    const auto& r = config.roster;
    REQUIRE(r.size() == 4);
    for (const string& user : vector<string>{ "a1", "a2" }) {
        REQUIRE(r.count(user) == 1);
        REQUIRE(r.at(user) == "A");
    }
    for (const string& user : vector<string>{ "b1", "b2" }) {
        REQUIRE(r.count(user) == 1);
        REQUIRE(r.at(user) == "B");
    }
}


TEST_CASE("get_assignment fails when no assignment matches the given name",
          "[config][get-assignment]") {
    auto config = Config(json::object());
    string name = rand_hw_name();
    REQUIRE_THROWS_WITH(
        config.validate_and_get_asgmt(name),
        "No such assignment exists: " + name
    );
}


TEST_CASE("get_assignment succeeds when there is a matching hw assignment",
          "[config][get-assignment]") {
    string expected_name = rand_hw_name();
    Config config;
    config.homeworks.emplace(expected_name, sys_seconds{});
    const auto& actual = dynamic_cast<const Hw&>(
        config.validate_and_get_asgmt(expected_name));
    REQUIRE(actual.name == expected_name);
}


TEST_CASE("get_assignment succeeds when there is a matching lab assignment",
          "[config][get-assignment]") {
    string expected_name = rand_lab_asgmt_name();
    Config config;
    config.lab_assignments.emplace(expected_name, year_month_day{}, get_day());
    const auto& actual = dynamic_cast<const LabAsgmt&>(
        config.validate_and_get_asgmt(expected_name));
    REQUIRE(actual.name == expected_name);
}


TEST_CASE("validate_and_get_lab",
          "[config][validate-and-get-lab]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    Config config;
    config.roster[user] = lab_id;

    SECTION("fails when the lab isn't in session") {
        auto ls = RandLabSesh(lab_id).today().now(false).get();
        config.lab_sessions.insert({ lab_id, ls });
        REQUIRE_THROWS_WITH(
            config.validate_and_get_lab(user),
            ls.format(
                "Lab {id} is not in session: {weekday} from {start} to {end}")
        );
    }

    SECTION("succeeds when the lab is in session") {
        auto expected_ls = RandLabSesh(lab_id).today().now(true).get();
        config.lab_sessions.insert({ lab_id, expected_ls });
        REQUIRE(config.validate_and_get_lab(user).id == lab_id);
    }
}
