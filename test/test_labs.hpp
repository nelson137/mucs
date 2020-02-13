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


constexpr bool IS_TODAY      = true;
constexpr bool NOT_TODAY     = false;
constexpr bool T_IN_BOUND    = true;
constexpr bool T_OUT_BOUND   = false;


template<bool is_today, bool in_bound>
LabSesh rand_labsesh() {
    LabSesh ls(rand_string(2, chars_lower));

    int w = current_weekday();
    if (is_today)
        ls.weekday = w;
    else
        ls.weekday = (w + rand_int(1,7)) % 7;

    static int zero            = 0;
    static int before_noon     = 43199;
    static int two             = 50400;
    static int before_midnight = 86399;
    if (in_bound) {
        ls.start = zero;
        ls.end = before_midnight;
    } else {
        time_t tt_now = time(nullptr);
        int h = localtime(&tt_now)->tm_hour;
        if (h <= 12) {
            ls.start = two;
            ls.end = before_midnight;
        } else {
            ls.start = zero;
            ls.end = before_noon;
        }
    }

    return ls;
}


#endif
