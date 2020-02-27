#include "util.hpp"


string error_id_unrecognized(
    const string& fn,
    const string& user,
    const string& id
) {
    return "Lab id not recognized: " + fn + "[\"roster\"][\"" + user + "\"]";
}


string error_prop(const string& fn, const string& k, const string& t) {
    return "Config requires key \"" + k + "\" with type " + t + ": " + fn;
}
