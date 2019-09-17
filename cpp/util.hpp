#include <string>
#include <vector>

#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "errors.hpp"
#include "execargs.hpp"

using namespace std;


void die(string);

template<typename... T>
string join_paths(string, string, T...);

template<typename... T>
void make_path(string, T...);

bool path_is_valid(string&);

void rmdir(string&);

void verify_dir_exists(string&);

template<typename... T>
void verify_paths(string, T...);
