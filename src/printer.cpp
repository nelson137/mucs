#include "printer.hpp"


bool Printer::init_nl = false;


void Printer::print(ostream& os, const string& msg) {
    os << msg << endl << endl;
}


void print(const string& msg) {
    Printer::print(cout, msg);
}


ostream& spacer(ostream& os) {
    return os << endl << endl;
}
