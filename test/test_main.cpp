#include "test_main.hpp"


int main(int argc, char *argv[]) {
    NOW = current_datetime();

    return Catch::Session().run(argc, argv);
}
