#ifndef T_UTIL_HPP
#define T_UTIL_HPP


#include <string>

#include "json.hpp"

#include "mucs/random.hpp"

#include "../src/config.hpp"

using namespace std;
using json = nlohmann::json;


int current_year();

string error_id_unrecognized(const string& user, const string& id);

string error_prop(const string& k, const string& t);

json new_config_data(json j = json::object());


#endif
