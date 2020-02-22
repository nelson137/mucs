#include "submit.hpp"


void submit(SubmitOptions& opts) {
    auto& config = Config::get().parse_file(Path(CONFIG_DIR) / opts.course);

    string user = get_user();

    if (not config.roster.count(user))
        throw mucs_exception("User not in course: " + user);

    vector<string> user_labs = config.roster[user];
    string lab;
    if (user_labs.size() == 1) {
        lab = user_labs[0];
        auto ls = config.lab_sessions[lab];
        if (ls.is_active() == false)
            throw mucs_exception(ls.format(
                "Lab {id} is not in session: {weekday} from {start} to {end}"
            ));
    } else {
        auto active_lab = stl_find_if(user_labs, [&] (const string& id) {
            return config.lab_sessions[id].is_active();
        });
        if (active_lab == user_labs.end())
            throw mucs_exception(
                "None of your labs are in session:", stl_join(user_labs));
        lab = *active_lab;
    }

    string assignment = config.get_assignment(opts.assignment_type);

    string spacer = string(get_term_width() * TERM_WIDTH_COEFF, '=');
    cout << w_green(spacer) << endl;
    cout << "Course:     " << w_bold(opts.course) << endl;
    cout << "Lab:        " << w_bold(lab) << endl;
    cout << "Assignment: " << w_bold(assignment) << endl;
    cout << "User:       " << w_bold(user) << endl;
    cout << "Files:     ";
    for (const auto& s : opts.sources)
        cout << ' ' << w_bold(s);
    cout << endl;
    cout << w_green(spacer) << endl;

    if (prompt_yesno("Are you sure you want to submit [Y/n]? ") == false)
        throw mucs_exception("Submission cancelled");

    Path assignment_d =
        Path(SUBMISSIONS_ROOT) / opts.course / lab / assignment;

    string now_str = format_datetime(system_clock::now(), ".%Y-%m-%d.%T");
    Path submit_d = assignment_d / ".submissions" / user + now_str;

    if (submit_d.exists())
        throw mucs_exception(
            "Attempted successive submissions too quickly, please try again");
    else
        submit_d.mkdir_recurse();

    Path latest_link = assignment_d / user;
    if (latest_link.exists())
        if (latest_link.rm() == false)
            throw mucs_exception(
                "Error removing symbolic link:", latest_link.str());
    latest_link.link_to(submit_d);

    Exec::Args ea = {
        "/usr/bin/install", "-C", "-m", "440", "-t", submit_d.str()
    };
    stl_extend<vector<string>>(ea, opts.sources);
    Exec::Ret ret = Exec::execute(ea);

    if (ret.code != 0 && ret.err.size()) {
        cerr << ret.err;
        cout << endl;
    }

    if (ret.code == 0)
        cout << w_green("Submission complete") << endl;
    else
        throw mucs_exception("Submission failed");
}
