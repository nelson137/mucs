#include "config.hpp"


LabSesh::LabSesh() {
}


LabSesh::LabSesh(const string& i) : id(i) {
    // Normalize id (uppercase)
    stl_transform(this->id, ::toupper);
}


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


list<vector<string>> LabSessions::to_table() const {
    list<vector<string>> table;
    for (auto it=this->begin(); it!=this->end(); it++) {
        // Create row
        vector<string> row;
        row.reserve(3);
        // Column 1
        row.push_back(it->first);
        // Column 2
        row.push_back(format_weekday(it->second.weekday));
        // Column 3
        stringstream time_range;
        time_range << format_datetime(it->second.start, TIME_FMT)
                   << " - "
                   << format_datetime(it->second.end, TIME_FMT);
        row.push_back(time_range.str());
        // Append row
        table.push_back(move(row));
    }
    return table;
}


ostream& operator<<(ostream& os, const LabSesh& ls) {
    return os << ls.id;
}


void from_json(const json& j, LabSesh& ls) {
    auto invalid_lab_spec = [&] () {
        throw Config::error(
            "Lab sessions must be in the format " \
                "\"<weekday> <start_time> - <end_time>\"",
            {"lab-sessions", ls.id});
    };

    string spec = j.get<string>();

    // Should be {"<weekday> <start_time> ", " <end_time>"}
    vector<string> chunks = string_split(spec, "-");
    if (chunks.size() != 2)
        invalid_lab_spec();

    // Should be {"<weekday>", "<start_time>", ""}
    // There will be no third element if spec doesn't have spaces
    //   around the dash
    vector<string> wday_start = string_split(chunks[0], " ");
    if (wday_start.size() < 2)
        invalid_lab_spec();

    ls.weekday = parse_weekday(wday_start[0]);
    ls.start = parse_time(wday_start[1]);
    ls.end = parse_time(chunks[1]);
}


void from_json(const json& j, LabSessions& lab_sessions) {
    string id;
    for (auto it=j.begin(); it!=j.end(); it++) {
        id = it.key();
        // Normalize id (uppercase)
        stl_transform(id, ::toupper);

        lab_sessions[id] = LabSesh(id);
        it.value().get_to(lab_sessions[id]);
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
