#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP


#include <string>

#include "catch.hpp"
#include "json.hpp"

#include "mucs/except.hpp"

#include "test_util.hpp"

#include "../src/config.hpp"

using namespace std;
using json = nlohmann::json;


string error_prop(const string& fn, const string& k, const string& t);

string error_id_unrecognized(
    const string& fn,
    const string& user,
    const string& id
);


#endif
