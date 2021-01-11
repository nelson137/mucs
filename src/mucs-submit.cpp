#include "mucs.hpp"


void Mucs::submit() {
    this->config.apply_overrides(this->user, this->asgmt_name);

    // Make sure "Student not in course" error is first
    this->config.roster.safe_get(this->user);

    const IAssignment& asgmt =
        this->config.validate_and_get_asgmt(this->asgmt_name);
    const LabSesh& lab_sesh = this->config.validate_and_get_lab(this->user);

    if (not asgmt.is_active())
        throw mucs_exception(
            "Submission window is closed for assignment: " + asgmt.name);
    if (asgmt.type() == IAssignment::Asgmt_Lab && not lab_sesh.is_active())
        throw mucs_exception(lab_sesh.format(
            "Lab {id} is not in session: {weekday} from {start} to {end}"
        ));

    for (const Path& src : this->sources) {
        if (src.exists() == false)
            throw mucs_exception("Source file does not exist:", src);
        if (src.is_dir())
            throw mucs_exception("Cannot submit directories:", src);
    }

    if (not this->compile_sources())
        throw mucs_exception("Program doesn't compile");

    this->submit_summary(lab_sesh, asgmt.name);
    if (this->prompt_yesno("Are you sure you want to submit [y/n]? ") == false)
        throw mucs_exception("Submission cancelled");

    // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT
    Path asgmt_d = Path(SUBMIT_DIR) / this->course / lab_sesh / asgmt.name;

    string now_str = format("_%Y-%m-%d_%H:%M:%S", NOW);
    // .submissions/USER.DATE.TIME
    Path submit_d_rel = Path(".submissions") / this->user + now_str;

    // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT/.submissions/USER_DATE_TIME
    if ((asgmt_d / submit_d_rel).exists())
        throw mucs_exception(
            "Attempted successive submissions too quickly, please try again");

    this->copy_submission_files(asgmt_d, submit_d_rel);
}


void Mucs::copy_submission_files(
    const Path& asgmt_d,
    const Path& submit_d_rel
) const {
    Path submit_d_abs = asgmt_d / submit_d_rel;

    // Create the submit directory
    submit_d_abs.mkdir_recurse(0770);

    // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT/
    //   USER -> .submissions/USER_DATE_TIME
    (asgmt_d / this->user).link_to(submit_d_rel);

    for (const Path& src : this->sources)
        src.copy_into(submit_d_abs, 0440);

    cout << rang::fg::green << "Submission complete" << rang::fg::reset << endl;
}
