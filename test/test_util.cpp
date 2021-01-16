#include "test_util.hpp"


TEST_CASE("join_paths correctly joins arguments", "[util][join_paths]") {
    SECTION("2 empty args") {
        REQUIRE(join_paths("", "") == "");
    }

    SECTION("1 empty arg, 1 not empty arg") {
        REQUIRE(join_paths("a", "") == "a");
        REQUIRE(join_paths("", "a") == "a");
    }

    SECTION("the joining slash is already part of >=1 of the args") {
        string expected = "a/b";
        REQUIRE(join_paths("a",  "/b") == expected);
        REQUIRE(join_paths("a/",  "b") == expected);
        REQUIRE(join_paths("a/", "/b") == expected);
    }

    SECTION("more than 2 args") {
        string expected = "a/b";
        REQUIRE(join_paths("a", "b") == expected);
        expected += "/c";
        REQUIRE(join_paths("a", "b", "c") == expected);
        expected += "/d";
        REQUIRE(join_paths("a", "b", "c", "d") == expected);
    }
}


TEST_CASE("stl_join returns an empty string for an empty container",
          "[util][stl_join]") {
    vector<string> c;
    REQUIRE(stl_join(c) == "");
}


TEST_CASE("stl_join doesn't include delimiter for a container with 1 element",
          "[util][stl_join]") {
    string element = rand_string();
    vector<string> c = { element };
    REQUIRE(stl_join(c) == element);
}


TEST_CASE("stl_join correctly joins the elements of container with delimiter",
          "[util][stl_join]") {
    string e1 = rand_string(), e2 = rand_string(), e3 = rand_string();
    vector<string> c = { e1, e2, e3 };
    REQUIRE(stl_join(c) == e1 + "," + e2 + "," + e3);
}


TEST_CASE("stl_join correctly uses the provided delimiter",
          "[util][stl_join]") {
    string e1 = rand_string(), e2 = rand_string(), e3 = rand_string();
    vector<string> c = { e1, e2, e3 };
    REQUIRE(stl_join(c, "|") == e1 + "|" + e2 + "|" + e3);
}


TEST_CASE("string_split returns a vector with just the given string if it "
              "doesn't contain delimiter",
          "[util][string_split") {
    string e = rand_string();
    vector<string> expected = { e };
    REQUIRE(string_split(e, ",") == expected);
}


TEST_CASE("string_split correctly splits a string that contains >=1 delimiter",
          "[util][string_split") {
    string e1 = rand_string(), e2 = rand_string(), e3 = rand_string();
    string input = e1 + "," + e2 + "," + e3;
    vector<string> expected = { e1, e2, e3 };
    REQUIRE(string_split(input, ",") == expected);
}


TEST_CASE("string_strip returns an empty string if the input is empty",
          "[util][string_strip]") {
    REQUIRE("" == string_strip(""));
}


TEST_CASE("string_strip correctly removes whitespace",
          "[util][string_strip]") {
    string whitespace = rand_string(10, chars_whitespace);
    string c(1, rand_char());
    REQUIRE("" == string_strip(whitespace));
    REQUIRE(c == string_strip(whitespace + c));
    REQUIRE(c == string_strip(c + whitespace));
    REQUIRE(c == string_strip(whitespace + c + whitespace));
}
