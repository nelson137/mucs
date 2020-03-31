#ifndef CONSTS_HPP
#define CONSTS_HPP


#include <string>

using namespace std;


#ifdef _MUCS_ROOT_LOCAL
#  define MUCS_ROOT "test_root"
#  define COMPILE_SCRIPT "scripts/compile"
#else
#  define MUCS_ROOT "/group/cs1050"
#  define COMPILE_SCRIPT "/usr/local/bin/compile"
#endif

#define SUBMIT_DIR (MUCS_ROOT "/submissions")
#define CONFIG_DIR (MUCS_ROOT "/config.d")

#define DATETIME_FMT     "%Y-%m-%d %H:%M:%S"
#define DATETIME_EXT_FMT ".%Y-%m-%d.%H:%M:%S"
#define DATE_FMT         "%Y-%m-%d"
#define TIME_FMT         "%H:%M:%S"

constexpr double TERM_WIDTH_COEFF = .85;

static const string FG_BOLD  = "\033[1m";
static const string FG_GREEN = "\033[32m";
static const string FG_RED   = "\033[91m";
static const string FG_RESET = "\033[0m";


#endif
