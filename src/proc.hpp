#ifndef PROC_HPP
#define PROC_HPP


#include <algorithm>
#include <cctype>
#include <cstring>
#include <initializer_list>
#include <sstream>
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

    /**
     * Free memory allocated for args.
     */
    void cleanup();

    /**
     * Copy v_args into args and return it.
     */
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

    /**
     * Append s to the argument list.
     */
    void push_back(const string& s);

    /**
     * Append all elements of c to the argument list.
     */
    template<class Container>
    void extend(const Container& c) {
        this->v_args.reserve(this->v_args.size() + c.size());
        for (const auto& e : c)
            this->v_args.push_back(e);
    }

    /**
     * Return the first element of the argument list.
     */
    const char *bin() const;

    /**
     * Fork and exec the argument list, return a Ret object.
     * The exit code, stdout, and stderr are captured and stored in the Ret
     * object.
     */
    Ret execute();

};


#endif
