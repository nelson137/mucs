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
    //   a and ends then b starts and ends
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
    time_t tt = system_clock::to_time_t(this->start);
    tm *t = localtime(&tt);

    ostringstream spec;
    spec << format_datetime(this->start, LAB_ASGMT_FMT)
         << " "
         << ((int) t->tm_mday / 7);
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

    // Should be {"<month>", "<week-of-month>"}
    vector<string> chunks = string_split(j.get<string>(), " ");
    if (chunks.size() != 2)
        invalid_lab_asgmt();

    // t = 00:00:00 on the first day of the given month
    time_t tt = system_clock::to_time_t(
        parse_datetime(chunks[0], LAB_ASGMT_FMT));
    tm t = *localtime(&tt);
    // t.tm_year = this year
    tt = system_clock::to_time_t(NOW);
    t.tm_year = localtime(&tt)->tm_year;

    // If the first day of the month isn't a monday
    if (t.tm_wday != 1)
        // Move forward to the next monday
        tm_add_days(&t, (8 - t.tm_wday) % 7);

    try {
        // Move forward to the monday of the chunks[1]'th week
        tm_add_days(&t, stoi(chunks[1]) * 7);
    } catch (const invalid_argument& e) {
        invalid_lab_asgmt();
    } catch (const out_of_range& e) {
        invalid_lab_asgmt();
    }

    la.start = system_clock::from_time_t(mktime(&t));
    tm_add_days(&t, 6);  // Move forward to sunday
    la.end = system_clock::from_time_t(mktime(&t));
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
