#include "mucs.hpp"


static bool try_compile_sources(const vector<Path>& sources) {
#ifdef MUCS_TEST
    return TEST_SOURCES_COMPILE;
#else
    Proc p = {"/usr/local/bin/compile", "-o", "/dev/null"};
    p.extend(sources);

    Proc::Ret ret = p.execute();
    if (ret.err.size())
        cerr << ret.err;

    return ret.code == 0;
#endif
}


static void submit_summary(
    const string& course,
    const LabSesh& lab,
    const string& assignment,
    const string& user,
    const vector<Path>& sources
) {
#ifndef MUCS_TEST
    const string spacer = string(get_term_width() * TERM_WIDTH_COEFF, '=');

    cout << w_green(spacer) << endl;
    cout << "Course:     " << w_bold(course) << endl;
    cout << "Lab:        " << w_bold(lab) << endl;
    cout << "Assignment: " << w_bold(assignment) << endl;
    cout << "User:       " << w_bold(user) << endl;
    cout << "Files:     ";
    for (const auto& s : sources)
        cout << ' ' << w_bold(s);
    cout << endl;
    cout << w_green(spacer) << endl;
#endif
}


void Mucs::submit(const Config& config) {
    string user = get_user();
    if (not config.roster.count(user))
        throw mucs_exception("User not in course:", user);

    LabSesh lab = config.get_lab(user);
    string assignment = config.get_assignment(this->assignment_type);

    if (try_compile_sources(this->sources) == false)
        throw mucs_exception("Unable to submit, program does not compile");

    submit_summary(this->course, lab, assignment, user, this->sources);
    if (prompt_yesno("Are you sure you want to submit [Y/n]? ") == false)
        throw mucs_exception("Submission cancelled");

    Path assignment_d = Path(SUBMIT_DIR) / this->course / lab / assignment;

    string now_str = format_datetime(NOW, DATETIME_EXT_FMT);
    Path submit_d_rel = Path(".submissions") / user + now_str;

    Path submit_d_abs = assignment_d / submit_d_rel;
    if (submit_d_abs.exists())
        throw mucs_exception(
            "Attempted successive submissions too quickly, please try again");

    (assignment_d / user).link_to(submit_d_rel);

    for (const Path& src : sources)
        src.copy_into(submit_d_abs, 0440);

    cout << w_green("Submission complete") << endl;
}
