#include "mucs.hpp"


void Mucs::submit() {
    if (not config.roster.count(this->user))
        throw mucs_exception("User not in course:", this->user);

    LabSesh lab = config.get_lab(this->user);

    const IAssignment& assignment = config.get_assignment(this->assignment);
    if (not assignment.is_active())
        throw mucs_exception(
            "Submission window is closed for assignment: " + assignment.name);

    // Make sure sources exist and are files
    for (const Path& src : this->sources) {
        if (src.exists() == false)
            throw mucs_exception("Source file does not exist:", src);
        if (src.is_dir())
            throw mucs_exception("Cannot submit directories:", src);
    }

    // Make sure sources compile
    this->compile_sources();

    // Show user a summary of their submission and prompt for confirmation
    this->submit_summary(lab, assignment.name);
    if (this->prompt_yesno("Are you sure you want to submit [y/n]? ") == false)
        throw mucs_exception("Submission cancelled");

    this->copy_submission_files(lab, assignment);
}


void Mucs::copy_submission_files(
    const LabSesh& lab,
    const IAssignment& assignment
) const {
    // SUBMIT_DIR
    Path submit_root = Path(SUBMIT_DIR);
    // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT
    Path assignment_d = submit_root / this->course / lab / assignment.name;

    string now_str = format(DATETIME_EXT_FMT, NOW);
    // .submissions/USER.DATE.TIME
    Path submit_d_rel = Path(".submissions") / this->user + now_str;

    // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT/.submissions/USER_DATE_TIME
    Path submit_d_abs = assignment_d / submit_d_rel;
    if (submit_d_abs.exists())
        throw mucs_exception(
            "Attempted successive submissions too quickly, please try again");

    // Make sure submit directory exists
    submit_d_abs.mkdir_recurse(0770);

    // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT:
    //   USER -> .submissions/USER_DATE_TIME
    (assignment_d / this->user).link_to(submit_d_rel);

    for (const Path& src : this->sources)
        src.copy_into(submit_d_abs, 0440);

    cout << rang::fg::green << "Submission complete" << rang::fg::reset << endl;
}
