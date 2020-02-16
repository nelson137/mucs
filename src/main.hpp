#ifndef MAIN_HPP
#define MAIN_HPP


#include <iostream>
#include <string>
#include <vector>

#include "CLI11.hpp"

#include "consts.hpp"
#include "submit.hpp"

using namespace std;


template<typename Cmd, typename... Args>
void subcommand(const Cmd& cmd, Args&... args) {
    try {
        cmd(args...);
    } catch (const mucs_exception& me) {
        cerr << me.what() << endl;
    }
}


#endif
