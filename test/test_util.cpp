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
