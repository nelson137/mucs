#include "mucs.hpp"


void Mucs::invoke(void (Mucs::*subcmd)()) {
    this->user = get_user();
    this->config = Config(Path(CONFIG_DIR) / this->course);
    this->config.validate().deserialize();
    this->config.load_roster(Path(ROSTER_DIR));
    (this->*subcmd)();
}


unique_ptr<CLI::App> Mucs::get_cli(const vector<string>& courses_available) {
    // App

    unique_ptr<CLI::App> app(new CLI::App);
    app->require_subcommand();

    auto invoke_wrapper = [this] (void (Mucs::*subcmd)()) {
        return bind(mem_fn(&Mucs::invoke), this, subcmd);
    };

    auto add_course_arg = [this,&courses_available] (CLI::App *app) {
        app->add_option("course", this->course)
            ->required()
            ->check(CLI::IsMember(courses_available));
    };

    // Submit subcommand

    CLI::App *submit_subcmd = app
        ->add_subcommand("submit")
        ->callback(invoke_wrapper(&Mucs::submit));
    add_course_arg(submit_subcmd);
    submit_subcmd
        ->add_option("assignment", this->asgmt_name)
        ->required();
    submit_subcmd
        ->add_option("sources", this->sources)
        ->required();

    // Admin subcommand

    CLI::App *admin_subcmd = app
        ->add_subcommand("admin")
        ->require_subcommand();

    // Admin Dump subcommand

    CLI::App *admin_dump_subcmd = admin_subcmd
        ->add_subcommand("dump")
        ->callback(invoke_wrapper(&Mucs::admin_dump));

    auto register_dump_flag = [&] (const string& opt, Mucs::DumpFlags flag) {
        admin_dump_subcmd->add_flag_callback(opt, [&] () {
            this->dump_flags |= flag;
        });
    };

    register_dump_flag("-o,--overrides",       Mucs::DumpOverrides);
    register_dump_flag("-s,--lab-sessions",    Mucs::DumpLabSessions);
    register_dump_flag("-a,--lab-assignments", Mucs::DumpLabAssignments);
    register_dump_flag("-w,--homeworks",       Mucs::DumpHomeworks);
    register_dump_flag("-r,--roster",          Mucs::DumpRoster);

    add_course_arg(admin_dump_subcmd);

    // Admin Update-Password subcommand

    CLI::App *admin_update_passwd_subcmd = admin_subcmd
        ->add_subcommand("update-password")
        ->callback(invoke_wrapper(&Mucs::admin_update_password));

    add_course_arg(admin_update_passwd_subcmd);

    return app;
}
