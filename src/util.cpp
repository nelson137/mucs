#include "util.hpp"


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


vector<string> list_dir(const string& path) {
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr)
        throw mucs_exception("Could not list config directory");

    struct dirent *ent;
    struct stat s;
    vector<string> children;
    string child, child_full;

    while ((ent = readdir(dir)) != nullptr) {
        child = string(ent->d_name);
        if (child == "." || child == "..")
            continue;
        child_full = path + "/" + child;
        if (stat(child_full.c_str(), &s) < 0)
            throw mucs_exception("Could not stat file: " + child_full);
        if (S_ISREG(s.st_mode)) {
            children.push_back(child_full);
        } else if (S_ISDIR(s.st_mode)) {
            vector<string> grandchildren = list_dir(child_full);
            children.insert(
                children.end(), grandchildren.begin(), grandchildren.end());
        }
    }

    closedir(dir);
    return children;
}


bool path_exists(const string& path) {
    return access(path.c_str(), F_OK) == 0;
}


bool path_is_valid(const string& path) {
    if (path.find("..") != string::npos)
        return false;
    if (path.find("/") != string::npos)
        return false;
    return true;
}


bool path_is_dir(const string& path) {
    static struct stat s;
    if (stat(path.c_str(), &s) < 0)
        return false;
    return S_ISDIR(s.st_mode);
}


bool path_is_file(const string& path) {
    static struct stat s;
    if (stat(path.c_str(), &s) < 0)
        return false;
    return S_ISREG(s.st_mode);
}


tuple<string, string> path_split_ext(const string& path) {
    auto i = path.find_last_of(".");
    if (i == string::npos || i == 0)
        return make_tuple(path, "");
    else
        return make_tuple(path.substr(0, i), path.substr(i));
}


void rmdir(const string& path) {
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


vector<string> string_split(const string& s, const string& delim) {
    vector<string> tokens;
    size_t pos = 0;
    size_t last = 0;
    size_t delim_size = delim.size();

    while ((pos = s.find(delim, last)) != string::npos) {
        tokens.push_back(s.substr(last, pos-last));
        last = pos + delim_size;
    }
    tokens.push_back(s.substr(last));

    return tokens;
}


void verify_dir_exists(const string& dirpath) {
    struct stat s;
    if (stat(dirpath.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode) == 0)
            die(ERR_NOT_A_DIR + dirpath);
    } else {
        die(ERR_NOT_A_DIR + dirpath);
    }
}
