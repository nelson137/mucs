#ifndef EXC_HPP
#define EXC_HPP


#include <string>
#include <vector>

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

    const char *what();

    operator string() const;

};


#endif
