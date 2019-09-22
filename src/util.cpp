#include "util.hpp"


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


bool path_exists(string path) {
    return access(path.c_str(), F_OK) == 0;
}


bool path_is_valid(string& path) {
    if (path.find("..") != string::npos)
        return false;
    if (path.find("/") != string::npos)
        return false;
    return true;
}


bool path_is_dir(string path) {
    static struct stat s;
    if (stat(path.c_str(), &s) < 0)
        return false;
    return S_ISDIR(s.st_mode);
}


bool path_is_file(string path) {
    static struct stat s;
    if (stat(path.c_str(), &s) < 0)
        return false;
    return S_ISREG(s.st_mode);
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
