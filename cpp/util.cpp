#include "util.hpp"


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


template<typename... T>
string join_paths(string a, string b, T... rest) {
    vector<string> components = { b, rest... };
    string p = a;
    for (auto c : components)
        p += "/" + c;
    return p;
}


template<typename... T>
void make_path(string base, T... comps) {
    struct stat s;
    string path = base;
    vector<string> components = { comps... };

    for (auto c : components) {
        path = join_paths(path, c);
        if(stat(path.c_str(), &s) == 0) {   // If path exists
            if (S_ISDIR(s.st_mode) == 0)    //   If not a directory
                die(ERR_NOT_A_DIR + path);  //     Error
        } else {                            // If path doesn't exist
            if (mkdir(path.c_str(), 0770))  //   Attempt to make directory
                die(ERR_MKDIR + path);      //     Error if failed
        }
    }
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


template<typename... T>
void verify_paths(string comp1, T... ts) {
    vector<string> paths = { comp1, ts... };
    for (auto p : paths) {
        if (not path_is_valid(p))
            die(ERR_INVALID_PARAM + p);
    }
}
