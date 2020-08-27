#include "test_lab_assignments.hpp"


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
    json data = { {key, "Feb 1"} };
    REQUIRE_NOTHROW(data.get<LabAssignments>());
}


TEST_CASE("serialize lab-assignments", "[lab-assignments][serialize]") {
    string name = rand_lab_asgmt_name();
    year_month_day start(year(2020)/February/Monday[1]);
    year_month_day end = start.year()/start.month()/(start.day() + days(6));

    string expected = json({ {name, "Feb 1"} }).dump();

    LabAssignments las;
    las.insert({ name, LabAsgmt(name, start, end) });
    string actual = json(las).dump();

    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}


TEST_CASE("deserialized lab-assignments entries are in sorted order",
          "[lab-assignments]") {
    json data = {
        {"lab1", "May 1"},
        {"lab2", "Jan 2"}
    };
    auto lab_assignments = data.get<LabAssignments>();
    REQUIRE(lab_assignments.begin()->first == "lab2");
}
