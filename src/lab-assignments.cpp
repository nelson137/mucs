#include "config.hpp"


LabAsgmt::LabAsgmt() {
}


LabAsgmt::LabAsgmt(const string& n) : name(n) {
    // Normalize name (lowercase)
    stl_transform(this->name, ::tolower);
}


bool LabAsgmt::compare::operator()(
    const pair<string,LabAsgmt>& a,
    const pair<string,LabAsgmt>& b
) const {
    // Sort `a` before `b` iff:
    //   a starts and ends then b starts and ends
    //   overlapping but offset, a starts first
    //   b is inside of a
    // Otherwise sort `b` before `a`
    if (a.second.start < b.second.start)
        return true;
    else if (a.second.start == b.second.start)
        return a.second.end < b.second.end;
    else
        return false;
}


string LabAsgmt::str() const {
    ostringstream spec;
    spec << format("%b", this->start)
         << " "
         << year_month_weekday(this->start).index();
    return spec.str();
}


void from_json(const json& j, LabAsgmt& la) {
    if (j.type() != json::value_t::string)
        throw Config::error(
            "Lab assignments must be of type string",
            {"lab-assignments", la.name});

    auto invalid_lab_asgmt = [&] () {
        throw Config::error(
            "Lab assignments must be in the format \"<month> <week-of-month>\"",
            {"lab-assignments", la.name});
    };

    month_day md;
    istringstream(j.get<string>()) >> parse(LAB_ASGMT_FMT, md);
    if (not md.ok())
        invalid_lab_asgmt();

    auto week = (unsigned) md.day();
    auto today = get_day();

    la.start = year_month_day(today.year()/md.month()/Monday[week]);
    if (not la.start.ok())
        invalid_lab_asgmt();

    la.end = today.year()/md.month()/(la.start.day() + days(6));
}


void from_json(const json& j, LabAssignments& lab_assignments) {
    string name;
    LabAsgmt la;
    for (auto it=j.begin(); it!=j.end(); it++) {
        if (it.value().type() != json::value_t::string)
            throw Config::error(
                "Lab assignments must be of type string",
                {"lab-assignments", it.key()});

        name = it.key();
        // Normalize month
        stl_transform(name, ::tolower);

        la = LabAsgmt(name);
        it.value().get_to(la);
        lab_assignments.insert({ name, la });
    }
}


void to_json(json& j, const LabAsgmt& la) {
    j = json(la.str());
}


void to_json(json& j, const LabAssignments& lab_assignments) {
    j = json::object();
    for (auto& la : lab_assignments)
        j[la.first] = json(la.second);
}
