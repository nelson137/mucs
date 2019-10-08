#ifndef MUCS_EXCEPT_HPP
#define MUCS_EXCEPT_HPP


#include <ostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;


#define USAGE              "Usage: mucs-submit SUBMIT_DIR ASSIGNMENT " \
                               "USERNAME SOURCE_DIR"
#define ERR_DIR_NOT_FOUND  "Directory not found: "
#define ERR_FORK           "Could not fork"
#define ERR_INVALID_PARAM  "Parameters course, assignment, and username may " \
                               "not contain '..' or '/': "
#define ERR_MKDIR          "Unable to make directory: "
#define ERR_NOT_A_DIR      "Path exists but is not directory: "


class mucs_exception {

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

    const char *what() const noexcept;

    operator string() const;

    friend ostream& operator<<(ostream& os, const mucs_exception& me);

};


#endif
