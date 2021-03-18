#include "test_except.hpp"


TEST_CASE("mucs_except constructor joins all arguments with a space",
          "[mucs-exception]") {
    string a = rand_string();
    string b = rand_string();
    string c = rand_string();

    ostringstream expected_msg;
    expected_msg << a << ' ' << b << ' ' << c;

    REQUIRE_THROWS_WITH(
        throw mucs_exception(a, b, c),
        expected_msg.str()
    );
}


TEST_CASE("mucs_except::what returns a char* to the message",
          "[mucs-exception]") {
    string expected_msg = rand_string();
    mucs_exception actual(expected_msg);
    REQUIRE(string(actual.what()) == string(expected_msg.c_str()));
}


TEST_CASE("casting a mucs_exception to a string yields the message",
          "[mucs-exception]") {
    string expected_msg = rand_string();
    mucs_exception actual(expected_msg);
    REQUIRE((string)actual == expected_msg);
}


TEST_CASE("operator<<(mucs_except) appends the message to the ostream",
          "[mucs-exception]") {
    string expected_msg = rand_string();
    mucs_exception me(expected_msg);
    ostringstream actual;
    actual << me;
    REQUIRE(actual.str() == expected_msg);
}
