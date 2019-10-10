#include "test_util.hpp"


/*************************************************
 * temp_file
 ************************************************/


temp_file::temp_file() : temp_file("/tmp") {
}


temp_file::temp_file(const string& dir) {
    static string name_template = "mucs-temp.XXXXXX";

    int size = dir.size() + 1 + name_template.size() + 1;
    char full_template[size];

    snprintf(full_template, size, "%s/%s", dir.c_str(), name_template.c_str());

    if ((this->fd = mkstemp(full_template)) < 0)
        throw mucs_exception(
            "Unable to make temporary file: " + string(full_template));

    close(this->fd);
    this->name = full_template;
}


temp_file::temp_file(const string& dir, const string& name) {
    this->name = dir + '/' + name;
    ofstream(this->name).close();
}


temp_file::~temp_file() noexcept(false) {
    if (remove(this->name.c_str()) < 0)
        throw mucs_exception("Unable to remove file: " + this->name);
}


void temp_file::write(string& data) {
    ofstream fs(this->name);
    fs << data;
    fs.close();
}


void temp_file::write(string&& data) {
    return this->write(data);
}


temp_file::operator string() {
    return this->name;
}


ostream& operator<<(ostream& os, const temp_file& tf) {
    os << tf.name;
    return os;
}


/*************************************************
 * temp_dir
 ************************************************/


temp_dir::temp_dir() : temp_dir("/tmp") {
}


temp_dir::temp_dir(const string& dir) {
    static string name_template = "mucs-temp.XXXXXX";

    int size = dir.size() + 1 + name_template.size() + 1;
    char full_template[size];

    snprintf(full_template, size, "%s/%s", dir.c_str(), name_template.c_str());

    char *full_name = mkdtemp(full_template);
    if (full_name == NULL)
        throw mucs_exception(
            "Unable to make directory: " + string(full_template));
    this->name = full_name;
}


temp_dir::temp_dir(const string& dir, const string& name) {
    this->name = dir + '/' + name;
    if (mkdir(this->name.c_str(), 0700) < 0)
        throw mucs_exception("Unable to make directory: " + this->name);
}


temp_dir::~temp_dir() noexcept(false) {
    if (rmdir(this->name.c_str()) < 0)
        throw mucs_exception("Unable to remove directory: " + this->name);
}


temp_dir::operator string() {
    return this->name;
}


ostream& operator<<(ostream& os, const temp_dir& td) {
    os << td.name;
    return os;
}
