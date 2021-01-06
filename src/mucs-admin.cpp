#include "mucs.hpp"


void Mucs::admin_authenticate() const {
    stringstream prompt;
    prompt << "Admin password for " << this->course << ": ";
    string password = this->prompt_password(prompt.str());
    if (picosha2::hash256_hex_string(password) != this->config.admin_hash)
        throw mucs_exception("Password incorrect");
}


void Mucs::admin_dump() {
    this->admin_authenticate();

    if (this->dump_flags == 0)
        this->dump_flags =
            DumpLabSessions | DumpHomeworks | DumpLabAssignments | DumpRoster;

    if (this->dump_flags & DumpLabSessions) {
        cout << "Lab Sessions:" << endl;
        print_table(this->config.lab_sessions.to_table());
        cout << endl;
    }

    if (this->dump_flags & DumpLabAssignments) {
        cout << "Lab Assignments:" << endl;
        print_table(this->config.lab_assignments.to_table());
        cout << endl;
    }

    if (this->dump_flags & DumpHomeworks) {
        cout << "Homeworks:" << endl;
        print_table(this->config.homeworks.to_table());
        cout << endl;
    }

    if (this->dump_flags & DumpRoster) {
        cout << "Roster:" << endl;
        print_table(this->config.roster.to_table());
        cout << endl;
    }
}


void Mucs::admin_update_password() {
    this->admin_authenticate();

    stringstream prompt;
    prompt << "New password for " << this->course << ": ";

    string new_pass = this->prompt_password(prompt.str());

    if (this->prompt_password("Confirm new password: ") != new_pass)
        throw mucs_exception("Passwords do not match");

    this->update_config_admin_hash(picosha2::hash256_hex_string(new_pass));
}
