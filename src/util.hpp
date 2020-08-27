#ifndef UTIL_HPP
#define UTIL_HPP


#include <algorithm>
#include <chrono>
#include <ctime>
#include <deque>
#include <iomanip>
#include <ios>
#include <iostream>
#include <list>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <pwd.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "date.h"
#include "json.hpp"

#include "except.hpp"

#include "consts.hpp"
#include "proc.hpp"

using namespace std;
using namespace chrono;
using namespace date;
using json = nlohmann::json;


extern sys_seconds NOW;


struct Tabular {

    virtual list<vector<string>> to_table() const = 0;

};


sys_seconds current_datetime();

year_month_day get_day(sys_seconds tp = NOW);

int get_term_width();

seconds get_time(sys_seconds tp = NOW);

weekday get_weekday(sys_seconds tp = NOW);

string join_paths(string a, deque<string> parts);

void print_table(const list<vector<string>>& table, const string& delim = "  ");

vector<string> string_split(const string& s, const string& delim);

string string_strip(string s);


template<typename... String>
string join_paths(string a, string b, String... rest) {
    deque<string> parts = {b, rest...};
    return join_paths(a, parts);
}


template<int n = 2, char c = '0'>
ostream& prefix_char(ostream& os) {
    return os << setw(n) << setfill(c);
}


template<int n = 2>
ostream& prefix_zeros(ostream& os) {
    return os << prefix_char<n,'0'>;
}


template<typename Container, typename T>
bool stl_contains(const Container& c, const T& val) {
    return ::find(c.begin(), c.end(), val) != c.end();
}


template<
    typename Container,
    typename ContainerIt = typename Container::const_iterator,
    typename UnaryOp
>
ContainerIt stl_find_if(const Container& container, const UnaryOp& op) {
    return find_if(container.begin(), container.end(), op);
}


template<typename Container>
string stl_join(const Container& c, const string& delim = ",") {
    if (c.size() == 0)
        return "";
    return accumulate(
        c.cbegin() + 1, c.cend(),
        c.front(),
        [&] (const string& a, const string& b) -> string {
            return a + delim + b;
        }
    );
}


template<typename Container, typename UnaryOp>
void stl_transform(Container& c, UnaryOp op) {
    transform(c.begin(), c.end(), c.begin(), op);
}


#endif
