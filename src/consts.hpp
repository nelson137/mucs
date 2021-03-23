#ifndef CONSTS_HPP
#define CONSTS_HPP


#include <string>
#include <vector>

using namespace std;


#define _STR(s) #s
#define STR(s) _STR(s)

#define MUCS_VERSION_MAJOR 2
#define MUCS_VERSION_MINOR 2

#define MUCS_ROOT      STR(_MUCS_ROOT)
#define COMPILE_SCRIPT STR(_COMPILE_SCRIPT)
#define SUBMIT_DIR     MUCS_ROOT "/submissions"
#define CONFIG_DIR     MUCS_ROOT "/config.d"
#define ROSTER_DIR     MUCS_ROOT "/roster.d"
#define SCHEMA_PATH    MUCS_ROOT "/schema.json"

#define HW_FMT   "%Y-%m-%d %H:%M:%S"
#define TIME_FMT "%H:%M:%S"

constexpr double TERM_WIDTH_COEFF = .85;


#endif
