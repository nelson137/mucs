#include "test_roster.hpp"


TEST_CASE("insert adds a new entry when the given user doesn't exist",
          "[roster][insert]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    Roster roster;

    roster.insert(user, lab_id);

    REQUIRE(roster.count(user) == 1);
    const vector<string>& labs = roster.at(user);
    REQUIRE_THAT(labs, Equals<string>({ lab_id }));
}


TEST_CASE("insert appends to an existing entry when the given user exists",
          "[roster][insert]") {
    string user = rand_user();
    string lab_id1 = rand_lab_sesh_id();
    string lab_id2 = rand_lab_sesh_id();
    Roster roster;
    roster[user] = { lab_id1 };

    roster.insert(user, lab_id2);

    REQUIRE(roster.count(user) == 1);
    const vector<string>& labs = roster.at(user);
    REQUIRE_THAT(labs, UnorderedEquals<string>({ lab_id1, lab_id2 }));
}
