#include "mucs.hpp"


void Mucs::init() {
    this->config = Config::parse_file(Path(CONFIG_DIR) / this->course);
}
