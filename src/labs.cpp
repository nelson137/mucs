#include "config.hpp"


LabSesh::operator string() const {
    return this->id;
}


bool LabSesh::is_active() const {
    if (current_weekday() != this->weekday)
        return false;
    system_clock::time_point now = current_time();
    return this->start <= now && now < this->end;
}


string LabSesh::format(string fmt) const {
    static const string fmt_t_pretty = "%I:%M:%S%P";

    map<string,string> repl = {
        {"{id}", this->id},

        {"{weekday}", format_weekday(this->weekday)},
        {"{weekday_n}", to_string(this->weekday)},

        {"{start}", format_datetime(this->start, TIME_FMT)},
        {"{start_p}", format_datetime(this->start, fmt_t_pretty)},

        {"{end}", format_datetime(this->end, TIME_FMT)},
        {"{end_p}", format_datetime(this->end, fmt_t_pretty)}
    };

    size_t i;
    for (const auto& r : repl) {
        i = 0;
        while ((i = fmt.find(r.first, i)) != string::npos)
            fmt.replace(i, r.first.size(), r.second);
    }

    return fmt;
}


ostream& operator<<(ostream& os, const LabSesh& ls) {
    return os << ls.id;
}


bool LabAsgmnt::compare::operator()(
    const pair<string, LabAsgmnt>& a,
    const pair<string, LabAsgmnt>& b
) const {
    return a.second.start < b.second.start;
}


void from_json(const json& j, LabSesh& ls) {
    if (j.type() != json::value_t::string)
        throw Config::error(
            "lab_sessions entries must be of type string",
            {"lab_sessions", ls.id});

    auto invalid_lab_spec = [&] () {
        throw Config::error(
            "lab_sessions entries must be in the format " \
                "\"<weekday> <start_time> - <end_time>\"",
            {"lab_sessions", ls.id});
    };

    string lab_spec_str = j.get<string>();

    // Should be {"<weekday> <start_time> ", " <end_time>"}
    vector<string> chunks = string_split(lab_spec_str, "-");
    if (chunks.size() != 2)
        invalid_lab_spec();

    // Should be {"<weekday>", "<start_time>", ""}
    vector<string> wday_start = string_split(chunks[0], " ");
    ls.weekday = parse_weekday(wday_start[0]);
    ls.start = parse_time(wday_start[1]);
    ls.end = parse_time(chunks[1]);
}


void from_json(const json& j, LabSessions& lab_sessions) {
    auto& config = Config::get();
    string id;
    for (auto it=j.begin(); it!=j.end(); it++) {
        id = it.key();
        // Normalize id (uppercase)
        stl_transform(id, ::toupper);

        config.lab_ids.push_back(id);
        lab_sessions[id] = LabSesh();
        lab_sessions[id].id = id;
        it.value().get_to(lab_sessions[id]);
    }
}


void from_json(const json& j, LabAsgmnt& lab_a) {
    lab_a.start = parse_date(j.get<string>());
    lab_a.end = lab_a.start + duration<long, ratio<86400>>(1);
}


void from_json(const json& j, LabAssignments& lab_assignments) {
    string name;
    LabAsgmnt lab_a;

    for (auto it=j.begin(); it!=j.end(); it++) {
        if (it.value().type() != json::value_t::string)
            throw mucs_exception::config(
                "lab_assignments entries must be of type string",
                Config::get().filename,
                {"lab_assignments", it.key()});

        name = it.key();
        // Normalize name (lowercase)
        stl_transform(name, ::tolower);

        lab_a = LabAsgmnt();
        lab_a.name = name;
        lab_a = it.value().get<LabAsgmnt>();
        lab_assignments.insert({ name, lab_a });
    }
}


void to_json(json& j, const LabSesh& ls) {
    j = ls.format("{weekday} {start} - {end}");
}


void to_json(json& j, const LabSessions& lab_sessions) {
    j = json::object();
    for (auto& ls : lab_sessions)
        j[ls.first] = json(ls.second);
}


void to_json(json& j, const LabAsgmnt& lab_a) {
    j = format_datetime(lab_a.start, DATE_FMT);
}


void to_json(json& j, const LabAssignments& lab_assignments) {
    j = json::object();
    for (auto& la : lab_assignments)
        j[la.first] = json(la.second);
}
