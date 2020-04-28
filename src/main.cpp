#include "main.hpp"


#ifndef MUCS_TEST

int main(int argc, char **argv) {
    NOW = system_clock::now();
    CONFIGS_AVAILABLE = Path(CONFIG_DIR).ls_base();

    Mucs mucs;
    unique_ptr<CLI::App> app = mucs.get_cli();

    try {
        CLI11_PARSE(*app, argc, argv);
    } catch (const mucs_exception& me) {
        cerr << me.what() << endl;
    }

    return 0;
}

#endif
