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


string LabAsgmt::week_str() const {
    ostringstream spec;
    spec << format("%Y %b", this->start)
         << " "
         << year_month_weekday(this->start).index();
    return spec.str();
}


/**
 * Row format: {WEEK_SPEC}
 * where WEEK_SPEC is the same format as in the json config
 */
list<vector<string>> LabAssignments::to_table() const {
    list<vector<string>> table;
    for (auto it=this->begin(); it!=this->end(); it++)
        table.push_back({ it->week_str() });
    return table;
}


void from_json(const json& j, LabAsgmt& la) {
    string name = j["name"];
    la.name = name;
    stl_transform(la.name, ::tolower);

    auto invalid_lab_asgmt = [&] () {
        throw Config::error(
            "Invalid lab assignment week",
            {"lab-assignments", name});
    };

    /**
     * Explicitly zero-initialize this variable so that it is invalid. The
     * fields will be y_=0, m_=0, and d_=0; m_ and d_ are invalid in this case.
     *
     * If declared as `year_month_day ymd;` its members will be uninitialized
     * which could result in a valid date. Then, if parsing the "week" key
     * with invalid data that doesn't modify ymd, the uninitialized data could
     * erroneously pass undetected, causing a flickering test (and possibly
     * weird behavior).
     */
    year_month_day ymd{};

    // Parses as "YEAR MONTH_ABREV DAY" but DAY will be used as a week index
    istringstream iss_week(j["week"].get<string>());
    iss_week >> date::parse("%Y %b %d", ymd);
    if (not ymd.ok())
        invalid_lab_asgmt();

    auto week = (unsigned) ymd.day();

    la.start = year_month_day(ymd.year()/ymd.month()/Monday[week]);
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
    for (const LabAsgmt& la : lab_assignments)
        j.push_back(la);
}
