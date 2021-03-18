#include "test_roster.hpp"


TEST_CASE("safe_insert adds an entry when the user doesn't have an entry yet",
          "[roster][safe-insert]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    Roster roster;

    roster.safe_insert(user, lab_id);

    REQUIRE(roster.size() == 1);
    REQUIRE(roster.count(user) == 1);
    REQUIRE(roster.at(user) == lab_id);
}


TEST_CASE("safe_insert throws when there is already an entry for the user",
          "[roster][safe-insert]") {
    string user = rand_user();
    string lab_id1 = rand_lab_sesh_id();
    string lab_id2 = rand_lab_sesh_id();
    Roster roster;

    REQUIRE_NOTHROW(roster.safe_insert(user, lab_id1));
    REQUIRE_THROWS_WITH(
        roster.safe_insert(user, lab_id2),
        "Student cannot be in lab sessions '" + lab_id1 + "' and '" + lab_id2
            + "': " + user
    );
}
