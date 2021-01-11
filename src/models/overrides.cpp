#include "config.hpp"


OLab::OLab() {
}


OLab::OLab(const OLab& o) : user(o.user), session(o.session) {
    delete this->asgmt;
    this->asgmt = o.asgmt ? new string(o.asgmt->c_str()) : nullptr;
}


OLab::OLab(string u, string s) : user(u), session(s) {
}


OLab::OLab(string u, string s, const string& a_name) : OLab(u, s) {
    this->asgmt = new string(a_name.c_str());
}


OLab::~OLab() {
    delete this->asgmt;
}


OLab& OLab::operator=(const OLab& o) {
    if (this == &o)
        return *this;
    this->user = o.user;
    this->session = o.session;
    delete this->asgmt;
    this->asgmt = o.asgmt ? new string(o.asgmt->c_str()) : nullptr;
    return *this;
}


bool OLab::asgmt_matches(const string& asgmt_name) const {
    return not this->asgmt || *this->asgmt == asgmt_name;
}


OHomework::OHomework() {
}


OHomework::OHomework(string u, string n) : user(u), name(n) {
}


/**
 * Row format for lab overrides: {"USER in lab session LAB_ID for LAB_ASGMT"}
 * Note: specifying the lab assignment is optional, so the " for LAB_ASGMT"
 *   might not be present
 * Row format for homework overrides: {NAME, "HW_NAME1,..."}
 */
list<vector<string>> Overrides::to_table() const {
    list<vector<string>> table;
    for (const OLab& ol : this->o_labs) {
        // Create row
        vector<string> row;
        // Column 1
        ostringstream desc;
        desc << ol.user << " in lab session " << ol.session;
        if (ol.asgmt)
            desc << *ol.asgmt;
        row.push_back(desc.str());
        // Append row
        table.push_back(move(row));
    }
    for (const OHomework& oh : this->o_homeworks) {
        // Create row
        vector<string> row;
        // Column 1
        row.push_back(oh.user);
        // Column 2
        row.push_back(oh.name);
        // Append row
        table.push_back(move(row));
    }
    return table;
}


void from_json(const json& j, OLab& o_lab) {
    o_lab.user = j["user"];
    o_lab.session = j["session"];
    if (j.contains("assignment"))
        o_lab.asgmt = new string(j["assignment"]);
}


void from_json(const json& j, OHomework& o_homework) {
    o_homework.user = j["user"];
    j["name"].get_to(o_homework.name);
}


void from_json(const json& j, Overrides& overrides) {
    j["labs"].get_to(overrides.o_labs);
    j["homeworks"].get_to(overrides.o_homeworks);
}


void to_json(json& j, const OLab& o_lab) {
    j = {
        {"user", o_lab.user},
        {"session", o_lab.session},
    };
    if (o_lab.asgmt)
        j["assignment"] = *o_lab.asgmt;
}


void to_json(json& j, const OHomework& o_homework) {
    j = {
        {"user", o_homework.user},
        {"name", o_homework.name},
    };
}


void to_json(json& j, const Overrides& overrides) {
    j = {
        {"labs", overrides.o_labs},
        {"homeworks", overrides.o_homeworks}
    };
}
