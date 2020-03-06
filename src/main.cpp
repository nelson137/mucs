#include "main.hpp"


int main(int argc, char **argv) {
    SubmitOptions submit_opts = {};

    vector<string> configs = Path(CONFIG_DIR).ls();

    CLI::App app;

    app.require_subcommand();

    CLI::App *submit_subcmd = app
        .add_subcommand("submit")
        ->callback([&] () {
            submit(submit_opts);
        });
    submit_subcmd
        ->add_option("course", submit_opts.course)
        ->required()
        ->check(CLI::IsMember(configs));
    submit_subcmd
        ->add_option("assignment_type", submit_opts.assignment_type)
        ->required()
        ->check(CLI::IsMember({ "hw", "lab" }));
    submit_subcmd
        ->add_option("sources", submit_opts.sources)
        ->required();

    try {
        CLI11_PARSE(app, argc, argv);
    } catch (const mucs_exception& me) {
        cerr << me.what() << endl;
    }

    return 0;
}
