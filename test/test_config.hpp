#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP


#include <string>

#include "catch.hpp"
#include "json.hpp"

#include "util.hpp"

#include "../src/config.hpp"
#include "../src/path.hpp"

using namespace std;
using Catch::Matchers::Contains;
using Catch::Matchers::Equals;
using Catch::Matchers::StartsWith;
using json = nlohmann::json;


#endif
