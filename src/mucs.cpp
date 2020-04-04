#include "mucs.hpp"


void Mucs::admin_authenticate() {
    stringstream prompt;
    prompt << "Admin password for " << this->course << ": ";
    string password = this->prompt_password(prompt.str());
    if (picosha2::hash256_hex_string(password) != this->config.admin_hash)
        throw mucs_exception("Password incorrect");
}


string Mucs::prompt_password(const string& prompt) {
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


bool Mucs::prompt_yesno(const string& prompt) {
    if (prompt.size())
        cout << prompt;
    string response;
    cin >> response;
    cout << endl;
    stl_transform(response, ::tolower);
    return string_strip(response) == "y";
}


void Mucs::submit_summary(const LabSesh& lab, const string& assignment) {
    int w = get_term_width() * TERM_WIDTH_COEFF;
    const string spacer = string(min(w, 80), '=');

    cout << w_green(spacer) << endl;
    cout << "Course     : " << w_bold(course) << endl;
    cout << "Lab        : " << w_bold(lab) << endl;
    cout << "Assignment : " << w_bold(assignment) << endl;
    cout << "User       : " << w_bold(this->user) << endl;
    cout << "Files      :";
    for (const Path& s : this->sources)
        cout << ' ' << w_bold(s);
    cout << endl;
    cout << w_green(spacer) << endl;
}


bool Mucs::try_compile_sources() {
    Proc p = {COMPILE_SCRIPT, "-o", "/dev/null"};
    p.extend(this->sources);

    Proc::Ret ret = p.execute();
    if (ret.err.size())
        cerr << ret.err;

    return ret.code == 0;
}


void Mucs::update_config_admin_hash(const string& new_hash) {
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
