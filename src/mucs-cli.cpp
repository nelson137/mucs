#include "mucs.hpp"


void Mucs::invoke(void (Mucs::*subcmd)()) {
    this->user = this->get_user();
    this->config = Config(Path(CONFIG_DIR) / this->course);
    this->config.validate().parse();
    this->config.load_roster(Path(ROSTER_DIR));
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
        ->add_option("assignment", this->assignment)
        ->required();
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

    auto or_dump_flags = [&] (const string& opt, Mucs::DumpFlags flag) {
        admin_dump_subcmd->add_flag_callback(
            opt, [&] () { this->dump_flags |= flag; });
    };
    or_dump_flags("-a,--lab-assignments", Mucs::DumpLabAssignments);
    or_dump_flags("-r,--roster", Mucs::DumpRoster);
    or_dump_flags("-s,--lab-sessions", Mucs::DumpLabSessions);
    or_dump_flags("-w,--homeworks", Mucs::DumpHomeworks);

    // Admin Update Password subcommand

    admin_subcmd
        ->add_subcommand("update-password")
        ->callback(invoke_wrapper(&Mucs::admin_update_password));

    return app;
}
