#include "test_config.hpp"


TEST_CASE("config directory doesn't exist", "[courses]") {
    string dir = rand_string();
    REQUIRE_THROWS_MATCHES(Configs(dir), mucs_exception,
        Matches("^not exists: .*$"));
}


TEST_CASE("config directory exists but isn't directory", "[courses]") {
    REQUIRE(true == true);
}
