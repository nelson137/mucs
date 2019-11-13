#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP


#include <cstdio>
#include <fstream>
#include <ostream>
#include <string>

#include <unistd.h>

#include <sys/stat.h>

#include "json.hpp"

#include "mucs/except.hpp"
#include "mucs/random.hpp"

using namespace std;
using json = nlohmann::json;


class temp_file {

public:
    int fd;
    string name;

    temp_file();

    temp_file(const string& dir);

    temp_file(const string& dir, const string& name);

    ~temp_file() noexcept(false);

    void write(const string& data);

    operator string();

    friend temp_file& operator<<(temp_file& tf, const string& data);

};


json new_config_data();


#endif
