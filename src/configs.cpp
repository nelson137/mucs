#include "configs.hpp"


Configs::Configs(string config_dir) {
    if (not path_exists(config_dir))
        throw mucs_exception("Config directory does not exist: " + config_dir);
    if (not path_is_dir(config_dir))
        throw mucs_exception("Config path must be a directory: " + config_dir);

    string base, ext;
    bool found_config = false;
    for (string child : list_dir(config_dir)) {
        tie(base, ext) = path_split_ext(child);
        if (ext == ".json") {
            found_config = true;
            this->read_config(child);
        }
    }

    if (not found_config)
        throw mucs_exception(
            "Directory contains no json files: " + config_dir);
}


void Configs::read_config(string& path) {
    ifstream fs(path);
    json data;

    try {
        fs >> data;
        fs.close();
    } catch (json::parse_error& pe) {
        fs.close();
        throw mucs_exception("Invalid json: " + path);
    }

    CourseConfig cfg(path, data);
    string course_id = cfg["course_id"].get<string>();
    (*this)[course_id] = cfg;
}
