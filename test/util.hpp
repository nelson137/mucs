#ifndef T_UTIL_HPP
#define T_UTIL_HPP


#include <string>

#include "json.hpp"

#include "mucs/random.hpp"

#include "../src/config.hpp"

using namespace std;
using json = nlohmann::json;


struct NewConfig {

    json j;

    operator    json() const { return this->j; }
    operator Config&() const { return Config::get().parse(this->j); }

};


string error_id_unrecognized(
    const string& fn,
    const string& user,
    const string& id
);

string error_prop(const string& fn, const string& k, const string& t);


template<typename Ret>
Ret new_config(json j = json::object()) {
    Config::get().filename = rand_string();

    auto default_val = [&] (const string& key, const json& val) {
        if (j.count(key) == 0)
            j[key] = val;
    };

    default_val("course_id", rand_string(4));
    default_val("admin_hash", "!");
    default_val("homeworks", json::object());
    default_val("labs", json::object());
    default_val("current_lab", "");
    default_val("roster", json::object());

    Ret ret = NewConfig{j};
    return ret;
}


#endif
