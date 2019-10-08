#ifndef MAIN_HPP
#define MAIN_HPP


#include <string>
#include <vector>

#include "CLI11.hpp"

using namespace std;


struct SubmitOptions {
    string course;
    string assignment_type;
    vector<string> sources;
};


#endif
