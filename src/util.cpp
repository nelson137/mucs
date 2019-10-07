#include "util.hpp"


struct termios tp, orig_tp;


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


string get_username() {
    struct passwd *p = getpwuid(getuid());
    return p == nullptr ? "" : string(p->pw_name);
}


bool input_available() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO+1, &fds, nullptr, nullptr, &tv);
}


vector<string> list_dir(string path) {
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


static void no_echo_tty() {
    tp = orig_tp;
    tp.c_lflag &= ~(ECHO | ECHOCTL);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) < 0)
        die("Error setting tty settings");
    atexit(restore_tty);  // Just in case
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


tuple<string, string> path_split_ext(string path) {
    auto i = path.find_last_of(".");
    if (i == string::npos || i == 0)
        return make_tuple(path, "");
    else
        return make_tuple(path.substr(0, i), path.substr(i));
}


static void restore_tty() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &orig_tp) < 0)
        die("Error resetting tty settings");
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
