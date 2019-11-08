#ifndef PRINTER_HPP
#define PRINTER_HPP


#include <iostream>
#include <string>

using namespace std;


class Printer {

private:
    static bool init_nl;

public:
    static void print(ostream& os, const string& msg);

};


void print(const string& msg);

ostream& spacer(ostream& os);


#endif
