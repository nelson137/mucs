#include "random.hpp"


chars_t::chars_t(const string& cs) : _choices(cs) {
}


string chars_t::get() const {
    return this->_choices;
}


size_t chars_t::size() const {
    return this->_choices.size();
}


char chars_t::operator[](const int index) const {
    return this->_choices.at(index);
}


chars_t operator|(const chars_t& a, const chars_t& b) {
    return chars_t(a._choices + b._choices);
}


ostream& operator<<(ostream& os, const chars_t& cc) {
    os << cc._choices;
    return os;
}


char rand_char(const chars_t& chars) {
    return chars[rand_int(chars.size())];
}


int rand_int(const int start, const int end) {
    random_device rng;
    uniform_int_distribution<int> dist(start, end - 1);
    return dist(rng);
}


int rand_int(const int end) {
    return rand_int(0, end);
}


string rand_string(const int size, const chars_t& chars) {
    string s;
    s.reserve(size);
    for (int i=0; i<size; i++)
        s += rand_char(chars);
    return s;
}


json new_config_data(json root) {
    auto default_val = [] (json& j, const string& key, const json& val) {
        if (not j.contains(key))
            j[key] = val;
    };

    default_val(root, "course_id", rand_string(4));
    default_val(root, "admin_hash", "!");
    default_val(root, "overrides", json::object());
    default_val(root["overrides"], "labs", json::array());
    default_val(root["overrides"], "homeworks", json::array());
    default_val(root, "homeworks", json::array());
    default_val(root, "lab-sessions", json::array());
    default_val(root, "lab-assignments", json::array());

    return root;
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


json rand_lab_sesh_data(json j) {
    auto default_val = [&] (const string& key, const json& val) {
        if (not j.contains(key))
            j[key] = val;
    };

    default_val("id", rand_lab_sesh_id());
    default_val("day", to_string(Monday + days(rand_int(7))));
    default_val("start", "00:00:00");
    default_val("end", "23:59:59");

    return j;
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
    this->called_today = true;
    weekday w = get_weekday();
    this->ls.wd = today ? w : w + days(rand_int(1, 7));
    return *this;
}


RandLabSesh& RandLabSesh::now(bool now) {
    this->called_now = true;

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
    if (!(this->called_today && this->called_now))
        throw "RandLabSesh: all builder methods must be called to build";
    return this->ls;
}


RandLabAsgmt::RandLabAsgmt(const string& n) : name(n) {
}


RandLabAsgmt& RandLabAsgmt::this_week(bool this_week) {
    this->called_this_week = true;

    year_month_day today = get_day();
    if (this_week) {
        /**
         * The date library considers weeks to be Sunday to Saturday but we
         * want weeks to be Monday to Sunday. this->ymwd needs to be the Monday
         * of the desired week.
         */
        weekday wd = year_month_weekday(today).weekday();
        this->ymwd = wd == Monday
            ? year_month_weekday(today)
            : year_month_weekday(
                // today - distance_from_monday
                sys_days(today) - (wd == Sunday ? days(6) : wd - Monday))
            ;
    } else {
        month m = today.month() + months(rand_int(12));
        this->ymwd = today.year()/m/Monday[1];
    }

    return *this;
}


LabAsgmt RandLabAsgmt::get() const {
    if (!this->called_this_week)
        throw "RandLabAsgmt: all builder methods must be called to build";

    ostringstream week_ss;
    week_ss << this->ymwd.month();
    json j = {
        {"name", this->name},
        {"year", (int)this->ymwd.year()},
        {"month", week_ss.str()},
        {"week", (int)this->ymwd.index()},
    };
    return j.get<LabAsgmt>();
}
