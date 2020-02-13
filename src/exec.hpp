#ifndef EXEC_HPP
#define EXEC_HPP


#include <algorithm>
#include <cctype>
#include <cstring>
#include <ostream>
#include <string>
#include <vector>

#include <unistd.h>

/* #include <sys/type.h> */
#include <sys/wait.h>

#include "mucs/except.hpp"

using namespace std;


namespace Exec {


class Args : public vector<string> {

private:
    bool is_prepared = false;
    int m_argc = 0;
    char **m_argv = nullptr;

public:
    using vector<string>::vector;

    ~Args();

    void prepare();
    const char *bin() const;
    char* const* get() const;

    string& operator[](int i);
    const string& operator[](int i) const;

    friend ostream& operator<<(ostream& os, const Args& ea);

};


struct Ret {
    int code;
    string out;
    string err;
};


Ret execute(Args& args);


}  // end namespace Exec


#endif
