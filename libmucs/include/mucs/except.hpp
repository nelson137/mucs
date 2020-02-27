#ifndef MUCS_EXCEPT_HPP
#define MUCS_EXCEPT_HPP


#include <algorithm>
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
    mucs_exception(const string& msg1, const String&... msgs) {
        vector<string> pieces = { msgs... };
        ostringstream full_msg;

        full_msg << msg1;
        for (const auto& p : pieces)
            full_msg << ' ' << p;

        this->message = full_msg.str();
    }

    const char *what() const noexcept;

    operator string() const;

    friend ostream& operator<<(ostream& os, const mucs_exception& me);

};


#endif
