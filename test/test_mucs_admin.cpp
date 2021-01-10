#include "test_mucs_admin.hpp"


TEST_CASE("admin_authenticate", "[mucs][mucs-admin][admin-authenticate]") {
    string password = rand_string();
    Mucs mucs;
    mucs.config.admin_hash = picosha2::hash256_hex_string(password);
    Mock<Mucs> spy(mucs);

    SECTION("throws when the entered password is incorrect") {
        When(Method(spy, prompt_password)).Return(rand_string());
        REQUIRE_THROWS_WITH(
            spy.get().admin_authenticate(),
            "Password incorrect"
        );
    }

    SECTION("succeeds when the entered password is correct") {
        When(Method(spy, prompt_password)).Return(password);
        REQUIRE_NOTHROW(spy.get().admin_authenticate());
    }

    Verify(Method(spy, prompt_password)).Once();
    VerifyNoOtherInvocations(spy);
}


TEST_CASE("subcommand admin dump", "[mucs][mucs-admin]") {
    Mucs mucs;
    mucs.dump_flags = 0;

    Mock<Mucs> spy(mucs);
    string err_msg = rand_string();
    When(Method(spy, admin_authenticate)).AlwaysThrow(err_msg);

    REQUIRE_THROWS_WITH(spy.get().admin_dump(), err_msg);
    Verify(Method(spy, admin_authenticate)).Once();
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

        Verify(
            Method(spy, admin_authenticate)
            + Method(spy, prompt_password)
            + Method(spy, prompt_password)
        ).Once();
    }

    SECTION("new passwords match") {
        string p = rand_string();
        When(Method(spy, prompt_password)).Return(2_Times(p));

        REQUIRE_NOTHROW(spy.get().admin_update_password());

        Verify(
            Method(spy, admin_authenticate)
            + Method(spy, prompt_password)
            + Method(spy, prompt_password)
            + Method(spy, update_config_admin_hash)
        ).Once();
    }

    VerifyNoOtherInvocations(spy);
}
