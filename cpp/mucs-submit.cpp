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


#include <iostream>
#include <string>
#include <vector>

#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

using namespace std;


#define USAGE              "Usage: mucs-submit SUBMIT_DIR ASSIGNMENT " \
                               "USERNAME SOURCE_DIR"
#define ERR_INVALID_PARAM  "Parameters may not contain '..' or '/': "
#define ERR_MKDIR          "Unable to make directory: "
#define ERR_NOT_A_DIR      "Path exists but is not directory: "
#define ERR_NO_SUBMIT_DIR  "Submission directory not found: "


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


void make_submit_component(string path) {
    struct stat s;
    // If path already exists
    if(stat(path.c_str(), &s) == 0) {
        // Error if not a directory
        if (S_ISDIR(s.st_mode) == 0)
            die(ERR_NOT_A_DIR + path);
    // If path doesn't exist
    } else {
        // Attempt to make directory
        if (mkdir(path.c_str(), 0770))
            die(ERR_MKDIR + path);
    }
}


bool path_is_valid(string& p) {
    if (p.find("..") != string::npos)
        return false;
    if (p.find("/") != string::npos)
        return false;
    return true;
}


void rmdir(string path) {
    char *argv[] = {
        (char*)"rm", (char*)"-rf",
        const_cast<char*>(path.c_str()),
        (char*)NULL
    };
    int ret;

    pid_t pid = fork();
    if (pid < 0) {
        die("Could not fork");
    } else if (pid == 0) {
        execv("/bin/rm", argv);
        _exit(1);
    } else {
        wait(&ret);
    }
}


void verify_paths(vector<string> paths) {
    for (auto p : paths) {
        if (path_is_valid(p) == false)
            die(ERR_INVALID_PARAM + p);
    }
}


void verify_submit_dir(string path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode) == 0)
            die(ERR_NOT_A_DIR + path);
    } else {
        die(ERR_NO_SUBMIT_DIR + path);
    }
}


class ExecArgs {

private:
    int _argc = -1;
    char **_argv = nullptr;

public:
    ExecArgs(const vector<string>&  v) { init(v); }
    ExecArgs(const vector<string>&& v) { init(v); }

    void init(const vector<string>& vec) {
        int size = this->_argc = vec.size() + 1;
        char **argv = this->_argv = static_cast<char**>(
            malloc(size * sizeof(char*))
        );

        int len;
        for (int i=0; i<size-1; i++) {
            len = strlen(vec[i].c_str()) + 1;
            argv[i] = static_cast<char*>(
                malloc(len * sizeof(char))
            );
            strncpy(argv[i], vec[i].c_str(), len);
        }

        argv[size-1] = nullptr;
    }

    ~ExecArgs() {
        for (int i=0; i<this->_argc; i++)
            free(this->_argv[i]);
        free(this->_argv);
    }

    char **get() const {
        return this->_argv;
    }

};


int main(int argc, char **argv) {
    if (argc != 5)
        die(USAGE);

    string submit_base_d = "/group/cs1050/submissions";
    string course        = argv[1];
    string assignment    = argv[2];
    string username      = argv[3];
    string source_d      = argv[4];

    vector<string> components = { course, assignment, username };

    verify_submit_dir(submit_base_d);
    verify_paths(components);

    string submit_d = submit_base_d;
    // submit_base_d
    make_submit_component(submit_d);
    // submit_base_d/course
    submit_d += "/" + course;
    make_submit_component(submit_d);
    // submit_base_d/course/assignment
    submit_d += "/" + assignment;
    make_submit_component(submit_d);
    // submit_base_d/course/assignment/username
    submit_d += "/" + username;
    struct stat s;
    if (stat(submit_d.c_str(), &s) == 0)
        rmdir(submit_d);
    make_submit_component(submit_d);

    chdir(source_d.c_str());

    // Copy files
    vector<string> cmd_argv = {
        "find", ".", "-type", "f", "-exec",
        "/usr/bin/install", "-C", "-g", "cs1050-ta",
        "-m", "660", "-t", submit_d, "{}",
        ";"
    };
    ExecArgs ea(cmd_argv);
    return execv("/usr/bin/find", ea.get());
}
