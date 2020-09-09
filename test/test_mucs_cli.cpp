#include "test_mucs_cli.hpp"


#define MUCS_CLI_PARSE(mucs, args) mucs.get_cli()->parse(args, true)

#define TEST_CLI_SUBCMD_CALLED(args, subcmd)              \
    do {                                                  \
        Mucs mucs; Mock<Mucs> spy(mucs);                  \
        Fake(Method(spy, invoke));                        \
        REQUIRE_NOTHROW(                                  \
            MUCS_CLI_PARSE(spy.get(), args)               \
        );                                                \
        REQUIRE_NOTHROW(                                  \
            Verify(                                       \
                Method(spy, invoke).Using(&Mucs::subcmd)  \
            ).Once()                                      \
        );                                                \
    } while (0)

#define TEST_CLI_SUBCMD_ERROR(args, error)  \
    do {                                    \
        Mucs mucs;                          \
        REQUIRE_THROWS_WITH(                \
            MUCS_CLI_PARSE(mucs, args),     \
            error                           \
        );                                  \
    } while (0)

#define TEST_INVOKE(method_name, method_ptr)               \
    do {                                                   \
        Mucs mucs;                                         \
        mucs.course = "test1";                             \
        Mock<Mucs> mock(mucs);                             \
        When(Method(mock, get_user)).Return(rand_user());  \
        Fake(Method(mock, submit));                        \
        Fake(Method(mock, admin_dump));                    \
        Fake(Method(mock, admin_update_password));         \
        mock.get().invoke(method_ptr);                     \
        Verify(Method(mock, method_name));                 \
    } while (0)


TEST_CASE("invoke calls the correct subcommand member functions",
          "[cli][invoke]") {
    TEST_INVOKE(submit, &Mucs::submit);
    TEST_INVOKE(admin_dump, &Mucs::admin_dump);
    TEST_INVOKE(admin_update_password, &Mucs::admin_update_password);
}


TEST_CASE("cli", "[cli]") {
    SECTION("no arguments") {
        TEST_CLI_SUBCMD_ERROR(
            "mucs",
            "A subcommand is required"
        );
    }
}


TEST_CASE("cli : admin", "[cli][cli-admin]") {
    CONFIGS_AVAILABLE = {"1050"};

    SECTION("no arguments") {
        TEST_CLI_SUBCMD_ERROR(
            "mucs admin 1050",
            "A subcommand is required"
        );
    }

    SECTION("dump") {
        TEST_CLI_SUBCMD_CALLED(
            "mucs admin 1050 dump",
            admin_dump
        );
    }
}


TEST_CASE("cli : submit", "[cli][cli-submit]") {
    SECTION("no arguments") {
        CONFIGS_AVAILABLE = {};
        TEST_CLI_SUBCMD_ERROR(
            "mucs submit",
            "course is required"
        );
    }

    SECTION("arguments : course") {
        CONFIGS_AVAILABLE = {"1050"};
        TEST_CLI_SUBCMD_ERROR(
            "mucs submit 1050",
            "assignment is required"
        );
    }

    SECTION("arguments : course assignment_type") {
        CONFIGS_AVAILABLE = {"1050"};
        TEST_CLI_SUBCMD_ERROR(
            "mucs submit 1050 hw",
            "sources: At least 1 required"
        );
    }

    SECTION("arguments : course assignment_type source") {
        CONFIGS_AVAILABLE = {"1050"};
        TEST_CLI_SUBCMD_CALLED(
            "mucs submit 1050 hw hw1.c",
            submit
        );
    }
}


#undef MUCS_CLI_PARSE
#undef TEST_CLI_SUBCMD_CALLED
#undef TEST_CLI_SUBCMD_ERROR
#undef TEST_INVOKE
