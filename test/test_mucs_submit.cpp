#include "test_mucs_submit.hpp"


TEST_CASE("submit throws when user not in course", "[mucs][submit]") {
    Mucs mucs;
    mucs.user = rand_user();
    mucs.config = Config();
    REQUIRE_THROWS_WITH(
        mucs.submit(),
        "Student not in course: " + mucs.user
    );
}


TEST_CASE("submit throws", "[mucs][submit]") {
    string user = rand_user();
    string lab_id = rand_lab_sesh_id();
    string lab_name = rand_lab_asgmt_name();

    Mucs mucs;
    mucs.user = user;
    mucs.asgmt_name = lab_name;
    mucs.config.course_id = rand_course();
    mucs.config.roster.safe_insert(user, lab_id);
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh().today().now().get()
    });
    mucs.config.lab_assignments.insert(
        RandLabAsgmt(lab_name).this_week(true).get());

    Mock<Mucs> spy(mucs);
    When(Method(spy, compile_sources)).AlwaysReturn(true);
    Fake(
        Method(spy, submit_summary),
        Method(spy, copy_submission_files)
    );

    SECTION("when a given source path doesn't exist") {
        string src_fn = rand_filename();
        mucs.sources = { Path(src_fn) };
        When(Method(spy, prompt_yesno)).AlwaysReturn(true);
        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            "Source file does not exist: " + src_fn
        );
    }

    SECTION("when a given source path is a directory") {
        string src_fn = "/bin";
        mucs.sources = { Path(src_fn) };
        When(Method(spy, prompt_yesno)).AlwaysReturn(true);
        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            "Cannot submit directories: " + src_fn
        );
    }

    SECTION("when cancelled by user") {
        When(Method(spy, prompt_yesno)).AlwaysReturn(false);
        REQUIRE_THROWS_WITH(
            spy.get().submit(),
            "Submission cancelled"
        );
    }
}


TEST_CASE("submit succeeds", "[mucs][submit]") {
    string user = rand_user();
    string course_id = rand_course();
    string lab_sesh_id = rand_lab_sesh_id();
    /**
     * For some reason, setting the date to 2000-01-01 also sets the time to
     *   06:36:00.
     * Note: `sys_seconds{}` zero-initializes to the unix epoch which is
     *   1970-01-01 at 00:00:00.
     */
    NOW = sys_seconds{} + years(30);
    const string datetime_ext = "_2000-01-01_06:36:00";

    bool lab_sesh_active = GENERATE(true, false);
    bool asgmt_active = GENERATE(true, false);
    bool compiles = GENERATE(true, false);
    bool expected_create_link;
    string asgmt_name;

    Mucs mucs;

    SECTION("homework") {
        expected_create_link = asgmt_active && compiles;
        asgmt_name = rand_hw_name();
        auto duedate = sys_seconds{}
            + years(asgmt_active ? rand_int(31, 60) : rand_int(29));
        mucs.config.homeworks.insert({ asgmt_name, duedate });
    }

    SECTION("lab") {
        expected_create_link = lab_sesh_active && asgmt_active && compiles;
        asgmt_name = rand_lab_asgmt_name();
        mucs.config.lab_assignments.insert(
            RandLabAsgmt(asgmt_name).this_week(asgmt_active).get()
        );
    }

    string archive_dir(expected_create_link ? ".valid" : ".invalid");
    ostringstream expected_asgmt_d, expected_submit_d_rel;
    expected_asgmt_d << SUBMIT_DIR << '/' << course_id << '/' << lab_sesh_id
                     << '/' << asgmt_name;
    expected_submit_d_rel << archive_dir << '/' << user << datetime_ext;

    mucs.user = user;
    mucs.asgmt_name = asgmt_name;
    mucs.course = course_id;
    mucs.config.roster.safe_insert(user, lab_sesh_id);
    mucs.config.lab_sessions.insert({
        lab_sesh_id, RandLabSesh(lab_sesh_id).today(lab_sesh_active).now().get()
    });

    Mock<Mucs> spy(mucs);
    When(Method(spy, prompt_yesno)).AlwaysReturn(true);
    When(Method(spy, compile_sources)).AlwaysReturn(compiles);
    Fake(Method(spy, submit_summary));
    /**
     * Assertions for arguments of copy_submission_files is done like this
     * instead of
     *     Verify(Method(spy, copy_submission_files).Matching(...)).Once()
     * because it causes a segfault.
     */
    When(Method(spy, copy_submission_files)).AlwaysDo(
        [&] (const Path& asgmt_d, const Path& submit_d_rel, bool create_link) {
            REQUIRE(create_link == expected_create_link);
            REQUIRE(asgmt_d.str() == expected_asgmt_d.str());
            REQUIRE(submit_d_rel.str() == expected_submit_d_rel.str());
        }
    );

    INFO("Assignment name: " << asgmt_name);
    INFO("Lab session active: " << boolalpha << lab_sesh_active);
    INFO("Assignment active: " << boolalpha << asgmt_active);
    INFO("Compiles: " << boolalpha << compiles)

    REQUIRE_NOTHROW(spy.get().submit());

    Verify(Method(spy, compile_sources)).Once();
    Verify(Method(spy, submit_summary)).Once();
    Verify(Method(spy, prompt_yesno)).Once();
    Verify(Method(spy, copy_submission_files)).Once();
    VerifyNoOtherInvocations(spy);
}


TEST_CASE("double submit homework too quickly", "[mucs][submit]") {
    NOW = sys_seconds{};
    string user = "user1";
    string lab_id = "X";
    string hw_name = "hw1";
    sys_seconds dd{};
    dd += years((int) get_day().year() - 1970 + 3);

    Mucs mucs;
    mucs.user = user;
    mucs.asgmt_name = hw_name;
    mucs.course = mucs.config.course_id = "1050";
    mucs.config.roster.safe_insert(user, lab_id);
    mucs.config.lab_sessions.insert({
        lab_id, RandLabSesh(lab_id).today().now().get()
    });
    mucs.config.homeworks.insert({ hw_name, dd });

    Mock<Mucs> spy(mucs);
    When(Method(spy, prompt_yesno)).AlwaysReturn(true);
    When(Method(spy, compile_sources)).AlwaysReturn(true);
    Fake(
        Method(spy, submit_summary),
        Method(spy, copy_submission_files)
    );

    REQUIRE_THROWS_WITH(
        spy.get().submit(),
        "Attempted successive submissions too quickly, please try again"
    );

    Verify(Method(spy, compile_sources)).Once();
    Verify(Method(spy, submit_summary)).Once();
    Verify(Method(spy, prompt_yesno)).Once();
    VerifyNoOtherInvocations(spy);
}
