#include "config.hpp"


LabAsgmt::LabAsgmt() {
}


LabAsgmt::LabAsgmt(string n) {
    // Normalize name (lowercase)
    stl_transform(n, ::tolower);
    this->name = n;
}


LabAsgmt::LabAsgmt(string n, year_month_day s, year_month_day e) : LabAsgmt(n) {
    this->start = s;
    this->end = e;
}


bool LabAsgmt::compare::operator()(const LabAsgmt& a, const LabAsgmt& b) const {
    /**
     * Sort a before b iff:
     *   a starts and ends before b starts and ends
     *   overlapping and a starts first
     *   b is inside of a
     *   a and b start at the same time but a ends first
     * Otherwise sort b before a
     */
    if (a.start < b.start)
        return true;
    else if (a.start == b.start)
        return a.end < b.end;
    else
        return false;
}


bool LabAsgmt::is_active() const {
    auto today = get_day();
    return this->start <= today && today <= this->end;
}


string LabAsgmt::str() const {
    ostringstream spec;
    spec << format("%Y-%m-%d", this->start)
         << " - "
         << format("%Y-%m-%d", this->end);
    return spec.str();
}


/**
 * Row format: {"START - END"}
 * where START and END are the start and end dates of the week, respectively,
 * both in the format "YYYY-MM-DD"
 */
list<vector<string>> LabAssignments::to_table() const {
    list<vector<string>> table;
    for (auto it=this->begin(); it!=this->end(); it++)
        table.push_back({ it->str() });
    return table;
}


void from_json(const json& j, LabAsgmt& la) {
    string name = j["name"];
    la.name = name;
    stl_transform(la.name, ::tolower);

    auto invalid_lab_asgmt = [&] (string reason) {
        throw Config::error(
            "Invalid lab assignment" + reason,
            {"lab-assignments", name});
    };

    year y(j["year"].get<int>());

    month m;
    istringstream iss_month(j["month"].get<string>());
    iss_month >> date::parse("%b", m);
    if (not m.ok())
        invalid_lab_asgmt(" month");

    int w = j["week"].get<int>();

    la.start = year_month_day(y/m/Monday[w]);
    if (not la.start.ok())
        invalid_lab_asgmt("");

    la.end = year_month_day(sys_days(la.start) + days(6));
}


void from_json(const json& j, LabAssignments& lab_assignments) {
    for (const json& la : j)
        lab_assignments.insert(la.get<LabAsgmt>());
}


void to_json(json& j, const LabAsgmt& la) {
    ostringstream oss_month;
    oss_month << la.start.month();
    j = {
        {"name", la.name},
        {"year", (int)la.start.year()},
        {"month", oss_month.str()},
        {"week", (int)year_month_weekday(la.start).index()}
    };
}


void to_json(json& j, const LabAssignments& lab_assignments) {
    j = json::array();
    for (const LabAsgmt& la : lab_assignments)
        j.push_back(la);
}
