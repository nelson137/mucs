#include "mucs.hpp"


void Mucs::admin_dump() {
    this->admin_authenticate();

    if (this->dump_flags == 0)
        this->dump_flags = DumpHomeworks | DumpLabs | DumpRoster | DumpCurrents;

    if (this->dump_flags & DumpCurrents) {
        cout << "Current Assignments:" << endl;
        string hw, lab;
        try { hw = this->config.get_current_hw(); }
        catch (const mucs_exception& me) {}
        try { lab = this->config.get_current_lab(); }
        catch (const mucs_exception& me) {}
        cout << "hw   " << hw << endl;
        cout << "lab  " << lab << endl;
        cout << endl;
    }

    if (this->dump_flags & DumpHomeworks) {
        cout << "Homeworks:" << endl;
        print_table(this->config.homeworks.to_table());
        cout << endl;
    }

    if (this->dump_flags & DumpLabs) {
        cout << "Lab Sessions:" << endl;
        print_table(this->config.lab_sessions.to_table());
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

    string new_pass = prompt_password(prompt.str());

    if (prompt_password("Confirm new password: ") != new_pass)
        throw mucs_exception("Passwords do not match");

    this->update_config_admin_hash(picosha2::hash256_hex_string(new_pass));
}
