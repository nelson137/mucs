#include "mucs/except.hpp"


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
