#include "config.hpp"


LabSesh::LabSesh() {
}


LabSesh::LabSesh(const string& i) : id(i) {
    // Normalize id (uppercase)
    stl_transform(this->id, ::toupper);
}


LabSesh::LabSesh(string id, weekday w, seconds s, seconds e)
        : id(id), wd(w), start(s), end(e) {
}


LabSesh::operator string() const {
    return this->id;
}


bool LabSesh::is_active() const {
    if (get_weekday() != this->wd)
        return false;
    seconds now = get_time();
    return this->start <= now && now < this->end;
}


string LabSesh::format(string fmt) const {
    static const string fmt_t_pretty = "%I:%M:%S%P";

    map<string,string> repl = {
        {"{id}", this->id},

        {"{weekday}", ::format("%A", this->wd)},
        {"{weekday_n}", to_string(this->wd.c_encoding())},

        {"{start}", ::format(TIME_FMT, this->start)},
        {"{start_p}", ::format(fmt_t_pretty, this->start)},

        {"{end}", ::format(TIME_FMT, this->end)},
        {"{end_p}", ::format(fmt_t_pretty, this->end)}
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
        row.push_back(::format("%A", it->second.wd));
        // Column 3
        stringstream time_range;
        time_range << ::format(TIME_FMT, it->second.start)
                   << " - "
                   << ::format(TIME_FMT, it->second.end);
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

    istringstream(string_strip(wday_start[0])) >> parse("%a", ls.wd);
    if (not ls.wd.ok())
        throw Config::error(
            "Lab session weekday is invalid (first char must be capitalized)",
            {"lab-sessions", ls.id});

    istringstream start_iss(string_strip(wday_start[1]));
    start_iss >> parse(TIME_FMT, ls.start);
    if (not start_iss.good())
        throw Config::error(
            "Lab session start time is invalid",
            {"lab-sessions", ls.id});

    istringstream end_iss(string_strip(chunks[1]));
    end_iss >> parse(TIME_FMT, ls.end);
    if (not end_iss.good())
        throw Config::error(
            "Lab session end time is invalid",
            {"lab-sessions", ls.id});
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
