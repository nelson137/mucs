#include "test_util.hpp"


TEST_CASE("join_paths", "[join_paths]") {
    string expected, actual;

    SECTION("empty, empty") {
        expected = "";
        actual = join_paths("", "");
    }

    SECTION("not empty, empty") {
        expected = "a";
        actual = join_paths("a", "");
    }
    SECTION("empty, not empty") {
        expected = "a";
        actual = join_paths("", "a");
    }

    SECTION("two args") {
        expected = "a/b";
        actual = join_paths("a", "b");
    }
    SECTION("three args") {
        expected = "a/b/c";
        actual = join_paths("a", "b", "c");
    }
    SECTION("four args") {
        expected = "a/b/c/d";
        actual = join_paths("a", "b", "c", "d");
    }

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

    REQUIRE_THAT(actual, Equals(expected));
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

        REQUIRE(actual.tm_year == expected.tm_year);
        REQUIRE(actual.tm_mon == expected.tm_mon);
        REQUIRE(actual.tm_mday == expected.tm_mday);
        REQUIRE(actual.tm_hour == expected.tm_hour);
        REQUIRE(actual.tm_min == expected.tm_min);
        REQUIRE(actual.tm_mday == expected.tm_mday);
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

        REQUIRE(actual.tm_hour == expected.tm_hour);
        REQUIRE(actual.tm_min == expected.tm_min);
        REQUIRE(actual.tm_sec == expected.tm_sec);
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
        REQUIRE(actual == expected);
    }

    SECTION("invalid") {
        string invalid_weekday = rand_string(2);
        REQUIRE_THROWS_WITH(
            parse_weekday(invalid_weekday),
            "Invalid weekday: " + invalid_weekday
        );
    }
}


TEST_CASE("stl_contains", "[stl_contains]") {
    vector<int> container;
    int target = rand_int(10, 20);

    SECTION("empty container") {
        REQUIRE(stl_contains(container, target) == false);
    }

    SECTION("non-empty container") {
        int n_before_target = rand_int(5);
        int n_after_target = rand_int(5);
        container.reserve(n_before_target + 1 + n_after_target);

        for (int i=0; i<n_before_target; i++)
            container.push_back(rand_int(5));
        container.push_back(target);
        for (int i=0; i<n_after_target; i++)
            container.push_back(rand_int(5));

        SECTION("valid") {
            REQUIRE(stl_contains(container, target) == true);
        }

        SECTION("invalid") {
            container.erase(container.begin() + n_before_target);
            REQUIRE(stl_contains(container, target) == false);
        }
    }
}


TEST_CASE("stl_join", "[stl_join]") {
    vector<int> container;
    string expected;

    SECTION("empty container") {
        string actual = stl_join(container);
    }

    SECTION("non-empty container") {
        SECTION("with 1 element") {
            container = {1};
            expected = "1";
        }
        SECTION("with multiple element") {
            container = {1, 2, 3};
            expected = "1,2,3";
        }
    }

    REQUIRE_THAT(stl_join(container), Equals(expected));
}
