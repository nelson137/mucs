#include "config.hpp"


void Roster::insert(string pawprint, string lab_id) {
    if (this->count(pawprint) == 0)
        map<string, vector<string>>::insert({ pawprint, {lab_id} });
    else
        this->at(pawprint).push_back(lab_id);
}


const vector<string>& Roster::safe_get(const string& user) const {
    auto it = this->find(user);
    if (it == this->end())
        throw mucs_exception("User not in course:", user);
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
        row.push_back(stl_join(it->second));
        // Append row
        table.push_back(move(row));
    }
    return table;
}
