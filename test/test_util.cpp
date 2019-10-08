#include "test_util.hpp"


/*************************************************
 * temp_file
 ************************************************/


temp_file::temp_file() : temp_file(gen_temp_name()) {
}


temp_file::temp_file(string name) {
    this->name = name;

    // Touch the file
    ofstream f(this->name);
}


temp_file::~temp_file() noexcept(false) {
    if (remove(this->name.c_str()) < 0)
        throw mucs_exception("Unable to remove file: " + this->name);
}


void temp_file::write(string& data) {
    ofstream(this->name) << data;
}


void temp_file::write(string&& data) {
    return this->write(data);
}


/*************************************************
 * temp_dir
 ************************************************/


temp_dir::temp_dir() {
    this->name = gen_temp_name();

    // Make the directory
    if (mkdir(this->name.c_str(), 0755) < 0)
        throw mucs_exception("Unable to make directory: " + this->name);
}


temp_dir::~temp_dir() noexcept(false) {
    if (rmdir(this->name.c_str()) < 0)
        throw mucs_exception("Unable to remove directory: " + this->name);
}
