#include "config.hpp"


LabSesh::LabSesh() {
}


LabSesh::LabSesh(const string& i) : id(i) {
    // Normalize id (uppercase)
    stl_transform(this->id, ::toupper);
}


bool LabSesh::is_active() const {
    if (current_weekday() != this->weekday)
        return false;
    system_clock::time_point now = current_time();
    return this->start <= now && now < this->end;
}


string LabSesh::format(string fmt) const {
    static const string fmt_t = "%H:%M:%S";
    static const string fmt_t_pretty = "%I:%M:%S%P";

    map<string,string> repl = {
        {"{id}", this->id},

        {"{weekday}", format_weekday(this->weekday)},
        {"{weekday_n}", to_string(this->weekday)},

        {"{start}", format_datetime(this->start, fmt_t)},
        {"{start_p}", format_datetime(this->start, fmt_t_pretty)},

        {"{end}", format_datetime(this->end, fmt_t)},
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


void from_json(const json& j, LabSesh& ls) {
    if (j.type() != json::value_t::string)
        throw mucs_exception::config(
            "Lab entries must be of type string",
            Config::get().filename,
            {"labs", ls.id});

    auto invalid_lab_spec = [&] () {
        throw mucs_exception::config(
            "Lab entries must be in the format " \
                "\"<weekday> <start_time> - <end_time>\"",
            Config::get().filename,
            {"labs", ls.id});
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

        LabSesh ls(id);
        lab_sessions[id] = it.value().get_to(ls);
        config.lab_ids.push_back(id);
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
