#ifndef CONFIG_HPP
#define CONFIG_HPP


#include <fstream>

#include "exc.hpp"
#include "util.hpp"

#include "json.hpp"

using namespace std;
using json = nlohmann::json;


class CourseConfig {

private:

public:
    string filename;

    CourseConfig(string filename, json data);

};


class Configs {

private:

public:
    Configs(string config_dir);

};


#endif
