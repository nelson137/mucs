#include "test_util.hpp"


TEST_CASE("join_paths", "[join_paths]") {
    string expected, actual;

    SECTION("empty, empty") {
        expected = "";
        actual = join_paths("", "");
    };

    SECTION("not empty, empty") {
        expected = "a";
        actual = join_paths("a", "");
    };
    SECTION("empty, not empty") {
        expected = "a";
        actual = join_paths("", "a");
    };

    SECTION("two args") {
        expected = "a/b";
        actual = join_paths("a", "b");
    };
    SECTION("three args") {
        expected = "a/b/c";
        actual = join_paths("a", "b", "c");
    };
    SECTION("four args") {
        expected = "a/b/c/d";
        actual = join_paths("a", "b", "c", "d");
    };

    SECTION("no slash, no slash") {
        expected = "a/b";
        actual = join_paths("a", "b");
    }
    SECTION("no slash, slash") {
        expected = "a/b";
        actual = join_paths("a", "/b");
    }
    SECTION("slash, no slash") {
        expected = "a/b";
        actual = join_paths("a/", "b");
    }
    SECTION("slash, slash") {
        expected = "a/b";
        actual = join_paths("a/", "/b");
    }

    REQUIRE_THAT(expected, Equals(actual));
}


TEST_CASE("parse_datetime", "[parse_datetime]") {
    tm expected;
    // tm_year = years since 1900
    expected.tm_year = 2000 + rand_int(20) - 1900;  // 2000 - 2019
    // tm_mon: Jan=0, Feb=1, ...
    expected.tm_mon = rand_int(11);
    expected.tm_mday = rand_int(1, 29);
    expected.tm_hour = rand_int(24);
    expected.tm_min = rand_int(60);
    expected.tm_sec = rand_int(60);

    static const string dt_fmt = "%Y-%m-%d %H:%M:%S";
    string expected_str;

    SECTION("valid") {
        expected_str = format_datetime(expected, dt_fmt);

        auto parsed_dt = parse_datetime(expected_str);
        time_t parsed_tt = system_clock::to_time_t(parsed_dt);
        tm actual = *localtime(&parsed_tt);

        REQUIRE(expected.tm_year == actual.tm_year);
        REQUIRE(expected.tm_mon == actual.tm_mon);
        REQUIRE(expected.tm_mday == actual.tm_mday);
        REQUIRE(expected.tm_hour == actual.tm_hour);
        REQUIRE(expected.tm_min == actual.tm_min);
        REQUIRE(expected.tm_mday == actual.tm_mday);
    }

    SECTION("invalid") {
        expected.tm_sec = rand_int(-59, 0);
        expected_str = format_datetime(expected, dt_fmt);

        REQUIRE_THROWS_WITH(
            parse_datetime(expected_str),
            "Invalid datetime: " + expected_str
        );
    }
}


TEST_CASE("parse_time", "[parse_time]") {
    tm expected;
    expected.tm_hour = rand_int(24);
    expected.tm_min = rand_int(60);
    expected.tm_sec = rand_int(60);

    static const string time_fmt = "%H:%M:%S";
    string expected_str;

    SECTION("valid") {
        expected_str = format_datetime(expected, time_fmt);

        time_t parsed_tt = parse_time(expected_str);
        tm actual = *localtime(&parsed_tt);

        REQUIRE(expected.tm_hour == actual.tm_hour);
        REQUIRE(expected.tm_min == actual.tm_min);
        REQUIRE(expected.tm_sec == actual.tm_sec);
    }

    SECTION("invalid") {
        expected.tm_sec = rand_int(-59, 0);
        expected_str = format_datetime(expected, time_fmt);

        REQUIRE(parse_time(expected_str) == -1);
    }
}


TEST_CASE("parse_weekday", "[parse_weekday]") {
    static vector<string> weekdays = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };

    SECTION("valid") {
        int expected = rand_int(7);
        int actual = parse_weekday(weekdays[expected]);
        REQUIRE(expected == actual);
    }

    SECTION("invalid") {
        string invalid_weekday = rand_string(2);
        REQUIRE_THROWS_WITH(
            parse_weekday(invalid_weekday),
            "Invalid weekday: " + invalid_weekday
        );
    }
}
