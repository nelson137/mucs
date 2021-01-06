#include "main.hpp"


#ifndef _MUCS_TEST

int main(int argc, char **argv) {
    NOW = current_datetime();
    CONFIGS_AVAILABLE = Path(CONFIG_DIR).ls_base();

    Mucs mucs;
    unique_ptr<CLI::App> app = mucs.get_cli();

    try {
        /**
         * Parse args. The correct method will be called according to the given
         * subcommand.
         */
        CLI11_PARSE(*app, argc, argv);
    } catch (const exception& e) {
        /**
         * Replace template string "{filename}" with the filename of the
         * targetted config.
         */
        string msg = e.what();
        size_t pos;
        while ((pos = msg.find("{filename}")) != string::npos) {
            msg.replace(pos, 10, mucs.config.filename);
        }
        cerr << msg << endl;
        return 1;
    }

    return 0;
}

#endif
