#include "exc.hpp"


const char *mucs_exception::what() const {
    return this->message.c_str();
}


mucs_exception::operator string() const {
    return (this->message);
}
