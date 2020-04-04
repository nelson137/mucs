#include "cli.hpp"


unique_ptr<CLI::App> prepare_cli(Mucs& mucs) {
    vector<string> configs_available = Path(CONFIG_DIR).ls_base();

    // App

    unique_ptr<CLI::App> app(new CLI::App);
    app->require_subcommand();

    // Submit subcommand

    CLI::App *submit_subcmd = app
        ->add_subcommand("submit")
        ->callback(mucs.get_invoke(&Mucs::submit));
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

    // Admin subcommand

    CLI::App *admin_subcmd = app
        ->add_subcommand("admin")
        ->require_subcommand();

    admin_subcmd
        ->add_option("course", mucs.course)
        ->required()
        ->check(CLI::IsMember(configs_available));

    // Admin Dump subcommand

    CLI::App *admin_dump_subcmd = admin_subcmd
        ->add_subcommand("dump")
        ->callback(mucs.get_invoke(&Mucs::admin_dump));

    admin_dump_subcmd->add_flag_callback("-c,--current-assignments", [&] () {
        mucs.dump_flags |= Mucs::DumpCurrents;
    });
    admin_dump_subcmd->add_flag_callback("-l,--labs", [&] () {
        mucs.dump_flags |= Mucs::DumpLabs;
    });
    admin_dump_subcmd->add_flag_callback("-r,--roster", [&] () {
        mucs.dump_flags |= Mucs::DumpRoster;
    });
    admin_dump_subcmd->add_flag_callback("-w,--homeworks", [&] () {
        mucs.dump_flags |= Mucs::DumpHomeworks;
    });

    // Admin Update Password subcommand

    admin_subcmd
        ->add_subcommand("update-password")
        ->callback(mucs.get_invoke(&Mucs::admin_update_password));

    return app;
}
