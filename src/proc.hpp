#ifndef PROC_HPP
#define PROC_HPP


#include <algorithm>
#include <cctype>
#include <cstring>
#include <initializer_list>
#include <memory>
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
    struct DynArgs : public vector<string> {
        using vector<string>::vector;

        /**
         * Return a fully dynamic char*[] copy of args for use by exec funcs.
         */
        shared_ptr<char*[]> get();
    };

    DynArgs args;

    bool pipe_output = false;
    bool pipe_stdout = false;
    bool pipe_stderr = false;

public:
    struct Ret {
        int code;
        string out;
        string err;
        operator int() const { return this->code; };
    };

    Proc();
    Proc(const initializer_list<string>& il);

public:
    template<class Container = initializer_list<string>>
    static Proc::Ret quiet_exec(const Container& c) {
        Proc p(c);
        p.capture_output();
        return p.exec();
    }

    /**
     * Set whether the process stdout, stderr, or all output should be captured.
     */
    Proc& capture_stdout(bool value = true);
    Proc& capture_stderr(bool value = true);
    Proc& capture_output(bool value = true);

    /**
     * Append s to the argument list.
     */
    Proc& push_back(const string& s);

    /**
     * Append all elements of c to the argument list.
     */
    template<class Container = initializer_list<string>>
    Proc& extend(const Container& c) {
        this->args.reserve(this->args.size() + c.size());
        this->args.insert(this->args.end(), c.begin(), c.end());
        return *this;
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
    Ret exec();

};


#endif
