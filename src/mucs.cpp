#include "mucs.hpp"


function<void()> Mucs::invoke(void (Mucs::*subcmd)(const Config&)) {
    return [subcmd,this] () {
        Config config = Config::parse_file(Path(CONFIG_DIR) / this->course);
        mem_fn(subcmd)(*this, config);
    };
}
