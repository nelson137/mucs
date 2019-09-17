#include "execargs.hpp"


ExecArgs::~ExecArgs() {
    if (this->_argv) {
        for (int i=0; i<this->size(); i++)
            free(this->_argv[i]);
        free(this->_argv);
    }
}


int ExecArgs::size() const {
    return this->_vec.size();
}


char **ExecArgs::prepare() {
    if (this->_argv)
        return this->_argv;

    int argc = this->_argc = this->size() + 1;
    char **argv = this->_argv = static_cast<char**>(
        malloc(argc * sizeof(char*))
    );

    int size;
    for (int i=0; i<argc-1; i++) {
        size = this->_vec[i].size() + 1;
        argv[i] = static_cast<char*>(
            malloc(size * sizeof(char))
        );
        strncpy(argv[i], this->_vec[i].c_str(), size);
    }

    argv[argc-1] = nullptr;

    return argv;
}


ostream& operator<<(ostream& os, ExecArgs& ea) {
    if (ea._argc < 0 || ea._argv == nullptr)
        return os;

    auto contains_whitespace = [](char *s) {
        for (; *s != '\0'; s++) {
            if (*s == ' ' || *s == '\t' || *s == '\n')
                return true;
        }
        return false;
    };
    auto wrap = [&](char *s) {
        return contains_whitespace(s)
            ? '\'' + string(s) + '\''
            : s;
    };

    if (ea._argc > 0)
        os << wrap(ea._argv[0]);
    for (int i=1; i<ea._argc-1; i++)
        os << string(" ") << wrap(ea._argv[i]);

    return os;
}
