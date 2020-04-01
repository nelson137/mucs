#include "test_util.hpp"


TEST_CASE("join_paths", "[util][join_paths]") {
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


TEST_CASE("parse_datetime", "[util][parse_datetime]") {
    tm expected;
    // tm_year = years since 1900
    expected.tm_year = rand_int(2000, 2020) - 1900;
    // tm_mon: Jan=0, Feb=1, ...
    expected.tm_mon = rand_int(12);
    expected.tm_mday = rand_int(1, 29);
    expected.tm_hour = rand_int(24);
    expected.tm_min = rand_int(60);
    expected.tm_sec = rand_int(60);

    string expected_str;

    SECTION("valid") {
        expected_str = format_datetime(expected, DATETIME_FMT);

        auto parsed_dt = parse_datetime(expected_str);
        time_t parsed_tt = system_clock::to_time_t(parsed_dt);
        tm *actual = localtime(&parsed_tt);

        REQUIRE(actual->tm_year == expected.tm_year);
        REQUIRE(actual->tm_mon == expected.tm_mon);
        REQUIRE(actual->tm_mday == expected.tm_mday);
        REQUIRE(actual->tm_hour == expected.tm_hour);
        REQUIRE(actual->tm_min == expected.tm_min);
        REQUIRE(actual->tm_mday == expected.tm_mday);
    }

    SECTION("invalid") {
        expected.tm_sec = rand_int(-59, 0);
        expected_str = format_datetime(expected, DATETIME_FMT);

        REQUIRE_THROWS_WITH(
            parse_datetime(expected_str),
            "Invalid datetime: " + expected_str
        );
    }
}


TEST_CASE("parse_time", "[util][parse_time]") {
    tm expected;
    expected.tm_hour = rand_int(24);
    expected.tm_min = rand_int(60);
    expected.tm_sec = rand_int(60);

    string expected_str;

    SECTION("valid") {
        expected_str = format_datetime(expected, TIME_FMT);

        system_clock::time_point parsed_tt = parse_time(expected_str);
        time_t actual_t = system_clock::to_time_t(parsed_tt);
        tm *actual = localtime(&actual_t);

        REQUIRE(actual->tm_hour == expected.tm_hour);
        REQUIRE(actual->tm_min == expected.tm_min);
        REQUIRE(actual->tm_sec == expected.tm_sec);
    }

    SECTION("invalid") {
        expected.tm_sec = rand_int(-59, 0);
        expected_str = format_datetime(expected, TIME_FMT);

        REQUIRE_THROWS_WITH(
            parse_time(expected_str),
            "Invalid time: " + expected_str
        );
    }
}


TEST_CASE("parse_weekday", "[util][parse_weekday]") {
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


TEST_CASE("stl_contains", "[util][stl_contains]") {
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


TEST_CASE("stl_join", "[util][stl_join]") {
    vector<string> container;
    string expected;

    SECTION("empty container") {
        string actual = stl_join(container);
    }

    SECTION("non-empty container") {
        SECTION("with 1 element") {
            container = {"a"};
            expected = "a";
        }
        SECTION("with multiple element") {
            container = {"a", "b", "c"};
            expected = "a,b,c";
        }
    }

    REQUIRE_THAT(stl_join(container), Equals(expected));
}


TEST_CASE("string_split", "[util][string_split]") {
    string test_str;
    vector<string> expected;

    SECTION("empty string") {
        expected = {""};
    }

    SECTION("non-empty string") {
        SECTION("with 1 element") {
            test_str = "a";
            expected = {"a"};
        }
        SECTION("with multiple elements") {
            test_str = "a,b,c";
            expected = {"a", "b", "c"};
        }
    }

    REQUIRE(string_split(test_str, ",") == expected);
}


TEST_CASE("string_strip", "[util][string_strip]") {
    string test_str, expected;

    SECTION("empty string") {
    }

    SECTION("all whitespace") {
        test_str = " \t\n";
        expected = "";
    }

    SECTION("whitespace prefix") {
        test_str = " \t\na";
        expected = "a";
    }
    SECTION("whitespace suffix") {
        test_str = "b \t\n";
        expected = "b";
    }

    REQUIRE_THAT(string_strip(test_str), Equals(expected));
}
