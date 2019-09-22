#include "config.hpp"


CourseConfig::CourseConfig(string filename, json data) {
    this->filename = filename;

    json j;

    fstream fs(filename);
    fs >> j;
    fs.close();
}


#include <iostream>
Configs::Configs(string config_dir) {
    if (not path_exists(config_dir))
        throw mucs_exception("not exists: " + config_dir);
    if (not path_is_dir(config_dir))
        throw mucs_exception("not dir");
}
