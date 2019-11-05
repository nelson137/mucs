#include "util.hpp"


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


string format_time(time_t t) {
    tm *buf = localtime(&t);
    ostringstream ret;
    ret << put_time(buf, "%I:%M:%S%p");
    return ret.str();
}


string format_weekday(int weekday) {
    tm t;
    t.tm_wday = weekday;
    ostringstream ret;
    ret << put_time(&t, "%A");
    return ret.str();
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


time_t parse_time(const string& t_str) {
    time_t epoch = 0;
    tm t;
    localtime_r(&epoch, &t);

    istringstream s(t_str);
    s >> get_time(&t, "%T");
    if (s.fail())
        return -1;

    // Adjust for timezone ("00:00:00" broken down should be 24hr-gmtoff)
    return mktime(&t) + 24*60*60 + t.tm_gmtoff;
}


int parse_weekday(string w_str) {
    if (w_str.size()) {
        w_str[0] = toupper(w_str[0]);
        transform(w_str.begin()+1, w_str.end(), w_str.begin()+1, ::tolower);
    }
    tm t;
    memset(&t, -1, sizeof(t));
    istringstream(w_str) >> get_time(&t, "%a");
    return t.tm_wday;
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
