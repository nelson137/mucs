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
        this->dump_flags = DumpOverrides | DumpLabSessions | DumpHomeworks
            | DumpLabAssignments | DumpRoster;

    struct D {
          DumpFlags flag;     const char *label;  const Tabular& model;
    };

    vector<D> dumps = {
        D{DumpOverrides,      "Overrides:",       this->config.overrides},
        D{DumpLabSessions,    "Lab Sessions:",    this->config.lab_sessions},
        D{DumpLabAssignments, "Lab Assignments:", this->config.lab_assignments},
        D{DumpHomeworks,      "Homeworks:",       this->config.homeworks},
        D{DumpRoster,         "Roster:",          this->config.roster},
    };

    for (const D& d : dumps) {
        if (this->dump_flags & d.flag) {
            cout << d.label << endl;
            print_table(d.model.to_table());
            cout << endl;
        }
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
