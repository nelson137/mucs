#include "mucs.hpp"


bool Mucs::compile_sources() const {
    auto p = Proc().capture_stdout();
    if (this->should_use_make())
        p.push_back(MAKE_PATH);
    else
        p.extend({ COMPILE_SCRIPT, "-o/dev/null" }).extend(this->sources);

    return p.exec().code == 0;
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


bool Mucs::should_use_make() const {
    // Check if in a git repo
    Proc::Ret ret = Proc::quiet_exec({
        GIT_PATH, "rev-parse", "--is-inside-work-tree" });
    if (ret.code != 0)
        return false;

    // Get the origin URL
    ret = Proc::quiet_exec({
        GIT_PATH, "config", "--get", "remote.origin.url" });
    if (ret.code != 0)
        return false;

    // Check that the origin URL matches
    // Note: the regex has to be a 100% match, hence the .* at the end
    regex re("(https://|git@)github\\.com/JimRies1966/.*",
        regex_constants::egrep);
    if (regex_match(ret.out, re) == false)
        return false;

    // Check that ./Makefile exists
    if (Path("./Makefile").exists() == false)
        return false;

    return true;
}


void Mucs::submit_summary(
    const LabSesh& lab,
    const string& asgmt_name,
    bool in_submit_window,
    bool compiles
) const {
    int w = get_term_width() * TERM_WIDTH_COEFF;
    const string spacer = string(min(w, 80), '=');

    auto color = rang::fg::green;
    if (not in_submit_window) color = rang::fg::yellow;
    else if (not compiles)    color = rang::fg::red;
    #define W_COLOR(x) color << x << rang::fg::reset
    #define W_BOLD(x) rang::style::bold << x << rang::style::reset

    cout << W_COLOR(spacer) << endl;
    if (not compiles)
    cout << "Note       : " << W_BOLD("COMPILATION FAILS") << endl;
    if (not in_submit_window)
    cout << "Note       : " << W_BOLD("OUTSIDE SUBMISSION WINDOW") << endl;
    cout << "Course     : " << W_BOLD(this->course) << endl;
    cout << "Lab        : " << W_BOLD(lab) << endl;
    cout << "Assignment : " << W_BOLD(asgmt_name) << endl;
    cout << "User       : " << W_BOLD(this->user) << endl;
    cout << "Files      : " << W_BOLD(stl_join(this->sources, ", ")) << endl;
    cout << W_COLOR(spacer) << endl;

    #undef W_COLOR
    #undef W_BOLD
}


void Mucs::update_config_admin_hash(const string& new_hash) const {
    Proc p = {
        "/bin/sed",
        "-i",
        "s/"+this->config.admin_hash+"/"+new_hash+"/",
        this->config.filename
    };
    Proc::Ret ret = p.exec();

    if (ret.code != 0) {
        cerr << ret.err;
        throw mucs_exception(
            "Failed to update admin hash for course:", this->course);
    }
}
