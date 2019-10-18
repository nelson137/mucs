#include "submit.hpp"


#include <iostream>
void submit_callback(const SubmitOptions& opts) {
    cout << "Submit callback:" << endl;
    cout << "  course:     " << opts.course << endl;
    cout << "  assignment: " << opts.assignment_type << endl;
    cout << "  sources:   ";
    for (auto s : opts.sources)
        cout << ' ' << s;
    cout << endl;
}
