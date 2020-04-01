#include "mucs.hpp"


void Mucs::invoke(void (Mucs::*subcmd)()) {
    this->user = get_user();
    this->config = Config::parse_file(Path(CONFIG_DIR) / this->course);
    (this->*subcmd)();
}


function<void()> Mucs::get_invoke(void (Mucs::*subcmd)()) {
    return bind(mem_fn(&Mucs::invoke), ref(*this), subcmd);
}


bool Mucs::prompt_yesno(const string& prompt) {
    if (prompt.size())
        cout << prompt;
    string response;
    cin >> response;
    cout << endl;
    stl_transform(response, ::tolower);
    return string_strip(response) == "y";
}
