#include "config.hpp"


LabSesh::LabSesh() : wd(9) {
    // Initialize wd to an invalid value so that we can tell if from_json
    // parsing failed
}


LabSesh::LabSesh(const string& i) : id(i) {
    this->wd = weekday(9);
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
    string id = j.value("id", "");
    ls.id = id;
    // Normalize id (uppercase)
    stl_transform(ls.id, ::toupper);

    string d = j.value("day", "");
    istringstream iss(d);
    iss >> date::parse("%a", ls.wd);
    if (not ls.wd.ok())
        throw Config::error(
            "Lab session weekday is invalid (first char must be capitalized)",
            {"lab-sessions", id});

    iss = istringstream(j.value("start", ""));
    iss >> date::parse(TIME_FMT, ls.start);
    if (not iss.good())
        throw Config::error(
            "Lab session start time is invalid",
            {"lab-sessions", id});

    iss = istringstream(j.value("end", ""));
    iss >> date::parse(TIME_FMT, ls.end);
    if (not iss.good())
        throw Config::error(
            "Lab session end time is invalid",
            {"lab-sessions", id});
}


void from_json(const json& j, LabSessions& lab_sessions) {
    for (const json& j_ls : j) {
        auto ls = j_ls.get<LabSesh>();
        lab_sessions[ls.id] = ls;
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
        j.push_back(ls.second);
}
