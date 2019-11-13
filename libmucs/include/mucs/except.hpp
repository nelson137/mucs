#ifndef MUCS_EXCEPT_HPP
#define MUCS_EXCEPT_HPP


#include <ostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;


class mucs_exception : public exception {

private:
    string message;

public:
    template<typename... T>
    mucs_exception(string msg1, T... msgs) {
        this->message = "";
        vector<string> pieces = { msg1, msgs... };
        if (pieces.size()) {
            this->message += pieces[0];
            for (auto it=pieces.begin()+1; it != pieces.end(); it++)
                this->message += " " + *it;
        }
    }

    const char *what() const noexcept {
        return this->message.c_str();
    }

    operator string() const {
        return this->message;
    }

    friend ostream& operator<<(ostream& os, const mucs_exception& me) {
        os << me.message;
        return os;
    }

};


#endif
