#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP


#include <cstdio>
#include <fstream>
#include <ostream>
#include <string>

#include <unistd.h>

#include <sys/stat.h>

#include "mucs/util.hpp"
#include "mucs/except.hpp"

using namespace std;


class temp_file {

public:
    string name;

    temp_file();
    temp_file(string name);
    ~temp_file() noexcept(false);

    void write(string&  data);
    void write(string&& data);

};


class temp_dir {

public:
    string name;

    temp_dir();
    ~temp_dir() noexcept(false);

};


#endif
