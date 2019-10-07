#include "test_util.hpp"


/*************************************************
 * chars_t
 ************************************************/


string chars_t::get() const {
    return this->_choices;
}


size_t chars_t::size() const {
    return this->_choices.size();
}


char chars_t::operator[](const int index) const {
    return this->_choices.operator[](index);
}


chars_t operator|(const chars_t& a, const chars_t& b) {
    return chars_t(a._choices + b._choices);
}


ostream& operator<<(ostream& os, const chars_t& cc) {
    os << cc._choices;
    return os;
}


/*************************************************
 * temp_file
 ************************************************/


temp_file::temp_file() : temp_file(gen_temp_name()) {
}


temp_file::temp_file(string name) {
    this->name = name;

    // Touch the file
    ofstream f(this->name);
}


temp_file::~temp_file() noexcept(false) {
    if (remove(this->name.c_str()) < 0)
        throw mucs_exception("Unable to remove file: " + this->name);
}


void temp_file::write(string& data) {
    ofstream(this->name) << data;
}


void temp_file::write(string&& data) {
    return this->write(data);
}


/*************************************************
 * temp_dir
 ************************************************/


temp_dir::temp_dir() {
    this->name = gen_temp_name();

    // Make the directory
    if (mkdir(this->name.c_str(), 0755) < 0)
        throw mucs_exception("Unable to make directory: " + this->name);
}


temp_dir::~temp_dir() noexcept(false) {
    if (rmdir(this->name.c_str()) < 0)
        throw mucs_exception("Unable to remove directory: " + this->name);
}


/*************************************************
 * Utility Functions
 ************************************************/


string gen_temp_name(string dir) {
    string fn_s = dir + "/mucs-temp.XXXXXX";
    int size = fn_s.size();

    char fn[size + 1];
    fn_s.copy(fn, size);
    fn[size] = 0;

    mktemp(fn);
    return string(fn);
}


int rand_int(int start, int end) {
    random_device rng;
    uniform_int_distribution<int> dist(start, end);
    return dist(rng);
}


int rand_int(int end) {
    return rand_int(0, end);
}


char rand_char(chars_t chars) {
    return chars[rand_int(chars.size()-1)];
}


string rand_string(int size, chars_t chars) {
    char s[size+1] = {0};
    for (int i=0; i<size; i++)
        s[i] = rand_char(chars);
    return s;
}
