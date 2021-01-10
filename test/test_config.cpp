#include "test_config.hpp"


TEST_CASE("config init throws when path doesn't exist",
          "[config][config-file]") {
    string fn = rand_filename();
    REQUIRE_THROWS_WITH(
        Config(Path(fn)),
        "Config file does not exist: " + fn
    );
}


TEST_CASE("config init throws when path isn't a regular file",
          "[config][config-file]") {
    REQUIRE_THROWS_WITH(
        Config(Path("/home")),
        "Config path must be a regular file: /home"
    );
}


TEST_CASE("config init throws when file has invalid json",
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


TEST_CASE("config validation throws when schema doesn't exist",
          "[config][validation][schema-file]") {
    string fn = rand_filename();
    Config config;
    REQUIRE_THROWS_WITH(
        config.validate(Path(fn)),
        "Schema does not exist: " + fn
    );
}


TEST_CASE("config validation throws when schema isn't a regular file",
          "[config][validation][schema-file]") {
    Config config;
    REQUIRE_THROWS_WITH(
        config.validate(Path("/home")),
        "Schema path must be a regular file: /home"
    );
}


TEST_CASE("config validation throws when schema has invalid json",
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


TEST_CASE("config validation throws when config has invalid json",
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


TEST_CASE("config deserialization succeeds when json is valid",
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


TEST_CASE("load_roster throws when the given path doesn't exist",
          "[config][load-roster]") {
    string fn = rand_filename();
    Config config;
    REQUIRE_THROWS_WITH(
        config.load_roster(Path(fn)),
        "Roster directory does not exist: " + fn
    );
}


TEST_CASE("load_roster throws when the given path isn't a directory",
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


TEST_CASE("apply_overrides succeeds with 0 entries",
          "[config][apply-overrides]") {
    REQUIRE_NOTHROW(Config().apply_overrides(rand_user(), rand_hw_name()));
}


TEST_CASE("apply_overrides throws when user isn't in roster",
          "[config][apply-overrides]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();

    Config config;
    config.overrides.o_labs.emplace_back(user, lab_id);

    REQUIRE_THROWS_WITH(
        config.apply_overrides(user, ""),
        "Student not in course: " + user
    );
}


TEST_CASE("apply_overrides for lab overrides", "[config][apply-overrides]") {
    string user = rand_user();
    string orig_lab_id = rand_lab_sesh_id();
    string new_lab_id = rand_lab_sesh_id();

    string asgmt_name;
    string expected_lab_id;
    Config config;
    config.roster[user] = orig_lab_id;

    SECTION("with no assignment, correctly updates the roster") {
        asgmt_name = "";
        config.overrides.o_labs.emplace_back(user, new_lab_id);
        expected_lab_id = new_lab_id;
    }

    SECTION("with a matching assignment, correctly updates the roster") {
        asgmt_name = rand_lab_asgmt_name();
        config.overrides.o_labs.emplace_back(user, new_lab_id, asgmt_name);
        expected_lab_id = new_lab_id;
    }

    SECTION("with a non-matching assignment, does nothing") {
        asgmt_name = rand_lab_asgmt_name();
        config.overrides.o_labs.emplace_back(
            user, new_lab_id, rand_lab_asgmt_name());
        expected_lab_id = orig_lab_id;
    }

    REQUIRE(config.roster.count(user) == 1);
    REQUIRE(config.roster.at(user) == orig_lab_id);

    REQUIRE_NOTHROW(config.apply_overrides(user, asgmt_name));

    REQUIRE(config.roster.count(user) == 1);
    REQUIRE(config.roster.at(user) == expected_lab_id);
}


TEST_CASE("apply_overrides correctly overrides Hw objects",
          "[config][apply-overrides]") {
    string user = rand_user();
    string hw_name = rand_hw_name();

    Config config;
    config.homeworks.emplace(hw_name);
    config.overrides.o_homeworks.emplace_back(user, hw_name);

    REQUIRE(config.homeworks.size() == 1);
    REQUIRE(config.homeworks.begin()->is_overridden == false);

    REQUIRE_NOTHROW(config.apply_overrides(user, hw_name));

    REQUIRE(config.homeworks.size() == 1);
    REQUIRE(config.homeworks.begin()->is_overridden == true);
}


TEST_CASE("get_assignment throws when no assignment matches the given name",
          "[config][get-assignment]") {
    Config config;
    string name = rand_hw_name();
    REQUIRE_THROWS_WITH(
        config.validate_and_get_asgmt(name),
        "No such assignment exists: " + name
    );
}


TEST_CASE("validate_and_get_assignment returns the matching hw assignment",
          "[config][validate-and-get-assignment]") {
    string expected_name = rand_hw_name();
    Config config;
    config.homeworks.emplace(expected_name, sys_seconds{});
    const auto& actual = dynamic_cast<const Hw&>(
        config.validate_and_get_asgmt(expected_name));
    REQUIRE(actual.name == expected_name);
}


TEST_CASE("validate_and_get_assignment returns the matching lab assignment",
          "[config][validate-and-get-assignment]") {
    string expected_name = rand_lab_asgmt_name();
    Config config;
    config.lab_assignments.emplace(expected_name, year_month_day{}, get_day());
    const auto& actual = dynamic_cast<const LabAsgmt&>(
        config.validate_and_get_asgmt(expected_name));
    REQUIRE(actual.name == expected_name);
}


TEST_CASE("validate_and_get_lab", "[config][validate-and-get-lab]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    Config config;
    config.roster[user] = lab_id;

    SECTION("throws when the lab is invalid") {
        REQUIRE_THROWS_WITH(
            config.validate_and_get_lab(user),
            "Student '" + user + "' has invalid lab: " + lab_id
        );
    }

    SECTION("returns the LabSesh assigned to the given user") {
        auto expected_ls = RandLabSesh(lab_id).today().now(true).get();
        config.lab_sessions.insert({ lab_id, expected_ls });
        REQUIRE(config.validate_and_get_lab(user).id == lab_id);
    }
}
