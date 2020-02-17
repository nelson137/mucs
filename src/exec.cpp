#include "exec.hpp"


namespace Exec {


Args::~Args() {
    if (this->m_argv) {
        for (unsigned i=0; i<this->size(); i++)
            delete[] this->m_argv[i];
        delete[] this->m_argv;
    }
}


void Args::prepare() {
    if (this->m_argv)
        return;

    int argc = this->m_argc = this->size() + 1;
    char **argv = this->m_argv = new char*[argc];

    int n;
    int i = 0;
    for (auto& a : *this) {
        n = a.size() + 1;
        argv[i] = new char[n];
        strncpy(argv[i++], a.c_str(), n);
    }

    argv[argc-1] = nullptr;

    this->is_prepared = true;
}


const char *Args::bin() const {
    return this->is_prepared ? this->m_argv[0] : nullptr;
}


char* const* Args::get() const {
    return this->is_prepared ? this->m_argv : nullptr;
}


string& Args::operator[](int i) {
    return vector<string>::at(i);
}


const string& Args::operator[](int i) const {
    return vector<string>::at(i);
}


ostream& operator<<(ostream& os, const Args& ea) {
    auto contains_whitespace = [](const string& s) {
        return any_of(s.begin(), s.end(), [](char c){ return isspace(c); });
    };
    auto wrap = [&](string s) {
        if (contains_whitespace(s))
            return '\'' + s + '\'';
        return s;
    };

    if (ea.size())
        os << wrap(ea[0]);
    for (unsigned i=1; i<ea.size(); i++)
        os << ' ' << wrap(ea[i]);

    return os;
}


bool read_fd(int fd, string& dest) {
    static char buff[256];
    int count;

    while ((count = read(fd, buff, sizeof(buff)-1)) > 0) {
        buff[count] = '\0';
        dest += buff;
    }

    return count == 0;
}


Ret execute(Args& args) {
    int pipe_out[2] = { -1, -1 };
    int pipe_err[2] = { -1, -1 };

    pipe(pipe_out);
    pipe(pipe_err);

    const int PARENT_OUT = pipe_out[0];
    const int PARENT_ERR = pipe_err[0];
    const int CHILD_OUT = pipe_out[1];
    const int CHILD_ERR = pipe_err[1];

    pid_t pid;
    int ret = 0;
    string out, err;

    if ((pid = fork()) < 0) {
        throw mucs_exception("Unable to fork");
    } else if (pid == 0) {
        // Child

        dup2(CHILD_OUT, STDOUT_FILENO);
        dup2(CHILD_ERR, STDERR_FILENO);

        close(PARENT_OUT);
        close(PARENT_ERR);
        close(CHILD_OUT);
        close(CHILD_ERR);

        args.prepare();
        execv(args.bin(), args.get());
    } else {
        // Parent

        close(CHILD_OUT);
        close(CHILD_ERR);

        int wstatus;
        waitpid(pid, &wstatus, 0);
        ret = WEXITSTATUS(wstatus);

        if (not read_fd(PARENT_OUT, out))
            throw mucs_exception("Error reading process stdout");
        if (not read_fd(PARENT_ERR, err))
            throw mucs_exception("Error reading process stderr");

        close(PARENT_OUT);
        close(PARENT_ERR);
    }

    return { ret, out, err };
}


}  // end namespace exec
