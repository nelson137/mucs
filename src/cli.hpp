#ifndef CLI_HPP
#define CLI_HPP


#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "CLI11.hpp"

#include "mucs.hpp"
#include "path.hpp"

using namespace std;


unique_ptr<CLI::App> prepare_cli(Mucs& mucs);


#endif
