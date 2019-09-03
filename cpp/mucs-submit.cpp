#include <string>
#include <vector>

#include <string.h>
#include <unistd.h>

using namespace std;


class ExecArgs {

private:
    int _argc = -1;
    char **_argv = nullptr;

public:
    ExecArgs(const vector<string>&  v) { init(v); }
    ExecArgs(const vector<string>&& v) { init(v); }

    void init(const vector<string>& vec) {
        int size = this->_argc = vec.size() + 1;
        char **argv = this->_argv = static_cast<char**>(
            malloc(size * sizeof(char*))
        );

        int len;
        for (int i=0; i<size-1; i++) {
            len = strlen(vec[i].c_str()) + 1;
            argv[i] = static_cast<char*>(
                malloc(len * sizeof(char))
            );
            strncpy(argv[i], vec[i].c_str(), len);
        }

        argv[size-1] = nullptr;
    }

    ~ExecArgs() {
        for (int i=0; i<this->_argc; i++)
            free(this->_argv[i]);
        free(this->_argv);
    }

    char **get() const {
        return this->_argv;
    }

};


int main(int argc, char **argv) {
    if (argc < 3)
        return 1;

    vector<string> files(argv+2, argv+argc);

    vector<string> cmd_argv;
    cmd_argv.reserve(8 + files.size());
    cmd_argv = {
        "install", "-C",
        "-g", "cs1050-ta",
        "-m", "660",
        "-t", argv[1]
    };
    cmd_argv.insert(cmd_argv.end(), files.begin(), files.end());

    ExecArgs ea(cmd_argv);
    return execv("/usr/bin/install", ea.get());
}
