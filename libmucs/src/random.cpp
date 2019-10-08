#include "mucs/random.hpp"


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
