#include "config.hpp"


LabSesh::LabSesh() : wd(9) {
    // Initialize wd to an invalid value so that we can tell if from_json
    // parsing failed
}


LabSesh::LabSesh(string i) : LabSesh() {
    // Normalize id (uppercase)
    stl_transform(i, ::toupper);
    this->id = i;
}


LabSesh::LabSesh(string id, weekday w, seconds s, seconds e) : LabSesh(id) {
    this->wd = w;
    this->start = s;
    this->end = e;
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

        {"{weekday}", ::format("%a", this->wd)},
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


vector<LabSesh> LabSessions::get_with_id(const string& id) const {
    return stl_copy_into_with<vector<LabSesh>>(
        *this,
        [&] (const LabSesh& ls) { return ls.id == id; }
    );
}


list<vector<string>> LabSessions::to_table() const {
    list<vector<string>> table;
    for (const LabSesh& ls : *this) {
        // Create row
        vector<string> row;
        row.reserve(3);
        // Column 1
        row.push_back(ls.id);
        // Column 2
        row.push_back(::format("%A", ls.wd));
        // Column 3
        stringstream time_range;
        time_range << ::format(TIME_FMT, ls.start)
                << " - "
                << ::format(TIME_FMT, ls.end);
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
    string id = j["id"];
    ls.id = id;
    // Normalize id (uppercase)
    stl_transform(ls.id, ::toupper);

    // Parse day field
    string d = j["day"];
    istringstream iss_day(d);
    iss_day >> date::parse("%a", ls.wd);
    if (not ls.wd.ok())
        throw Config::error(
            "Lab session weekday is invalid (first char must be capitalized)",
            {"lab-sessions", id});

    // Parse start time field
    istringstream iss_start(j["start"].get<string>());
    iss_start >> date::parse(TIME_FMT, ls.start);
    if (not iss_start.good())
        throw Config::error(
            "Lab session start time is invalid",
            {"lab-sessions", id});

    // Parse end time field
    istringstream iss_end(j["end"].get<string>());
    iss_end >> date::parse(TIME_FMT, ls.end);
    if (not iss_end.good())
        throw Config::error(
            "Lab session end time is invalid",
            {"lab-sessions", id});
}


void from_json(const json& j, LabSessions& lab_sessions) {
    for (const json& j_ls : j) {
        auto ls = j_ls.get<LabSesh>();
        lab_sessions.push_back(ls);
    }
}


void to_json(json& j, const LabSesh& ls) {
    j = {
        {"id", ls.id},
        {"day", ls.format("{weekday}")},
        {"start", ls.format("{start}")},
        {"end", ls.format("{end}")}
    };
}


void to_json(json& j, const LabSessions& lab_sessions) {
    j = json::array();
    for (auto& ls : lab_sessions)
        j.push_back(ls);
}
