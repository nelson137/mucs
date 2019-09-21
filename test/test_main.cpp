#include "test_main.hpp"


int main(int argc, char *argv[]) {
    srand(time(NULL));

    return Catch::Session().run(argc, argv);
}
