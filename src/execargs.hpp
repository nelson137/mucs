#ifndef EXECARGS_HPP
#define EXECARGS_HPP


#include <ostream>
#include <string>
#include <vector>

#include <string.h>

using namespace std;


class ExecArgs {

private:
    int _argc = 0;
    char **_argv = nullptr;
    vector<string> _vec;

public:
    ExecArgs(const vector<string>&  v) : _vec(v) {};
    template<typename... T>
    ExecArgs(const string& arg0, T... args) : _vec({ arg0, args... }) {
    }

    ~ExecArgs();

    int size() const;

    char **prepare();

    friend ostream& operator<<(ostream& os, const ExecArgs& ea);

};


#endif
