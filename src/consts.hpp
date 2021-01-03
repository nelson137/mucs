#ifndef CONSTS_HPP
#define CONSTS_HPP


#include <string>
#include <vector>

using namespace std;


#ifndef MUCS_ROOT
#define MUCS_ROOT "/group/cs1050"
#endif

#ifndef COMPILE_SCRIPT
#define COMPILE_SCRIPT "/usr/local/bin/compile"
#endif

#define SUBMIT_DIR  MUCS_ROOT "/submissions"
#define CONFIG_DIR  MUCS_ROOT "/config.d"
#define ROSTER_DIR  MUCS_ROOT "/roster"
#define SCHEMA_PATH CONFIG_DIR "/schema.json"

#define LAB_ASGMT_FMT    "%Y %b %d"
#define HW_FMT           "%Y-%m-%d %H:%M:%S"
#define DATETIME_EXT_FMT "_%Y-%m-%d_%H:%M:%S"
#define TIME_FMT         "%H:%M:%S"

constexpr double TERM_WIDTH_COEFF = .85;


extern vector<string> CONFIGS_AVAILABLE;


#endif
