#include "mucs.hpp"


void Mucs::admin_authenticate() const {
    stringstream prompt;
    prompt << "Admin password for " << this->course << ": ";
    string password = this->prompt_password(prompt.str());
    if (picosha2::hash256_hex_string(password) != this->config.admin_hash)
        throw mucs_exception("Password incorrect");
}


void Mucs::dump_currents() const {
    string hw;
    try {
        hw = this->config.get_current_hw();
    } catch (...) {}

    string lab;
    try {
        lab = this->config.get_current_lab();
    } catch (...) {}

    cout << "Current Assignments:" << endl;
    cout << "hw   " << hw << endl;
    cout << "lab  " << lab << endl;
    cout << endl;
}


void Mucs::dump_homeworks() const {
    cout << "Homeworks:" << endl;
    print_table(this->config.homeworks.to_table());
    cout << endl;
}


void Mucs::dump_labs() const {
    cout << "Lab Sessions:" << endl;
    print_table(this->config.lab_sessions.to_table());
    cout << endl;
}


void Mucs::dump_roster() const {
    cout << "Roster:" << endl;
    print_table(this->config.roster.to_table());
    cout << endl;
}


string Mucs::get_user() const {
    return getpwuid(getuid())->pw_name;
}


string Mucs::prompt_password(const string& prompt) const {
    if (prompt.size())
        cout << prompt;

    // Get terminal settings
    termios tty, tty_bak;
    tcgetattr(STDIN_FILENO, &tty_bak);
    tty = tty_bak;

    // Setup terminal for password input
    // https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
    tty.c_iflag &= ~(BRKINT | ICRNL | IXON);
    tty.c_oflag &= ~(OPOST);
    tty.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);

    // Read password
    string password;
    cin >> password;

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &tty_bak);
    cout << endl;
    return password;
}


bool Mucs::prompt_yesno(const string& prompt) const {
    if (prompt.size())
        cout << prompt;
    string response;
    cin >> response;
    cout << endl;
    stl_transform(response, ::tolower);
    return string_strip(response)[0] == 'y';
}


void Mucs::submit_summary(const LabSesh& lab, const string& assignment) const {
    int w = get_term_width() * TERM_WIDTH_COEFF;
    const string spacer = string(min(w, 80), '=');

    #define W_GREEN(x) rang::fg::green << x << rang::fg::reset
    #define W_BOLD(x) rang::style::bold << x << rang::style::reset

    cout << W_GREEN(spacer) << endl;
    cout << "Course     : " << W_BOLD(course) << endl;
    cout << "Lab        : " << W_BOLD(lab) << endl;
    cout << "Assignment : " << W_BOLD(assignment) << endl;
    cout << "User       : " << W_BOLD(this->user) << endl;
    cout << "Files      :";
    for (const Path& s : this->sources)
        cout << ' ' << s.str();
    cout << endl;
    cout << W_GREEN(spacer) << endl;

    #undef W_GREEN
    #undef W_BOLD
}


bool Mucs::try_compile_sources() const {
    Proc p = {COMPILE_SCRIPT, "-o", "/dev/null"};
    p.extend(this->sources);

    Proc::Ret ret = p.execute();
    if (ret.err.size())
        cerr << ret.err;

    return ret.code == 0;
}


void Mucs::update_config_admin_hash(const string& new_hash) const {
    // Generate new config with jq
    Proc p = {
        "/usr/bin/jq", "-M", "--indent", "4",
        ".admin_hash=\"" + new_hash + "\"",
        this->config.filename
    };
    Proc::Ret ret = p.execute();

    if (ret.code != 0) {
        cerr << ret.err;
        throw mucs_exception(
            "Failed to update admin hash for course:", to_string(this->course));
    }

    // Write new config to disk
    ofstream(this->config.filename, ios::out | ios::trunc) << ret.out;
}
