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


void temp_file::write(const string& data) {
    ofstream fs(this->name);
    fs << data;
    fs.close();
}


temp_file::operator string() {
    return this->name;
}


temp_file& operator<<(temp_file& tf, const string& data) {
    tf.write(data);
    return tf;
}
