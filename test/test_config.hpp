#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP


#include <string>

#include "catch.hpp"
#include "json.hpp"

#include "mucs/except.hpp"
#include "mucs/temp_file.hpp"

#include "util.hpp"

#include "../src/config.hpp"

using namespace std;
using Catch::Matchers::Equals;
using json = nlohmann::json;


#endif
