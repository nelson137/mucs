#include "submit.hpp"


Submitter::Submitter(const Path& sub_d) : submit_d(sub_d) {
    if (this->submit_d.exists())
        this->submit_d.rm();

    submit_d.mkdir_recurse();
}


bool Submitter::submit(const vector<string>& sources) {
    Exec::Args ea = {
        "/usr/bin/install", "-C", "-m", "660", "-t", this->submit_d.str()
    };
    stl_extend<vector<string>>(ea, sources);
    Exec::Ret ret = Exec::execute(ea);

    if (ret.code != 0 && ret.err.size()) {
        cerr << ret.err;
        cout << endl;
    }

    return ret.code == 0;
}


void submit_command(SubmitOptions& opts) {
    IConfig config;
#ifdef MUCS_TEST
    config = MockConfig();
#else
    config = Config(Path(CONFIG_DIR) / opts.course);
#endif

    string user = get_user();
    string assignment;

    try {
        if (opts.assignment_type == "lab")
            assignment = config.get_current_lab(user);
        else if (opts.assignment_type == "hw")
            assignment = config.get_current_hw();
        else
            throw mucs_exception("Not possible, caught by parser");
    } catch (const mucs_exception& me) {
        cerr << me.what() << endl;
        return;
    }

    string spacer = string(get_term_width(), '=');
    cout << w_green(spacer) << endl;
    cout << "Course:     " << w_bold(opts.course) << endl;
    cout << "Assignment: " << w_bold(assignment) << endl;
    cout << "User:       " << w_bold(user) << endl;
    cout << "Files:     ";
    for (auto& s : opts.sources)
        cout << ' ' << w_bold(s);
    cout << endl;
    cout << w_green(spacer) << endl;

    string input = prompt_user("Are you sure you want to submit [Y/n]? ");
    cout << endl;
    stl_transform(input, ::tolower);
    if (input != "y") {
        cerr << w_red("Submission cancelled") << endl;
        return;
    }

    auto submit_d = Path(SUBMISSIONS_ROOT) / opts.course / assignment / user;

    Submitter submitter(submit_d);
    if (submitter.submit(opts.sources)) {
        cout << w_green("Submission complete") << endl;
    } else {
        cerr << w_red("Submission failed") << endl;
    }
}
