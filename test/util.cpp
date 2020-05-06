#include "util.hpp"


int current_year() {
    time_t now_t = system_clock::to_time_t(NOW);
    tm *now = localtime(&now_t);
    return now->tm_year + 1900;
}


string error_id_unrecognized(const string& user, const string& id) {
    return "Lab id not recognized: {filename}[\"roster\"][\"" + user + "\"]";
}


string error_prop(const string& k, const string& t) {
    return "Config requires key \"" + k + "\" with type " + t + ": {filename}";
}


json new_config_data(json j) {
    auto default_val = [&] (const string& key, const json& val) {
        if (j.count(key) == 0)
            j[key] = val;
    };

    default_val("filename", rand_string());
    default_val("course_id", rand_string(4));
    default_val("admin_hash", "!");
    default_val("homeworks", json::object());
    default_val("lab-sessions", json::object());
    default_val("lab-assignments", json::object());
    default_val("roster", json::object());

    return j;
}


string rand_course() {
    return "course" + rand_string_digits(4);
}


string rand_filename() {
    return "file" + rand_string_digits(4);
}


string rand_hw_name() {
    return "hw" + rand_string_digits(4);
}


string rand_lab_asgmt_name() {
    return "lab" + rand_string_digits(4);
}


string rand_lab_sesh_id() {
    return "LAB_" + rand_string(3, chars_upper);
}


string rand_string_digits(int n) {
    return rand_string(n, chars_nums);
    string s;
    s.reserve(n);
    generate_n(back_inserter(s), n, bind((int(*)(int))&rand_int, 10));
    return s;
}


string rand_user() {
    return "user" + rand_string_digits(4);
}


RandLabSesh::RandLabSesh(const string& id) {
    this->ls.id = id;
}


RandLabSesh& RandLabSesh::today(bool today) {
    int weekday = current_weekday();
    int offset = rand_int(1, 7);
    this->ls.weekday = today ? weekday : ((weekday+offset) % 7);
    return *this;
}


RandLabSesh& RandLabSesh::now(bool now) {
    static tm tm_0 =
        { 0,  0,  0, 1, 0, 70, 0, 0, 0};
    static tm tm_2 =
        { 0,  0,  2, 1, 0, 70, 0, 0, 0};
    static tm tm_22 =
        { 0,  0, 22, 1, 0, 70, 0, 0, 0};
    static tm tm_midnight =
        {59, 59, 23, 1, 0, 70, 0, 0, 0};

    static const system_clock::time_point tp_0 =
        system_clock::from_time_t(mktime(&tm_0));
    static const system_clock::time_point tp_2 =
        system_clock::from_time_t(mktime(&tm_2));
    static const system_clock::time_point tp_22 =
        system_clock::from_time_t(mktime(&tm_22));
    static const system_clock::time_point tp_midnight =
        system_clock::from_time_t(mktime(&tm_midnight));

    if (now) {
        this->ls.start = tp_0;
        this->ls.end = tp_midnight;
    } else {
        time_t now = time(nullptr);
        if (localtime(&now)->tm_hour <= 12) {
            this->ls.start = tp_22;
            this->ls.end = tp_midnight;
        } else {
            this->ls.start = tp_0;
            this->ls.end = tp_2;
        }
    }

    return *this;
}


const LabSesh& RandLabSesh::get() const {
    return this->ls;
}


const vector<string> RandLabAsgmt::MONTHS = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec"
};


RandLabAsgmt::RandLabAsgmt(const string& n) : name(n) {
}


RandLabAsgmt& RandLabAsgmt::not_this_week() {
    time_t tt = system_clock::to_time_t(system_clock::now());
    tm *now = localtime(&tt);

    int dm = rand_int(1, 5) * (now->tm_mon < 6 ? 1 : -1);
    this->month = RandLabAsgmt::MONTHS[now->tm_mon + dm];

    this->week_n = rand_int(3);

    return *this;
}


RandLabAsgmt& RandLabAsgmt::this_week() {
    time_t tt = system_clock::to_time_t(NOW);
    tm *now = localtime(&tt);

    this->month = RandLabAsgmt::MONTHS[now->tm_mon];

    tt += (1 - now->tm_wday) * 24 * 60 * 60;
    this->week_n = localtime(&tt)->tm_mday / 7;

    return *this;
}


LabAsgmt RandLabAsgmt::get() const {
    string spec = this->month + " " + to_string(this->week_n);
    LabAsgmt la = json(spec).get<LabAsgmt>();
    la.name = this->name;
    return la;
}
