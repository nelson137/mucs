#include "mucs.hpp"


void Mucs::submit() {
    if (not config.roster.count(this->user))
        throw mucs_exception("User not in course:", this->user);

    LabSesh lab = config.get_lab(this->user);
    string assignment = config.get_assignment(this->assignment_type);

    // Make sure sources exist and are files
    for (const Path& src : this->sources) {
        if (src.exists() == false)
            throw mucs_exception("Source file does not exist:", src);
        if (src.is_dir())
            throw mucs_exception("Cannot submit directories:", src);
    }

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
