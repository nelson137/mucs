#include "mucs.hpp"


void Mucs::invoke(void (Mucs::*subcmd)()) {
    this->user = this->get_user();
    this->config = Config(Path(CONFIG_DIR) / this->course);
    this->config.validate().parse();
    (this->*subcmd)();
}


unique_ptr<CLI::App> Mucs::get_cli() {
    // App

    unique_ptr<CLI::App> app(new CLI::App);
    app->require_subcommand();

    auto invoke_wrapper = [this] (void (Mucs::*subcmd)()) {
        return bind(mem_fn(&Mucs::invoke), this, subcmd);
    };

    // Submit subcommand

    CLI::App *submit_subcmd = app
        ->add_subcommand("submit")
        ->callback(invoke_wrapper(&Mucs::submit));
    submit_subcmd
        ->add_option("course", this->course)
        ->required()
        ->check(CLI::IsMember(CONFIGS_AVAILABLE));
    submit_subcmd
        ->add_option("assignment_type", this->assignment_type)
        ->required()
        ->check(CLI::IsMember({ "hw", "lab" }));
    submit_subcmd
        ->add_option("sources", this->sources)
        ->required();

    // Admin subcommand

    CLI::App *admin_subcmd = app
        ->add_subcommand("admin")
        ->require_subcommand();

    admin_subcmd
        ->add_option("course", this->course)
        ->required()
        ->check(CLI::IsMember(CONFIGS_AVAILABLE));

    // Admin Dump subcommand

    CLI::App *admin_dump_subcmd = admin_subcmd
        ->add_subcommand("dump")
        ->callback(invoke_wrapper(&Mucs::admin_dump));

    admin_dump_subcmd->add_flag_callback("-c,--current-assignments", [&] () {
        this->dump_flags |= Mucs::DumpCurrents;
    });
    admin_dump_subcmd->add_flag_callback("-l,--labs", [&] () {
        this->dump_flags |= Mucs::DumpLabs;
    });
    admin_dump_subcmd->add_flag_callback("-r,--roster", [&] () {
        this->dump_flags |= Mucs::DumpRoster;
    });
    admin_dump_subcmd->add_flag_callback("-w,--homeworks", [&] () {
        this->dump_flags |= Mucs::DumpHomeworks;
    });

    // Admin Update Password subcommand

    admin_subcmd
        ->add_subcommand("update-password")
        ->callback(invoke_wrapper(&Mucs::admin_update_password));

    return app;
}
