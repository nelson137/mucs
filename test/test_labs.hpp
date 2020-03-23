#ifndef TEST_LABS_HPP
#define TEST_LABS_HPP


#include <ctime>
#include <iomanip>
#include <string>

#include "catch.hpp"

#include "mucs/except.hpp"
#include "mucs/random.hpp"

#include "util.hpp"

#include "../src/config.hpp"

using namespace std;
using Catch::Matchers::Equals;


constexpr bool TODAY      = true;
constexpr bool NOT_TODAY  = false;
constexpr bool ACTIVE     = true;
constexpr bool NOT_ACTIVE = false;


template<bool is_today, bool is_active>
LabSesh rand_labsesh(const string& id = rand_string(2, chars_upper)) {
    LabSesh ls(id);

    int w = current_weekday();
    if (is_today)
        ls.weekday = w;
    else
        ls.weekday = (w + rand_int(1,7)) % 7;

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

    if (is_active) {
        ls.start = tp_0;
        ls.end = tp_midnight;
    } else {
        time_t now = time(nullptr);
        if (localtime(&now)->tm_hour <= 12) {
            ls.start = tp_22;
            ls.end = tp_midnight;
        } else {
            ls.start = tp_0;
            ls.end = tp_2;
        }
    }

    return ls;
}


#endif
