#include "test_mucs_cli.hpp"


#define MUCS_CLI_PARSE(mucs, args) \
    mucs.get_cli()->parse(args, true)

#define TEST_CLI_SUBCMD_CALLED(subcmd, args)    \
    do {                                        \
        Mucs mucs; Mock<Mucs> spy(mucs);        \
        Fake(Method(spy, subcmd));              \
        REQUIRE_NOTHROW(                        \
            MUCS_CLI_PARSE(spy.get(), args)     \
        );                                      \
    } while (0)

#define TEST_CLI_SUBCMD_ERROR(args, error)  \
    do {                                    \
        Mucs mucs;                          \
        REQUIRE_THROWS_WITH(                \
            MUCS_CLI_PARSE(mucs, args),     \
            error                           \
        );                                  \
    } while (0)


TEST_CASE("cli", "[cli]") {
    SECTION("no arguments") {
        TEST_CLI_SUBCMD_ERROR(
            "mucs",
            "A subcommand is required"
        );
    }
}


TEST_CASE("cli : admin", "[cli][cli-admin]") {
    SECTION("no arguments") {
        TEST_CLI_SUBCMD_ERROR(
            "mucs admin 1050",
            "A subcommand is required"
        );
    }

    SECTION("dump") {
        TEST_CLI_SUBCMD_CALLED(
            admin_dump,
            "mucs admin 1050 dump"
        );
    }
}


TEST_CASE("cli : submit", "[cli][cli-submit]") {
    SECTION("no arguments") {
        TEST_CLI_SUBCMD_ERROR(
            "mucs submit",
            "course is required"
        );
    }

    SECTION("arguments : course") {
        TEST_CLI_SUBCMD_ERROR(
            "mucs submit 1050",
            "assignment_type is required"
        );
    }

    SECTION("arguments : course assignment_type") {
        TEST_CLI_SUBCMD_ERROR(
            "mucs submit 1050 hw",
            "sources: At least 1 required"
        );
    }

    SECTION("arguments : course assignment_type source") {
        TEST_CLI_SUBCMD_CALLED(
            submit,
            "mucs submit 1050 hw hw1.c"
        );
    }
}


#undef MUCS_CLI_PARSE
#undef TEST_CLI_SUBCMD_CALLED
#undef TEST_CLI_SUBCMD_ERROR
