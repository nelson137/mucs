#ifndef TEMP_FILE_HPP
#define TEMP_FILE_HPP


#include <cstdio>
#include <fstream>
#include <string>

#include <unistd.h>

#include <sys/stat.h>

#include "mucs/except.hpp"

using namespace std;


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


#endif
