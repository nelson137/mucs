#include "mucs.hpp"


bool Mucs::try_compile_sources() {
    Proc p = {COMPILE_SCRIPT, "-o", "/dev/null"};
    p.extend(this->sources);

    Proc::Ret ret = p.execute();
    if (ret.err.size())
        cerr << ret.err;

    return ret.code == 0;
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


void Mucs::submit() {
    if (not config.roster.count(this->user))
        throw mucs_exception("User not in course:", this->user);

    LabSesh lab = config.get_lab(this->user);
    string assignment = config.get_assignment(this->assignment_type);

    // Make sure sources exist
    for (const Path& src : this->sources)
        if (src.exists() == false)
            throw mucs_exception("Source file does not exist:", src);

    // Make sure sources compile
    if (this->try_compile_sources() == false)
        throw mucs_exception("Unable to submit, program does not compile");

    // Show user a summary of their submission and prompt for confirmation
    this->submit_summary(lab, assignment);
    if (this->prompt_yesno("Are you sure you want to submit [Y/n]? ") == false)
        throw mucs_exception("Submission cancelled");

    // /.../SUBMIT_DIR
    Path submit_root = Path(SUBMIT_DIR);
    // /.../SUBMIT_DIR/COURSE/LAB/ASSIGNMENT
    Path assignment_d = submit_root / this->course / lab / assignment;

    string now_str = format_datetime(NOW, DATETIME_EXT_FMT);
    // .submissions/USER.DATE.TIME
    Path submit_d_rel = Path(".submissions") / this->user + now_str;

    // /.../SUBMIT_DIR/COURSE/LAB/ASSIGNMENT/.submissions/USER.DATE.TIME
    Path submit_d_abs = assignment_d / submit_d_rel;
    if (submit_d_abs.exists())
        throw mucs_exception(
            "Attempted successive submissions too quickly, please try again");

    // Make sure submit directory exists
    submit_d_abs.mkdir_recurse(0770);

    // /.../SUBMIT_DIR/COURSE/LAB/ASSIGNMENT:
    //   USER -> .submissions/USER.DATE.TIME
    (assignment_d / this->user).link_to(submit_d_rel);

    for (const Path& src : sources)
        src.copy_into(submit_d_abs, 0440);

    // Fix all directory permissions
    submit_root.chmod_recurse(0770, Path::Dir);

    // Fix all file permissions
    submit_root.chmod_recurse(0440, Path::File);

    cout << w_green("Submission complete") << endl;
}
