#include "util.hpp"


time_t current_time() {
    time_t now = time(nullptr);
    tm *t = localtime(&now);
    return t->tm_hour*60*60 + t->tm_min*60 + t->tm_sec;
}


int current_weekday() {
    time_t now = time(nullptr);
    tm *t = localtime(&now);
    return t->tm_wday;
}


string format_time(time_t tt, const string& fmt) {
    tm *t = localtime(&tt);
    ostringstream ret;
    ret << put_time(t, fmt.c_str());
    return ret.str();
}


string format_weekday(int weekday) {
    tm t;
    t.tm_wday = weekday;
    ostringstream ret;
    ret << put_time(&t, "%A");
    return ret.str();
}


int get_term_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col * TERM_WIDTH_COEFF;
}


string get_user() {
    return getpwuid(getuid())->pw_name;
}


system_clock::time_point parse_datetime(const string& dt_str) {
    time_t epoch = 0;
    tm *t = localtime(&epoch);

    istringstream ss(dt_str);
    ss >> get_time(t, "%Y-%m-%d %T");

    if (ss.fail())
        throw mucs_exception("Invalid datetime: " + dt_str);

    return system_clock::from_time_t(mktime(t));
}


time_t parse_time(const string& t_str) {
    time_t epoch = 0;
    tm *t = localtime(&epoch);

    istringstream ss(t_str);
    ss >> get_time(t, "%Y-%m-%d %T");

    if (ss.fail())
        return -1;

    // Adjust for timezone ("00:00:00" broken down should be 24hr-gmtoff)
    return mktime(t) + 24*60*60 + t->tm_gmtoff;
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


tuple<string, string> path_split_ext(const string& path) {
    auto i = path.find_last_of(".");
    if (i == string::npos || i == 0)
        return make_tuple(path, "");
    else
        return make_tuple(path.substr(0, i), path.substr(i));
}


string prompt_user(const string& prompt) {
    if (prompt.size())
        cout << prompt;
    string response;
    cin >> response;
    return response;
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


string string_wrap(string s, const string& prefix, const string& suffix) {
    s.insert(0, prefix);
    s += suffix;
    return s;
}

string w_bold (string s) { return string_wrap(s, FG_BOLD);  }
string w_green(string s) { return string_wrap(s, FG_GREEN); }
string w_red  (string s) { return string_wrap(s, FG_RED);   }


void verify_dir_exists(const string& dirpath) {
    struct stat s;
    if (stat(dirpath.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode) == 0)
            throw mucs_exception(
                "Path exists but is not a directory: " + dirpath);
    } else {
        throw mucs_exception("No such file or directory: " + dirpath);
    }
}
