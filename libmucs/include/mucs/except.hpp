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
    template<typename... T>
    mucs_exception(string msg1, T... msgs) {
        this->message = msg1;
        vector<string> pieces = { msgs... };
        for_each(pieces.begin(), pieces.end(), [&](const string& p) {
            this->message += " " + p;
        });
    }

    template<typename... String>
    static string config_msg(
        const string& msg,
        const string& filename,
        const String&... pack_keys
    ) {
        vector<string> keys = { pack_keys... };
        ostringstream ret;
        ret << msg << ": " << filename;
        for (const auto& k : keys)
            ret << "[\"" << k << "\"]";
        return ret.str();
    }

    template<typename... String>
    static mucs_exception config(
        const string& msg,
        const string& filename,
        const String&... pack_keys
    ) {
        return mucs_exception(
            mucs_exception::config_msg(msg, filename, pack_keys...)
        );
    }

    const char *what() const noexcept;

    operator string() const;

    friend ostream& operator<<(ostream& os, const mucs_exception& me);

};


#endif
