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
