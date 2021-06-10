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

    /**
     * Return a 2D array of data to be printed in a table format.
     */
    virtual list<vector<string>> to_table() const = 0;

};


/**
 * Return the current datetime.
 */
sys_seconds current_datetime();

/**
 * Return the date of the given datetime.
 */
year_month_day get_day(sys_seconds tp = NOW);

/**
 * Return the number of columns for the current terminal.
 */
int get_term_width();

/**
 * Return the time of the given datetime.
 */
seconds get_time(sys_seconds tp = NOW);

/**
 * Return the weekday of the given datetime.
 */
weekday get_weekday(sys_seconds tp = NOW);

/**
 * Return the username of the current UID.
 */
string get_user();

/**
 * Join a and each element of parts using `/` as a delimeter.
 */
string join_paths(string a, deque<string> parts);

/**
 * Join all arguments using `/` as a delimeter.
 */
template<typename... String>
string join_paths(string a, string b, String... rest) {
    deque<string> parts = {b, rest...};
    return join_paths(a, parts);
}

/**
 * Set os to have a field width of n and a fill character of c.
 */
template<int n = 2, char c = '0'>
ostream& prefix_char(ostream& os) {
    return os << setw(n) << setfill(c);
}

/**
 * Set os to have a field width of n and a fill character of `0`.
 */
template<int n = 2>
ostream& prefix_zeros(ostream& os) {
    return os << prefix_char<n,'0'>;
}

/**
 * Print each column of table with the same width and separate each column with
 * delim.
 */
void print_table(const list<vector<string>>& table, const string& delim = "  ");

/**
 * Return whether c contains val.
 */
template<typename Container, typename T>
bool stl_contains(const Container& c, const T& val) {
    return ::find(c.begin(), c.end(), val) != c.end();
}

template<typename Container, typename Pred>
Container stl_copy_into_with(const Container& c, Pred pred) {
    Container out;
    copy_if(c.cbegin(), c.cend(), back_inserter(out), pred);
    return out;
}

/**
 * Return the first element of container for which op returns true.
 * If none is found return Container::end().
 */
template<
    typename Container,
    typename ContainerIt = typename Container::const_iterator,
    typename UnaryOp
>
ContainerIt stl_find_if(const Container& container, const UnaryOp& op) {
    return find_if(container.begin(), container.end(), op);
}

/**
 * Return a string of all elements of c joined by delim.
 */
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

/**
 * Modify each element of c using op.
 */
template<typename Container, typename UnaryOp>
void stl_transform(Container& c, UnaryOp op) {
    transform(c.begin(), c.end(), c.begin(), op);
}

/**
 * Return a new container of each element of c_in mapped with op.
 * Container b is resized to have the same number of elements as a.
 */
template<typename ContainerOut, typename ContainerIn, typename UnaryOp>
ContainerOut stl_transform_into(const ContainerIn& c_in, UnaryOp op) {
    ContainerOut c_out;
    c_out.reserve(c_in.size());
    transform(c_in.cbegin(), c_in.cend(), back_inserter(c_out), op);
    return c_out;
}

/**
 * Return a list of tokens of s split on delim.
 *
 * None of the tokens will contain delim.
 *
 * An empty list will never be returned. If s doesn't contain delim then the
 * list will have 1 element, being s.
 */
vector<string> string_split(const string& s, const string& delim);

/**
 * Return s with all whitespace characters removed from the start and end.
 */
string string_strip(string s);


#endif
