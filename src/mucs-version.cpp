#include "mucs.hpp"


void Mucs::version() {
    cout << "mucs " << MUCS_VERSION_MAJOR << '.' << MUCS_VERSION_MINOR << endl;
    cout << endl;
    cout << "Assignment submission system for the University of Missouri — Columbia, Department of Computer Science." << endl;
    cout << endl;
    cout << "MUCS_ROOT=" << MUCS_ROOT << endl;
    cout << "SUBMIT_DIR=" << SUBMIT_DIR << endl;
    cout << "CONFIG_DIR=" << CONFIG_DIR << endl;
    cout << "ROSTER_DIR=" << ROSTER_DIR << endl;
    cout << "SCHEMA_PATH=" << SCHEMA_PATH << endl;
    cout << "COMPILE_SCRIPT=" << COMPILE_SCRIPT << endl;
}
