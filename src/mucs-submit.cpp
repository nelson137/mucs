#include "mucs.hpp"


void submit_summary(
    const string& course,
    const LabSesh& lab,
    const string& assignment,
    const string& user,
    const vector<string>& sources
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
    Path submit_d = assignment_d / submit_d_rel;

    if (submit_d.exists())
        throw mucs_exception(
            "Attempted successive submissions too quickly, please try again");
    else
        submit_d.mkdir_recurse();

    Path latest_link = assignment_d / user;
    if (latest_link.exists() && latest_link.rm() == false)
        throw mucs_exception(
            "Error removing symbolic link:", latest_link.str());
    latest_link.link_to(submit_d_rel);

    Proc p = {
        "/usr/bin/install", "-C", "-m", "440", "-t", submit_d.str()
    };
    p.extend(this->sources);
    Proc::Ret ret = p.execute();

    if (ret.code != 0 && ret.err.size()) {
        cerr << ret.err;
        cout << endl;
    }

    if (ret.code == 0)
        cout << w_green("Submission complete") << endl;
    else
        throw mucs_exception("Submission failed");
}
