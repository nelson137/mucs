#include "cli.hpp"


unique_ptr<CLI::App> prepare_cli(Mucs& mucs) {
    vector<string> configs_available = Path(CONFIG_DIR).ls();

    auto app = unique_ptr<CLI::App>(new CLI::App);

    app->require_subcommand();

    CLI::App *submit_subcmd = app
        ->add_subcommand("submit")
        ->callback(mucs.invoke(&Mucs::submit));
    submit_subcmd
        ->add_option("course", mucs.course)
        ->required()
        ->check(CLI::IsMember(configs_available));
    submit_subcmd
        ->add_option("assignment_type", mucs.assignment_type)
        ->required()
        ->check(CLI::IsMember({ "hw", "lab" }));
    submit_subcmd
        ->add_option("sources", mucs.sources)
        ->required();

    return app;
}
