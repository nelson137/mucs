#include "test_main.hpp"


int main(int argc, char *argv[]) {
    NOW = system_clock::now();

    return Catch::Session().run(argc, argv);
}
