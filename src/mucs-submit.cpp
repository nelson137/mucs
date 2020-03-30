#include "mucs.hpp"


static Proc::Ret try_compile_sources(const vector<Path>& sources) {
#ifdef MUCS_TEST
    return TEST_SOURCES_COMPILE;
#else
    Proc p = {"/usr/local/bin/compile", "-o", "/dev/null"};
    p.extend(sources);
    return p.execute();
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

    const Proc::Ret& compile_ret = try_compile_sources(this->sources);
    if (compile_ret.code != 0) {
        cout << w_red("Unable to submit, program does not compile") << endl;
        if (compile_ret.err.size())
            cerr << compile_ret.err;
        return;
    }

    submit_summary(
        this->course,
        lab,
        assignment,
        user,
        this->sources
    );

    if (prompt_yesno("Are you sure you want to submit [Y/n]? ") == false)
        throw mucs_exception("Submission cancelled");

    Path assignment_d =
        Path(SUBMISSIONS_ROOT) / this->course / lab / assignment;

    string now_str = format_datetime(NOW, DATETIME_EXT_FMT);
    Path submit_d_rel = Path(".submissions") / user + now_str;
    Path submit_d_abs = assignment_d / submit_d_rel;

    if (submit_d_abs.exists())
        throw mucs_exception(
            "Attempted successive submissions too quickly, please try again");

    submit_d_abs.mkdir_recurse();

    Path latest_link = assignment_d / user;
    if (latest_link.exists() && latest_link.rm() == false)
        throw mucs_exception(
            "Error removing symbolic link:", latest_link.str());
    latest_link.link_to(submit_d_rel);

    for (const Path& src : sources)
        src.copy_into(submit_d_abs, 0440);

    cout << w_green("Submission complete") << endl;
}
