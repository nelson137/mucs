#include "proc.hpp"


static bool read_fd(int fd, ostringstream& dest) {
    static char buff[4096];
    int count;

    while ((count = read(fd, buff, sizeof(buff)-1)) > 0) {
        buff[count] = '\0';
        dest << buff;
    }

    return count == 0;
}


shared_ptr<char*[]> Proc::DynArgs::get() {
    size_t argc = this->size();
    auto argv = shared_ptr<char*[]>(
        // New memory
        new char*[argc+1],
        // Deleter func
        [=] (char *argv[]) {
            if (argv) {
                for (size_t i=0; i<argc; i++)
                    delete[] argv[i];
                delete[] argv;
            }
        }
    );

    int n, i = 0;
    for (const string& a : *this) {
        n = a.size() + 1;
        argv[i] = new char[n];
        strncpy(argv[i], a.c_str(), n);
        i++;
    }
    argv[argc] = nullptr;

    return argv;
}


Proc::Proc() {
}


Proc::Proc(const initializer_list<string>& il) : args(il) {
}


Proc& Proc::capture_stdout(bool value) {
    this->pipe_stdout = value;
    return *this;
}


Proc& Proc::capture_stderr(bool value) {
    this->pipe_stderr = value;
    return *this;
}


Proc& Proc::capture_output(bool value) {
    this->pipe_output = value;
    return *this;
}


Proc& Proc::push_back(const string& s) {
    this->args.emplace_back(s.c_str());
    return *this;
}


const char *Proc::bin() const {
    return this->args.size() ? this->args.at(0).c_str() : nullptr;
}


Proc::Ret Proc::exec() {
    if (this->args.empty())
        throw mucs_exception("Arguments are required to execute");

    Path exe(this->args.at(0));
    if (not exe.exists())
        throw mucs_exception("Failed to find executable:", exe.str());
    if (not exe.is_exe())
        throw mucs_exception("File is not executable:", exe.str());

    int pipe_out[2] = { -1, -1 };
    int pipe_err[2] = { -1, -1 };

    bool pipe_stdout = this->pipe_output || this->pipe_stdout;
    bool pipe_stderr = this->pipe_output || this->pipe_stderr;

    if ((pipe_stdout && pipe(pipe_out) < 0)
     || (pipe_stderr && pipe(pipe_err) < 0))
        throw mucs_exception("Unable to create pipe");

    static constexpr int PARENT = 0;
    static constexpr int CHILD  = 1;

    pid_t pid;
    int code = 0;
    ostringstream oss_out, oss_err;

    if ((pid = fork()) < 0) {
        throw mucs_exception("Unable to fork");
    } else if (pid == 0) {
        // Child

        close(pipe_out[PARENT]);
        close(pipe_err[PARENT]);

        if (pipe_stdout)
            dup2(pipe_out[CHILD], STDOUT_FILENO);
        if (pipe_stderr)
            dup2(pipe_err[CHILD], STDERR_FILENO);

        close(pipe_out[CHILD]);
        close(pipe_err[CHILD]);

        execv(this->bin(), this->args.get().get());
    } else {
        // Parent

        close(pipe_out[CHILD]);
        close(pipe_err[CHILD]);

        int wstatus;
        waitpid(pid, &wstatus, 0);
        code = WEXITSTATUS(wstatus);

        if (pipe_stdout && not read_fd(pipe_out[PARENT], oss_out))
            throw mucs_exception("Error reading process stdout");
        if (pipe_stderr && not read_fd(pipe_err[PARENT], oss_err))
            throw mucs_exception("Error reading process stderr");

        close(pipe_out[PARENT]);
        close(pipe_err[PARENT]);
    }

    return Ret{code, oss_out.str(), oss_err.str()};
}
