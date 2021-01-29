cmake_minimum_required(VERSION 2.8)

project(mucs)

##################################################
### PROJECT FILES
##################################################

# Note: these lists don't include main.cpp to avoid multiple definitions

list(APPEND SRCS
    src/consts.cpp
    src/except.cpp
    src/mucs.cpp
    src/mucs-admin.cpp
    src/mucs-cli.cpp
    src/mucs-submit.cpp
    src/path.cpp
    src/proc.cpp
    src/util.cpp
    src/models/config.cpp
    src/models/homeworks.cpp
    src/models/lab-assignments.cpp
    src/models/lab-sessions.cpp
    src/models/overrides.cpp
    src/models/roster.cpp
    )

list(APPEND TEST_SRCS
    test/random.cpp
    test/test_config.cpp
    test/test_except.cpp
    test/test_homeworks.cpp
    test/test_lab_assignments.cpp
    test/test_lab_sessions.cpp
    test/test_mucs_admin.cpp
    test/test_mucs_submit.cpp
    test/test_overrides.cpp
    test/test_roster.cpp
    test/test_util.cpp
    )

##################################################
### GLOBAL SETUP
##################################################

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_FLAGS "-Wall -Werror -pedantic -Wno-noexcept-type")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# set(CMAKE_VERBOSE_MAKEFILE true)

if(NOT DEFINED _MUCS_ROOT)
    set(_MUCS_ROOT "${PROJECT_SOURCE_DIR}/my_root")
endif()

if(NOT DEFINED _COMPILE_SCRIPT)
    set(_COMPILE_SCRIPT "${PROJECT_SOURCE_DIR}/scripts/compile")
endif()

if(_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_definitions(-DONLY_C_LOCALE=1)

##################################################
### MAIN TARGET
##################################################

add_executable(mucs ${SRCS} src/main.cpp)

set_target_properties(mucs PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(mucs PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    )

target_compile_definitions(mucs PRIVATE
    "_MUCS_ROOT=${_MUCS_ROOT}"
    "_COMPILE_SCRIPT=${_COMPILE_SCRIPT}"
    )

##################################################
### TEST TARGET
##################################################

add_executable(mucs_test ${SRCS} ${TEST_SRCS} test/main.cpp)

set_target_properties(mucs_test PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(mucs_test PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    )

target_compile_definitions(mucs_test PRIVATE
    "_MUCS_ROOT=${PROJECT_SOURCE_DIR}/test_root"
    "_COMPILE_SCRIPT=${_COMPILE_SCRIPT}"
    )

if(_COVERAGE)
    target_compile_options(mucs_test PRIVATE --coverage -O0)
    target_link_libraries(mucs_test PRIVATE gcov)
endif()

##################################################
### INSTALLATION
##################################################

if(_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${_PREFIX}")
endif()

install(TARGETS mucs
    DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ             GROUP_EXECUTE
                WORLD_READ             WORLD_EXECUTE
                SETUID
    )

install(PROGRAMS scripts/compile
    DESTINATION bin
    )

install(PROGRAMS scripts/mucs-gen-roster
    DESTINATION scripts
    )

if(_PREFIX)
    install(CODE "execute_process(COMMAND
        install-fix-perms \"${_PREFIX}\"
        )")
endif()
