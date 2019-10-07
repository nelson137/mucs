#ifndef TEST_COURSECONFIG_HPP
#define TEST_COURSECONFIG_HPP


#include <iostream>  // XXX
#include <string>

#include "catch.hpp"
#include "json.hpp"

#include "test_util.hpp"

// System under test
#include "../src/courseconfig.hpp"
#include "../src/exc.hpp"

using namespace std;
using Catch::Matchers::Equals;
using json = nlohmann::json;


#endif
