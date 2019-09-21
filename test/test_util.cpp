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
    return this->_choices.at(index);
}


chars_t operator|(const chars_t& a, const chars_t& b) {
    return chars_t(a._choices + b._choices);
}


ostream& operator<<(ostream& os, const chars_t& cc) {
    os << cc._choices;
    return os;
}


/*************************************************
 * Utility Functions
 ************************************************/


int rand_int(int start, int end) {
    int n = end - start;
    int r = rand() / (RAND_MAX / n+1);
    return start + r;
}


int rand_int(int end) {
    return rand_int(0, end);
}


char rand_char(chars_t chars) {
    return chars[rand_int(chars.size())];
}


string rand_string(int size, chars_t chars) {
    string str;
    str.reserve(size);

    for (int i=0; i<size; i++)
        str += rand_char(chars);

    return str;
}
