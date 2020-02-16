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


string format_datetime(const tm& t, const string& fmt) {
    size_t size = sizeof(char) * 32;
    char *buf = new char[size];
    while (strftime(buf, size, fmt.c_str(), &t) == 0) {
        delete[] buf;
        size *= 2;
        buf = new char[size];
    }
    string ret(buf);
    delete[] buf;
    return ret;
}


string format_datetime(time_t tt, const string& fmt) {
    return format_datetime(*localtime(&tt), fmt);
}


string format_weekday(int weekday) {
    tm t;
    t.tm_wday = weekday;
    return format_datetime(t, "%A");
}


int get_term_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}


string get_user() {
    return getpwuid(getuid())->pw_name;
}


string join_paths(string a, deque<string> parts) {
    string b = parts.front();
    parts.pop_front();

    if (not a.size()) return b;
    if (not b.size()) return a;

    if (a.back() == '/' && b.front() == '/')
        a.erase(a.end() - 1);
    else if (a.back() != '/' && b.front() != '/')
        a += "/";

    return parts.size()
        ? join_paths(a + b, parts)
        : a + b;
}


system_clock::time_point parse_datetime(const string& dt_str) {
    tm t;
    if (strptime(dt_str.c_str(), "%Y-%m-%d %T", &t) == nullptr)
        throw mucs_exception("Invalid datetime: " + dt_str);
    return system_clock::from_time_t(mktime(&t));
}


time_t parse_time(const string& t_str) {
    tm t;
    return strptime(t_str.c_str(), "%T", &t) == nullptr
        ? -1
        : mktime(&t);
}


int parse_weekday(const string& w_str) {
    tm t;
    if (strptime(w_str.c_str(), "%a", &t) == nullptr)
        throw mucs_exception("Invalid weekday: " + w_str);
    return t.tm_wday;
}


bool prompt_yesno(const string& prompt) {
    if (prompt.size())
        cout << prompt;
    string response;
    cin >> response;
    cout << endl;
    stl_transform(response, ::tolower);
    return string_strip(response) == "y";
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


string string_strip(string s) {
    static const string whitespace = " \t\n";
    const size_t begin = s.find_first_not_of(whitespace);
    const size_t end = s.find_last_not_of(whitespace);
    return begin == string::npos ? "" : s.substr(begin, end - begin + 1);
}


string w_bold (const string& s) { return FG_BOLD  + s + FG_RESET; }
string w_green(const string& s) { return FG_GREEN + s + FG_RESET; }
string w_red  (const string& s) { return FG_RED   + s + FG_RESET; }
