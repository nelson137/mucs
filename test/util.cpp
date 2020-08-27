#include "util.hpp"


json new_config_data(json j) {
    auto default_val = [&] (const string& key, const json& val) {
        if (j.count(key) == 0)
            j[key] = val;
    };

    default_val("filename", rand_string());
    default_val("course_id", rand_string(4));
    default_val("admin_hash", "!");
    default_val("homeworks", json::array());
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
    return rand_string(3, chars_upper);
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
    weekday w = get_weekday();
    this->ls.wd = today ? w : w + days(rand_int(1, 7));
    return *this;
}


RandLabSesh& RandLabSesh::now(bool now) {
    // Beginning and end of day
    constexpr seconds bod = seconds(0);
    constexpr seconds eod = days(1) - seconds(1);

    // Current time of day
    time_of_day<seconds> tod = make_time(get_time());

    if (now) {
        this->ls.start = bod;
        this->ls.end = eod;
    } else {
        if (tod.hours() < hours(12)) {
            this->ls.start = hours(rand_int(12, 24));
            this->ls.end = eod;
        } else {
            this->ls.start = bod;
            this->ls.end = hours(rand_int(1, 12));
        }
    }

    return *this;
}


const LabSesh& RandLabSesh::get() const {
    return this->ls;
}


RandLabAsgmt::RandLabAsgmt(const string& n) : name(n), ymwd(get_day()) {
}


RandLabAsgmt& RandLabAsgmt::this_week(bool this_week) {
    if (not this_week) {
        int dm = rand_int(1, 5) * (get_day().month() < month(6) ? 1 : -1);
        this->ymwd += months(dm);
    }
    return *this;
}


LabAsgmt RandLabAsgmt::get() const {
    year_month_day ymd(this->ymwd);
    ostringstream value;
    value << this->ymwd.month() << ' ' << this->ymwd.index();
    auto la = json(value.str()).get<LabAsgmt>();
    la.name = this->name;
    return la;
}
