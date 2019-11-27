#ifndef MAIN_HPP
#define MAIN_HPP


#include <iostream>
#include <string>

#include "CLI11.hpp"

#include "consts.hpp"
#include "submit.hpp"

using namespace std;


template<typename Func, typename... Args>
void subcommand(const Func& cmd, Args&... args) {
    try {
        cmd(args...);
    } catch (const mucs_exception& me) {
        cerr << me.what() << endl;
    }
}


#endif
