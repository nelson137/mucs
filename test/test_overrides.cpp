#include "test_overrides.hpp"


TEST_CASE("asgmt_matches returns true with no overridden assignment name",
          "[olab][asgmt-matches]") {
    REQUIRE(OLab().asgmt_matches(rand_hw_name()) == true);
}


TEST_CASE("asgmt_matches returns", "[olab][asgmt-matches]") {
    string override_name = rand_hw_name();
    string name;
    bool matches_ret;

    SECTION("true when the given assignment name matches") {
        name = override_name;
        matches_ret = true;
    }

    SECTION("false when the given assignment name doesn't match") {
        name = rand_hw_name();
        matches_ret = false;
    }

    OLab o_lab(rand_user(), rand_lab_sesh_id(), override_name);
    REQUIRE(o_lab.asgmt_matches(name) == matches_ret);
}


TEST_CASE("serialize OLab", "[olab][serialize]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    string overridden_name = rand_hw_name();
    OLab o_lab;
    json expected = { {"user", user}, {"session", lab_id} };

    SECTION("with no overridden assignment name") {
        o_lab = OLab(user, lab_id);
    }

    SECTION("with an overridden assignment name") {
        o_lab = OLab(user, lab_id, overridden_name);
        expected["assignment"] = overridden_name;
    }

    REQUIRE(json(o_lab) == expected);
}


TEST_CASE("serialize OHomeworks", "[ohomeworks][serialize]") {
    string user = rand_user();
    string name = rand_hw_name();
    OHomework o_hw(user, name);
    json expected = { {"user", user}, {"name", name} };
    REQUIRE(json(o_hw) == expected);
}


TEST_CASE("serialize Overrides", "[overrides][serialize]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    string hw_name = rand_hw_name();

    Overrides overrides;
    overrides.o_labs.emplace_back(user, lab_id);
    overrides.o_homeworks.emplace_back(user, hw_name);

    json expected = {
        {"labs", {
            { {"user", user}, {"session", lab_id} }
        }},
        {"homeworks", {
            { {"user", user}, {"name", hw_name} }
        }}
    };

    REQUIRE(json(overrides) == expected);
}
