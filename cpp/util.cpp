#include "util.hpp"


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


bool path_is_valid(string& path) {
    if (path.find("..") != string::npos)
        return false;
    if (path.find("/") != string::npos)
        return false;
    return true;
}


void rmdir(string& path) {
    ExecArgs ea("rm", "-rf", path);
    int ret;

    pid_t pid = fork();
    if (pid < 0) {
        // Error forking
        die(ERR_FORK);
    } else if (pid == 0) {
        // Child
        execv("/bin/rm", ea.prepare());
        _exit(1);
    } else {
        // Parent
        wait(&ret);
    }
}


void verify_dir_exists(string& dirpath) {
    struct stat s;
    if (stat(dirpath.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode) == 0)
            die(ERR_NOT_A_DIR + dirpath);
    } else {
        die(ERR_NOT_A_DIR + dirpath);
    }
}
