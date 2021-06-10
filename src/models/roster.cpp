#include "config.hpp"


void Roster::safe_insert(const string& user, string lab_id) {
    if (this->count(user))
        throw mucs_exception(
            "Student cannot be in lab sessions '" + this->at(user) + "' and '"
            + lab_id + "': " + user);
    map::insert({ user, lab_id });
}


string& Roster::safe_get(const string& user) {
    auto it = this->find(user);
    if (it == this->end())
        throw mucs_exception("Student not in course:", user);
    return it->second;
}


const string& Roster::safe_get(const string& user) const {
    return const_cast<Roster*>(this)->safe_get(user);
}


list<vector<string>> Roster::to_table() const {
    list<vector<string>> table;
    for (auto it=this->begin(); it!=this->end(); it++) {
        // Create row
        vector<string> row;
        row.reserve(2);
        // Column 1
        row.push_back(it->first);
        // Column 2
        row.push_back(it->second);
        // Append row
        table.push_back(move(row));
    }
    return table;
}
