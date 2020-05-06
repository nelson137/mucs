#include "test_lab_assignments.hpp"


TEST_CASE("lab-assignments entry has incorrect type",
          "[lab-assignments][entry]") {
    string key = rand_lab_asgmt_name();
    json data = { {key, rand_int(9)} };
    REQUIRE_THROWS_WITH(
        data.get<LabAssignments>(),
        "Lab assignments must be of type string: " \
            "{filename}[\"lab-assignments\"][\"" + key + "\"]"
    );
}


TEST_CASE("lab-assignments entry has incorrect format",
          "[lab-assignments][entry]") {
    string key = rand_lab_asgmt_name();
    json data = { {key, key} };
    REQUIRE_THROWS_WITH(
        data.get<LabAssignments>(),
        "Lab assignments must be in the format \"<month> <week-of-month>\": " \
            "{filename}[\"lab-assignments\"][\"" + key + "\"]"
    );
}


TEST_CASE("lab-assignments is valid", "[lab-assignments][entry]") {
    string key = rand_lab_asgmt_name();
    json data = { {key, "feb 1"} };
    REQUIRE_NOTHROW(data.get<LabAssignments>());
}


TEST_CASE("serialize lab-assignments", "[lab-assignments][serialize]") {
    string key = rand_lab_asgmt_name();
    json data = { {key, "feb 1"} };
    string expected = data.dump();
    string actual = json(data.get<LabAssignments>()).dump();
    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}
