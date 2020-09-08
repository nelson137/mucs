#include "test_mucs_admin.hpp"


TEST_CASE("subcommand admin dump", "[mucs][mucs-admin]") {
    Mucs mucs;
    Mock<Mucs> spy(mucs);
    Fake(
        Method(spy, admin_authenticate),
        Method(spy, dump_homeworks),
        Method(spy, dump_lab_assignments),
        Method(spy, dump_lab_sessions),
        Method(spy, dump_roster)
    );

    SECTION("everything by default") {
        mucs.dump_flags = 0;
        REQUIRE_NOTHROW(spy.get().admin_dump());
        Verify(Method(spy, dump_homeworks));
        Verify(Method(spy, dump_lab_assignments));
        Verify(Method(spy, dump_lab_sessions));
        Verify(Method(spy, dump_roster));
    }

    SECTION("homeworks") {
        mucs.dump_flags = Mucs::DumpHomeworks;
        REQUIRE_NOTHROW(spy.get().admin_dump());
        Verify(Method(spy, dump_homeworks));
    }

    SECTION("lab assignments") {
        mucs.dump_flags = Mucs::DumpLabAssignments;
        REQUIRE_NOTHROW(spy.get().admin_dump());
        Verify(Method(spy, dump_lab_assignments));
    }

    SECTION("lab sessions") {
        mucs.dump_flags = Mucs::DumpLabSessions;
        REQUIRE_NOTHROW(spy.get().admin_dump());
        Verify(Method(spy, dump_lab_sessions));
    }

    SECTION("roster") {
        mucs.dump_flags = Mucs::DumpRoster;
        REQUIRE_NOTHROW(spy.get().admin_dump());
        Verify(Method(spy, dump_roster));
    }

    Verify(Method(spy, admin_authenticate));
    VerifyNoOtherInvocations(spy);
}


TEST_CASE("subcommand admin update-password", "[mucs][mucs-admin]") {
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
            spy.get().admin_update_password(),
            "Passwords do not match"
        );
    }

    SECTION("new passwords match") {
        string p = rand_string();
        When(Method(spy, prompt_password)).Return(p).Return(p);

        REQUIRE_NOTHROW(spy.get().admin_update_password());

        Verify(Method(spy, update_config_admin_hash));
    }

    Verify(Method(spy, admin_authenticate));
}
