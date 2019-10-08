#ifndef CONFIGS_HPP
#define CONFIGS_HPP


#include <fstream>
#include <string>

#include "json.hpp"

#include "mucs/except.hpp"

#include "courseconfig.hpp"
#include "util.hpp"

using namespace std;
using json = nlohmann::json;


class Configs : public json {

public:
    Configs(string config_dir);

    void read_config(string& path);

};


#endif
