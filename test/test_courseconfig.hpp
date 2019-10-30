#ifndef TEST_COURSECONFIG_HPP
#define TEST_COURSECONFIG_HPP


#include <string>

#include "catch.hpp"
#include "json.hpp"

#include "mucs/except.hpp"

#include "test_util.hpp"

#include "../src/courseconfig.hpp"

using namespace std;
using json = nlohmann::json;


string error_missing_prop(const string& fn, const string& k, const string& t);

string error_incorrect_type(
    const string& fn,
    const string& k,
    const string& t
);

string error_id_unrecognized(
    const string& fn,
    const string& user,
    const string& id
);


#endif
