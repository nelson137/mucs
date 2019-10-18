#include "main.hpp"


/*
int main_old(int argc, char **argv) {
    if (argc != 5)
        die(USAGE);

    string submit_root = "/group/cs1050/submissions";
    string course      = argv[1];
    string assignment  = argv[2];
    string username    = argv[3];
    string source_d    = argv[4];

    verify_dir_exists(submit_root);
    verify_paths(course, assignment, username);
    verify_dir_exists(source_d);

    // submit_root/course/assignment/username
    string submit_d = join_paths(submit_root, course, assignment, username);

    struct stat s;
    if (stat(submit_d.c_str(), &s) == 0)
        rmdir(submit_d);

    make_path(submit_root, course, assignment, username);

    // Find doesn't work unless you cd into the starting point for some reason
    chdir(source_d.c_str());

    // Copy files
    ExecArgs ea(
        "find", ".", "-type", "f", "-exec",
        "/usr/bin/install", "-C", "-g", "cs1050-ta",
        "-m", "660", "-t", submit_d, "{}",
        ";"
    );
    return execv("/usr/bin/find", ea.prepare());
}
*/

int main(int argc, char **argv) {
    CLI::App app;

    app.require_subcommand();

    auto submit_opts = make_shared<SubmitOptions>();
    CLI::App *submit_subcmd = app
        .add_subcommand("submit")
        ->callback([submit_opts](){ submit_callback(*submit_opts); });
    submit_subcmd
        ->add_option("course", submit_opts->course)
        ->required();
    submit_subcmd
        ->add_option("assignment_type", submit_opts->assignment_type)
        ->required();
    submit_subcmd
        ->add_option("sources", submit_opts->sources)
        ->required();

    CLI11_PARSE(app, argc, argv);
    return 0;
}
