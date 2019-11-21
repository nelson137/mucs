#include "labs.hpp"


LabSesh::LabSesh() {
}


LabSesh::LabSesh(
    const string& fn,
    const string& i
) : filename(fn), id(i) {
    // Normalize id (uppercase)
    stl_transform(this->id, ::toupper);
}


LabSesh LabSesh::from_iter(const string& fn, const json::const_iterator& it) {
    LabSesh ls(fn, it.key());
    it.value().get_to(ls);
    return ls;
}


bool LabSesh::is_active() const {
    if (current_weekday() != this->weekday)
        return false;
    time_t now = current_time();
    return this->start <= now && now < this->end;
}


string LabSesh::format(string fmt) const {
    static const string fmt_t = "%H:%M:%S";
    static const string fmt_t_pretty = "%I:%M:%S%P";

    const auto str_repl = [&](const string& pat, const string& repl) {
        fmt = regex_replace(fmt, regex(pat), repl);
    };

    str_repl("\\{id}", this->id);

    str_repl("\\{weekday}", format_weekday(this->weekday));
    str_repl("\\{weekday_n}", to_string(this->weekday));

    str_repl("\\{start}", format_time(this->start, fmt_t));
    str_repl("\\{start_p}", format_time(this->start, fmt_t_pretty));

    str_repl("\\{end}", format_time(this->end, fmt_t));
    str_repl("\\{end_p}", format_time(this->end, fmt_t_pretty));

    return fmt;
}


LabSessions::LabSessions(const string& fn) {
    this->filename = fn;
}


void from_json(const json& j, LabSesh& ls) {
    if (j.type() != json::value_t::string)
        throw mucs_exception(error_config(
            "Lab entries must be of type string", ls.filename, "labs", ls.id));

    // Normalize lab specification (lowercase)
    string lab_spec = j.get<string>();
    stl_transform(lab_spec, ::tolower);

    regex lab_spec_re(R"(\s*(\S+)\s+(\S+)\s*-\s*(\S+)\s*)");
    smatch match;
    if (not regex_match(lab_spec, match, lab_spec_re))
        throw mucs_exception(error_config(
            "Lab entries must be in the format " \
                "\"<weekday> <start_time> - <end_time>\"",
            ls.filename,
            "labs",
            ls.id));

    ls.weekday = parse_weekday(match[1]);
    ls.start = parse_time(match[2]);
    ls.end = parse_time(match[3]);
}


void from_json(const json& j, LabSessions& lab_sessions) {
    for (auto it=j.begin(); it!=j.end(); it++) {
        auto ls = LabSesh::from_iter(lab_sessions.filename, it);
        lab_sessions[ls.id] = ls;
        lab_sessions.all_ids.push_back(ls.id);
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
