#include "test_mucs_cli.hpp"


#define MUCS_CLI_PARSE(mucs, args) \
    mucs.get_cli()->parse(args, true)

#define SPY_CLI_PARSE(spy, args) \
    MUCS_CLI_PARSE(spy.get(), args)

#define TEST_CLI_SUBCMD_ERROR(args, error)  \
    do {                                    \
        Mucs mucs;                          \
        REQUIRE_THROWS_WITH(                \
            MUCS_CLI_PARSE(mucs, args),     \
            error                           \
        );                                  \
    } while (0)


TEST_CASE("cli : no arguments", "[cli]") {
    TEST_CLI_SUBCMD_ERROR("mucs", "A subcommand is required");
}


TEST_CASE("cli : subcommand admin no arguments", "[cli][cli-admin]") {
    TEST_CLI_SUBCMD_ERROR("mucs admin 1050", "A subcommand is required");
}


TEST_CASE("cli : subcommand admin dump", "[cli][cli-admin]") {
    Mucs mucs;
    Mock<Mucs> spy(mucs);
    Fake(
        Method(spy, admin_authenticate),
        Method(spy, dump_currents),
        Method(spy, dump_homeworks),
        Method(spy, dump_labs),
        Method(spy, dump_roster)
    );

    SECTION("with no args everything is dumped") {
        REQUIRE_NOTHROW(SPY_CLI_PARSE(spy, "mucs admin 1050 dump"));
        Verify(
            Method(spy, dump_currents),
            Method(spy, dump_homeworks),
            Method(spy, dump_labs),
            Method(spy, dump_roster)
        );
    }

    SECTION("with no args everything is dumped") {
        REQUIRE_NOTHROW(SPY_CLI_PARSE(spy, "mucs admin 1050 dump -c"));
        Verify(Method(spy, dump_currents));
    }

    SECTION("with no args everything is dumped") {
        REQUIRE_NOTHROW(SPY_CLI_PARSE(spy, "mucs admin 1050 dump -w"));
        Verify(Method(spy, dump_homeworks));
    }

    SECTION("with no args everything is dumped") {
        REQUIRE_NOTHROW(SPY_CLI_PARSE(spy, "mucs admin 1050 dump -l"));
        Verify(Method(spy, dump_labs));
    }

    SECTION("with no args everything is dumped") {
        REQUIRE_NOTHROW(SPY_CLI_PARSE(spy, "mucs admin 1050 dump -r"));
        Verify(Method(spy, dump_roster));
    }

    Verify(Method(spy, admin_authenticate));
    VerifyNoOtherInvocations(spy);
}


TEST_CASE("cli : subcommand admin update-password", "[cli][cli-admin]") {
    Mucs mucs;
    Mock<Mucs> spy(mucs);
    Fake(
        Method(spy, admin_authenticate),
        Method(spy, update_config_admin_hash)
    );

    SECTION("new passwords do not match") {
        string p1 = rand_string();
        string p2 = rand_string();
        When(Method(spy, prompt_password)).Return(p1).Return(p2);

        REQUIRE_THROWS_WITH(
            SPY_CLI_PARSE(spy, "mucs admin 1050 update-password"),
            "Passwords do not match"
        );
    }

    SECTION("new passwords match") {
        string p = rand_string();
        When(Method(spy, prompt_password)).Return(p).Return(p);

        REQUIRE_NOTHROW(SPY_CLI_PARSE(spy, "mucs admin 1050 update-password"));

        Verify(Method(spy, update_config_admin_hash));
    }

    Verify(Method(spy, admin_authenticate));
}


TEST_CASE("cli : subcommand submit", "[cli][cli-submit]") {
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
        string user = rand_user();
        Mucs mucs;
        Mock<Mucs> spy(mucs);
        When(Method(spy, get_user)).Return(user);

        REQUIRE_THROWS_WITH(
            MUCS_CLI_PARSE(mucs, "mucs submit 1050 hw hw1.c"),
            "User not in course: " + user
        );
    }
}


#undef MUCS_CLI_PARSE
#undef SPY_CLI_PARSE
#undef TEST_CLI_SUBCMD_ERROR
