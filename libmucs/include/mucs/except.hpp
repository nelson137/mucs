#ifndef MUCS_EXCEPT_HPP
#define MUCS_EXCEPT_HPP


#include <algorithm>
#include <initializer_list>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


class mucs_exception : public exception {

private:
    string message;

public:
    template<typename... String>
    mucs_exception(string msg1, String... msgs) {
        this->message = msg1;
        vector<string> pieces = { msgs... };
        for_each(pieces.begin(), pieces.end(), [&](const string& p) {
            this->message += " " + p;
        });
    }

    const char *what() const noexcept;

    operator string() const;

    friend ostream& operator<<(ostream& os, const mucs_exception& me);

};


#endif
