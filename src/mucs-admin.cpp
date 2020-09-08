#include "mucs.hpp"


void Mucs::admin_dump() {
    this->admin_authenticate();

    if (this->dump_flags == 0)
        this->dump_flags =
            DumpLabSessions | DumpHomeworks | DumpLabAssignments | DumpRoster;

    if (this->dump_flags & DumpLabSessions)
        this->dump_lab_sessions();

    if (this->dump_flags & DumpLabAssignments)
        this->dump_lab_assignments();

    if (this->dump_flags & DumpHomeworks)
        this->dump_homeworks();

    if (this->dump_flags & DumpRoster)
        this->dump_roster();
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
