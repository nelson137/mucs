/**
 * Usage: mucs-submit SUBMIT_DIR COURSE ASSIGNMENT USERNAME SOURCE_DIR
 *
 * Description
 *        mucs-submit is the part of mucs that does the heavy lifting when
 *        subitting.  It is responsible for creating any necessary components
 *        of the submission directory tree as well as copying the source files
 *        into the tree.  For this program to work correctly its executable
 *        must have the setuid bit enabled and owned by a user in the cs1050-ta
 *        group.
 *
 * Positional Arguments
 *        SUBMIT_DIR    Base directory for mucs submissions
 *        COURSE        Course id
 *        ASSIGNMENT    Name of current assignment
 *        USERNAME      Name of user for which to submit
 *        SOURCE_DIR    Path to directory containing source files to submit
 *
 * Examples
 *        mucs-submit /group/cs1050/submissions 1050 hw1 jer676 /tmp/mucs.tmp
 */


#include "main.hpp"


int main(int argc, char **argv) {
    if (argc != 5)
        die(USAGE);

    string submit_root = "/group/cs1050/submissions";
    string course      = argv[1];
    string assignment  = argv[2];
    string username    = argv[3];
    string source_d    = argv[4];

    verify_dir_exists(submit_root);
    verify_paths(course, assignment, username);
    verify_dir_exists(source_d);

    // submit_root/course/assignment/username
    string submit_d = join_paths(submit_root, course, assignment, username);

    struct stat s;
    if (stat(submit_d.c_str(), &s) == 0)
        rmdir(submit_d);

    make_path(submit_root, course, assignment, username);

    // Find doesn't work unless you cd into the starting point for some reason
    chdir(source_d.c_str());

    // Copy files
    ExecArgs ea(
        "find", ".", "-type", "f", "-exec",
        "/usr/bin/install", "-C", "-g", "cs1050-ta",
        "-m", "660", "-t", submit_d, "{}",
        ";"
    );
    return execv("/usr/bin/find", ea.prepare());
}
