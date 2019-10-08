#include "exc.hpp"


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


CATCH_TRANSLATE_EXCEPTION(mucs_exception& me) {
    return me.what();
}
