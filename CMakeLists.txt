cmake_minimum_required(VERSION 3.10)

project(mucs)

##################################################
### PROJECT FILES
##################################################

# Note: these lists don't include main.cpp to avoid multiple definitions

list(APPEND SRCS
    src/except.cpp
    src/mucs.cpp
    src/mucs-admin.cpp
    src/mucs-cli.cpp
    src/mucs-submit.cpp
    src/mucs-version.cpp
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

set(CMAKE_VERBOSE_MAKEFILE true)

set(_DEFAULT_MUCS_ROOT "${PROJECT_SOURCE_DIR}/my_root")
set(_DEFAULT_COMPILE_SCRIPT "${PROJECT_SOURCE_DIR}/scripts/compile")

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    )

add_compile_options(
    -Wall -Werror -pedantic -Wno-noexcept-type
    "$<$<CONFIG:DEBUG>:-g>"
    "$<$<CONFIG:RELEASE>:-O2>"
    "$<$<CONFIG:RELWITHDEBINFO>:-g>"
    )

add_definitions(-DONLY_C_LOCALE=1)

##################################################
### MAIN TARGET
##################################################

add_executable(mucs ${SRCS} src/main.cpp)

set_target_properties(mucs PROPERTIES LINKER_LANGUAGE CXX)

if(DEFINED _MUCS_ROOT)
    target_compile_definitions(mucs PRIVATE "_MUCS_ROOT=${_MUCS_ROOT}")
elseif(DEFINED _PREFIX)
    target_compile_definitions(mucs PRIVATE "_MUCS_ROOT=${_PREFIX}")
else()
    target_compile_definitions(mucs PRIVATE "_MUCS_ROOT=${_DEFAULT_MUCS_ROOT}")
endif()

if(DEFINED _COMPILE_SCRIPT)
    target_compile_definitions(mucs PRIVATE "_COMPILE_SCRIPT=${_COMPILE_SCRIPT}")
elseif(DEFINED _PREFIX)
    target_compile_definitions(mucs PRIVATE "_COMPILE_SCRIPT=${_PREFIX}/bin/compile")
else()
    target_compile_definitions(mucs PRIVATE "_COMPILE_SCRIPT=${_DEFAULT_COMPILE_SCRIPT}")
endif()

##################################################
### TEST TARGET
##################################################

add_executable(mucs_test ${SRCS} ${TEST_SRCS} test/main.cpp)

set_target_properties(mucs_test PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(mucs_test PRIVATE
    "${PROJECT_SOURCE_DIR}/test"
    )

target_compile_definitions(mucs_test PRIVATE
    "_MUCS_ROOT=${PROJECT_SOURCE_DIR}/test_root"
    "_COMPILE_SCRIPT=${_DEFAULT_COMPILE_SCRIPT}"
    )

if(DEFINED _COVERAGE)
    target_compile_options(mucs_test PRIVATE --coverage -O0)
    target_link_libraries(mucs_test PRIVATE gcov)
endif()

##################################################
### INSTALLATION
##################################################

if(_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${_PREFIX}")

    install(
        TARGETS mucs
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ             GROUP_EXECUTE
                    WORLD_READ             WORLD_EXECUTE
                    SETUID
        )

    install(
        FILES schema.json
        DESTINATION .
        )

    install(
        PROGRAMS scripts/compile
        DESTINATION bin
        )

    install(
        PROGRAMS
            scripts/gen-roster
            scripts/get-course-students
            scripts/student-vimrc
        DESTINATION scripts
        )

    install(CODE "execute_process(
        COMMAND \"${PROJECT_SOURCE_DIR}/scripts/post-install\" \"${_PREFIX}\"
        )")
endif()
