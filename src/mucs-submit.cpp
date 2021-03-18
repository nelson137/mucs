#include "mucs.hpp"


void Mucs::submit() {
    this->config.apply_overrides(this->user, this->asgmt_name);

    // Make sure "Student not in course" error is first
    this->config.roster.safe_get(this->user);

    const IAssignment& asgmt =
        this->config.validate_and_get_asgmt(this->asgmt_name);
    const LabSesh& lab_sesh = this->config.validate_and_get_lab(this->user);

    bool in_submit_window;
    switch (asgmt.type()) {
        case IAssignment::Asgmt_Hw:
            in_submit_window = asgmt.is_active();
            break;
        case IAssignment::Asgmt_Lab:
            in_submit_window = lab_sesh.is_active() && asgmt.is_active();
            break;
        default:
            in_submit_window = false;
            throw mucs_exception(
                "Assignment type not implemented:", asgmt.type_name());
            break;
    }

    for (const Path& src : this->sources) {
        if (src.exists() == false)
            throw mucs_exception("Source file does not exist:", src);
        if (src.is_dir())
            throw mucs_exception("Cannot submit directories:", src);
    }

    bool compiles = this->compile_sources();

    this->submit_summary(lab_sesh, asgmt.name, in_submit_window, compiles);

    // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT
    Path asgmt_d = Path(SUBMIT_DIR) / this->course / lab_sesh / asgmt.name;

    bool is_valid = in_submit_window && compiles;
    Path archive_dir(is_valid ? ".valid" : ".invalid");

    string now_str = format("_%Y-%m-%d_%H:%M:%S", NOW);
    // .submissions/USER.DATE.TIME
    Path submit_d_rel = archive_dir / this->user + now_str;

    // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT/.submissions/USER_DATE_TIME
    if ((asgmt_d / submit_d_rel).exists())
        throw mucs_exception(
            "Attempted successive submissions too quickly, please try again");

    this->copy_submission_files(asgmt_d, submit_d_rel, is_valid);
}


void Mucs::copy_submission_files(
    const Path& asgmt_d,
    const Path& submit_d_rel,
    bool create_link
) const {
    Path submit_d_abs = asgmt_d / submit_d_rel;

    // Create the submit directory
    submit_d_abs.mkdir_recurse(0770);

    if (create_link) {
        // SUBMIT_DIR/COURSE/LAB/ASSIGNMENT/
        //   USER -> .submissions/USER_DATE_TIME
        (asgmt_d / this->user).link_to(submit_d_rel);
    }

    for (const Path& src : this->sources)
        src.copy_into(submit_d_abs, 0440);

    cout << rang::fg::green << "Submission complete" << rang::fg::reset << endl;
}
