#include "homeworks.hpp"


Hw::Hw(const string& fn, const string& n) : filename(fn), name(n) {
    // Normalize name (lowercase)
    stl_transform(this->name, ::tolower);
}


Hw Hw::from_iter(const string& fn, const json::const_iterator& it) {
    Hw hw(fn, it.key());
    it.value().get_to(hw);
    return hw;
}


bool Hw::compare::operator()(
    const pair<string,Hw>& a,
    const pair<string,Hw>& b
) const {
    return a.second.duedate < b.second.duedate;
}


Homeworks::Homeworks(const string& fn) {
    this->filename = fn;
}


void from_json(const json& j, Hw& hw) {
    hw.duedate = parse_datetime(j.get<string>());
}


void from_json(const json& j, Homeworks& homeworks) {
    string id;
    for (auto it=j.begin(); it!=j.end(); it++) {
        if (it.value().type() != json::value_t::string)
            throw mucs_exception(error_config(
                "Homework entries must be of type string",
                homeworks.filename,
                "homeworks",
                it.key()));

        auto hw = Hw::from_iter(homeworks.filename, it);
        homeworks.insert({ hw.name, hw });
    }
}


void to_json(json& j, const Hw& hw) {
    time_t tt = duration_cast<seconds>(hw.duedate.time_since_epoch()).count();
    tm *t = localtime(&tt);

    ostringstream os;
    os << put_time(t, "%Y-%m-%d %T");
    j = json(os.str());
}


void to_json(json& j, const Homeworks& homeworks) {
    j = json::object();
    for (auto& hw : homeworks)
        j[hw.first] = json(hw.second);
}
