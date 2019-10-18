#ifndef SUBMIT_HPP
#define SUBMIT_HPP


#include <string>
#include <vector>

using namespace std;


struct SubmitOptions {
    string course;
    string assignment_type;
    vector<string> sources;
};


void submit_callback(const SubmitOptions& opts);


#endif
