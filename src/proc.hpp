#ifndef PROC_HPP
#define PROC_HPP


#include <algorithm>
#include <cctype>
#include <cstring>
#include <initializer_list>
#include <ostream>
#include <string>
#include <vector>

#include <unistd.h>

#include <sys/wait.h>

#include "except.hpp"

#include "path.hpp"

using namespace std;


class Proc {

private:
    vector<string> v_args;
    char **args = nullptr;

    void cleanup();
    char **get_argv();

public:
    struct Ret {
        int code;
        string out;
        string err;
        operator int() const { return this->code; };
    };

    Proc();
    Proc(const initializer_list<string>& il);
    ~Proc();

    void push_back(const string& s);

    template<class Container>
    void extend(const Container& c) {
        this->v_args.reserve(this->v_args.size() + c.size());
        for (const auto& e : c)
            this->v_args.push_back(e);
    }

    const char *bin() const;

    Ret execute();

};


#endif
