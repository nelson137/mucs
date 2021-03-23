#include "test_lab_assignments.hpp"


TEST_CASE("lab-asgmt month is invalid", "[lab-asgmt]") {
    string name = rand_lab_asgmt_name();
    string m = rand_string();
    json data = { {"name", name}, {"year", 2000}, {"month", m}, {"week", 1} };
    REQUIRE_THROWS_WITH(
        data.get<LabAsgmt>(),
        "Invalid lab assignment month: " \
            "{filename}[\"lab-assignments\"][\"" + name + "\"]"
    );
}


TEST_CASE("lab-asgmt is valid", "[lab-asgmt]") {
    string name = rand_lab_asgmt_name();
    json data = { {"name",name}, {"year",2000}, {"month","Jan"}, {"week",1} };
    REQUIRE_NOTHROW(data.get<LabAsgmt>());
}


TEST_CASE("serialize lab-asgmt", "[lab-assignments][serialize]") {
    string name = rand_lab_asgmt_name();
    year_month_day start(year(2020)/February/Monday[1]);
    year_month_day end = start.year()/start.month()/(start.day() + days(6));

    string expected = json({
        {"name", name}, {"year", 2020}, {"month", "feb"}, {"week", 1}
    }).dump();

    LabAsgmt la(name, start, end);
    string actual = json(la).dump();

    REQUIRE_THAT(expected, Equals(actual, Catch::CaseSensitive::No));
}


TEST_CASE("deserialized lab-assignments entries are in sorted order",
          "[lab-assignments]") {
    json data = {
        { {"name", "lab1"}, {"year", 2020}, {"month", "May"}, {"week", 1} },
        { {"name", "lab2"}, {"year", 2020}, {"month", "Jan"}, {"week", 2} }
    };
    auto lab_assignments = data.get<LabAssignments>();
    REQUIRE(lab_assignments.begin()->name == "lab2");
}
