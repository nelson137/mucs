#include "mucs/except.hpp"


string mucs_exception::config_msg(
    const string& msg,
    const string& filename,
    initializer_list<string> keys
) {
    ostringstream ret;
    ret << msg << ": " << filename;
    for (const auto& k : keys)
        ret << "[\"" << k << "\"]";
    return ret.str();
}


mucs_exception mucs_exception::config(
    const string& msg,
    const string& filename,
    initializer_list<string> keys
) {
    return mucs_exception(config_msg(msg, filename, keys));
}


const char *mucs_exception::what() const noexcept {
    return this->message.c_str();
}


mucs_exception::operator string() const {
    return this->message;
}


ostream& operator<<(ostream& os, const mucs_exception& me) {
    os << me.message;
    return os;
}
