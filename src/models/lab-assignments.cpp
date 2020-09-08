#include "config.hpp"


LabAsgmt::LabAsgmt() {
}


LabAsgmt::LabAsgmt(const string& n) : name(n) {
    // Normalize name (lowercase)
    stl_transform(this->name, ::tolower);
}


LabAsgmt::LabAsgmt(const string& n, year_month_day s, year_month_day e)
        : name(n), start(s), end(e) {
}


bool LabAsgmt::compare::operator()(const LabAsgmt& a, const LabAsgmt& b) const {
    // Sort `a` before `b` iff:
    //   a starts and ends then b starts and ends
    //   overlapping but offset, a starts first
    //   b is inside of a
    // Otherwise sort `b` before `a`
    if (a.start < b.start)
        return true;
    else if (a.start == b.start)
        return a.end < b.end;
    else
        return false;
}


string LabAsgmt::week_str() const {
    ostringstream spec;
    spec << format("%b", this->start)
         << " "
         << year_month_weekday(this->start).index();
    return spec.str();
}


list<vector<string>> LabAssignments::to_table() const {
    list<vector<string>> table;
    for (auto it=this->begin(); it!=this->end(); it++)
        table.push_back({ it->week_str() });
    return table;
}


void from_json(const json& j, LabAsgmt& la) {
    string name = j.value("name", "");
    la.name = name;
    stl_transform(la.name, ::tolower);

    auto invalid_lab_asgmt = [&] () {
        throw Config::error(
            "Invalid lab assignment week",
            {"lab-assignments", name});
    };

    month_day md;
    istringstream(j.value("week", "")) >> parse(LAB_ASGMT_FMT, md);
    if (not md.ok())
        invalid_lab_asgmt();

    auto week = (unsigned) md.day();
    auto today = get_day();

    la.start = year_month_day(today.year()/md.month()/Monday[week]);
    if (not la.start.ok())
        invalid_lab_asgmt();

    la.end = year_month_day(sys_days(la.start) + days(6));
}


void from_json(const json& j, LabAssignments& lab_assignments) {
    for (const json& la : j)
        lab_assignments.insert(la.get<LabAsgmt>());
}


void to_json(json& j, const LabAsgmt& la) {
    j = {
        {"name", la.name},
        {"week", la.week_str()}
    };
}


void to_json(json& j, const LabAssignments& lab_assignments) {
    j = json::array();
    for (auto& la : lab_assignments)
        j.push_back(la);
}
