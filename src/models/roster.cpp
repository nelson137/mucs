#include "config.hpp"


void Roster::insert(string user, string lab_id) {
    if (this->count(user) == 0)
        map<string, string>::insert({ user, lab_id });
    else
        throw mucs_exception(
            "Student cannot be in lab sessions '" + this->at(user) + "' and '"
            + lab_id + "': " + user);
}


const string& Roster::safe_get(const string& user) const {
    auto it = this->find(user);
    if (it == this->end())
        throw mucs_exception("Student not in course:", user);
    return it->second;
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
