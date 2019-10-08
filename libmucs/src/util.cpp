#include "mucs/util.hpp"


string gen_temp_name(string dir) {
    string fn_s = dir + "/mucs-temp.XXXXXX";
    int size = fn_s.size();

    char fn[size + 1];
    fn_s.copy(fn, size);
    fn[size] = 0;

    mktemp(fn);
    return string(fn);
}
