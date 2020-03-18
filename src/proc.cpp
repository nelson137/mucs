#include "proc.hpp"


static bool read_fd(int fd, string& dest) {
    static char buff[256];
    int count;

    while ((count = read(fd, buff, sizeof(buff)-1)) > 0) {
        buff[count] = '\0';
        dest += buff;
    }

    return count == 0;
}


void Proc::cleanup() {
    if (this->args) {
        for (unsigned i=0; i<this->v_args.size(); i++)
            delete[] this->args[i];
        delete[] this->args;
    }
    this->args = nullptr;
}


char **Proc::get_argv() {
    if (this->args != nullptr)
        this->cleanup();

    int argc = this->v_args.size() + 1;
    this->args = new char*[argc];

    int n;
    int i = 0;
    for (const auto& a : this->v_args) {
        n = a.size() + 1;
        this->args[i] = new char[n];
        strncpy(this->args[i], a.c_str(), n);
        i++;
    }

    this->args[argc-1] = nullptr;

    return this->args;
}


Proc::Proc() {
}


Proc::Proc(const initializer_list<string>& il) : v_args(il) {
}


Proc::~Proc() {
    this->cleanup();
}


void Proc::push_back(const string& s) {
    this->v_args.push_back(s);
}


const char *Proc::bin() const {
    return this->v_args.size() ? this->v_args.at(0).c_str() : nullptr;
}


Proc::Ret Proc::execute() {
    int pipe_out[2] = { -1, -1 };
    int pipe_err[2] = { -1, -1 };

    if (pipe(pipe_out) != 0 || pipe(pipe_err) != 0)
        throw mucs_exception("Unable to create pipe");

    static constexpr int PARENT = 0;
    static constexpr int CHILD  = 1;

    pid_t pid;
    int code = 0;
    string out, err;

    if ((pid = fork()) < 0) {
        throw mucs_exception("Unable to fork");
    } else if (pid == 0) {
        // Child

        close(pipe_out[PARENT]);
        close(pipe_err[PARENT]);

        dup2(pipe_out[CHILD], STDOUT_FILENO);
        dup2(pipe_err[CHILD], STDERR_FILENO);

        close(pipe_out[CHILD]);
        close(pipe_err[CHILD]);

        execv(this->bin(), this->get_argv());
    } else {
        // Parent

        int wstatus;
        waitpid(pid, &wstatus, 0);
        code = WEXITSTATUS(wstatus);

        close(pipe_out[CHILD]);
        close(pipe_err[CHILD]);

        if (not read_fd(pipe_out[PARENT], out))
            throw mucs_exception("Error reading process stdout");
        if (not read_fd(pipe_err[PARENT], err))
            throw mucs_exception("Error reading process stderr");

        close(pipe_out[PARENT]);
        close(pipe_err[PARENT]);
    }

    return Ret{code, out, err};
}
