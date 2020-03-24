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
    default_val("labs", json::object());
    default_val("current_lab", "");
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


string rand_lab_id() {
    return "LAB" + rand_string_digits(4);
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


RandLabSesh::RandLabSesh(const string& id) : ls(LabSesh(id)) {
}


const LabSesh& RandLabSesh::get() const {
    return this->ls;
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
