#include "util.hpp"


sys_seconds NOW;


sys_seconds current_datetime() {
    time_t now_tt = time(nullptr);
    system_clock::time_point now_tp = system_clock::from_time_t(now_tt);
    seconds utc_offset(localtime(&now_tt)->tm_gmtoff);
    return floor<seconds>(now_tp) + utc_offset;
}


year_month_day get_day(sys_seconds tp) {
    return floor<days>(tp);
}


int get_term_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}


seconds get_time(sys_seconds tp) {
    return tp - floor<days>(tp);
}


weekday get_weekday(sys_seconds tp) {
    return year_month_weekday(get_day(tp)).weekday();
}


/**
 * Uses UID instead of EUID because EUID is affected by the set-uid bit on
 * executables.
 */
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


void print_table(const list<vector<string>>& table, const string& delim) {
    if (table.empty())
        return;

    int n_cols = table.front().size();
    vector<int> widths;
    widths.reserve(n_cols);

    for (int i=0; i<n_cols; i++) {
        widths.push_back(
            max_element(
                table.cbegin(), table.cend(),
                // Must capture `i` by reference to get most up-to-date value
                [&i] (const vector<string>& a, const vector<string>& b) {
                    return a[i].size() < b[i].size();
                }
            )->at(i).size()
        );
    }

    for (const vector<string>& row : table) {
        for (int i=0; i<n_cols; i++)
            cout << setw(widths[i]) << setfill(' ') << left << row[i] << delim;
        cout << endl;
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


string string_strip(string s) {
    static const string whitespace = " \t\n";
    const size_t begin = s.find_first_not_of(whitespace);
    const size_t end = s.find_last_not_of(whitespace);
    return begin == string::npos ? "" : s.substr(begin, end - begin + 1);
}
