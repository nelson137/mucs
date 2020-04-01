#include "mucs.hpp"


void Mucs::admin_dump() {
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
