#include "config.hpp"


bool Hw::compare::operator()(
    const pair<string,Hw>& a,
    const pair<string,Hw>& b
) const {
    return a.second.duedate < b.second.duedate;
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
                Config::get().filename,
                "homeworks",
                it.key()));

        id = it.key();
        // Normalize id (lowercase)
        stl_transform(id, ::tolower);

        homeworks.insert({ id, it.value().get<Hw>() });
    }
}


void to_json(json& j, const Hw& hw) {
    time_t tt = duration_cast<seconds>(hw.duedate.time_since_epoch()).count();
    j = json(format_datetime(tt, "%Y-%m-%d %T"));
}


void to_json(json& j, const Homeworks& homeworks) {
    j = json::object();
    for (auto& hw : homeworks)
        j[hw.first] = json(hw.second);
}
