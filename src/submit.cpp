#include "submit.hpp"


void submit(SubmitOptions& opts) {
    auto& config = Config::get().parse_file(Path(CONFIG_DIR) / opts.course);

    string user = get_user();

    if (not config.roster.count(user))
        throw mucs_exception("User not in course: " + user);

    string assignment = config.get_assignment(opts.assignment_type, user);

    string spacer = string(get_term_width() * TERM_WIDTH_COEFF, '=');
    cout << w_green(spacer) << endl;
    cout << "Course:     " << w_bold(opts.course) << endl;
    cout << "Assignment: " << w_bold(assignment) << endl;
    cout << "User:       " << w_bold(user) << endl;
    cout << "Files:     ";
    for (const auto& s : opts.sources)
        cout << ' ' << w_bold(s);
    cout << endl;
    cout << w_green(spacer) << endl;

    if (prompt_yesno("Are you sure you want to submit [Y/n]? ") == false) {
        cerr << w_red("Submission cancelled") << endl;
        return;
    }

    Path submit_d = Path(SUBMISSIONS_ROOT) / opts.course / assignment / user;
    submit_d.mkdir_recurse();

    Exec::Args ea = {
        "/usr/bin/install", "-C", "-m", "660", "-t", submit_d.str()
    };
    stl_extend<vector<string>>(ea, opts.sources);
    Exec::Ret ret = Exec::execute(ea);

    if (ret.code != 0 && ret.err.size()) {
        cerr << ret.err;
        cout << endl;
    }

    if (ret.code == 0) {
        cout << w_green("Submission complete") << endl;
    } else {
        cerr << w_red("Submission failed") << endl;
    }
}
