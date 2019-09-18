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
#define ERR_DIR_NOT_FOUND  "Directory not found: "
#define ERR_FORK           "Could not fork"
#define ERR_INVALID_PARAM  "Parameters course, assignment, and username may " \
                               "not contain '..' or '/': "
#define ERR_MKDIR          "Unable to make directory: "
#define ERR_NOT_A_DIR      "Path exists but is not directory: "


class ExecArgs {

private:
    int _argc = 0;
    char **_argv = nullptr;
    vector<string> _vec;

public:
    ExecArgs(const vector<string>&  v) : _vec(v) {}
    ExecArgs(const vector<string>&& v) : _vec(v) {}
    template<typename... T>
    ExecArgs(string arg0, T... args) : _vec({ arg0, args... }) {}

    ~ExecArgs() {
        if (this->_argv) {
            for (int i=0; i<this->size(); i++)
                free(this->_argv[i]);
            free(this->_argv);
        }
    }

    int size() const {
        return this->_vec.size();
    }

    char **prepare() {
        if (this->_argv)
            return this->_argv;

        int argc = this->_argc = this->size() + 1;
        char **argv = this->_argv = static_cast<char**>(
            malloc(argc * sizeof(char*))
        );

        int size;
        for (int i=0; i<argc-1; i++) {
            size = this->_vec[i].size() + 1;
            argv[i] = static_cast<char*>(
                malloc(size * sizeof(char))
            );
            strncpy(argv[i], this->_vec[i].c_str(), size);
        }

        argv[argc-1] = nullptr;

        return argv;
    }

};


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


template<typename... T>
string join_paths(string a, string b, T... rest) {
    vector<string> components = { b, rest... };
    string p = a;
    for (auto c : components)
        p += "/" + c;
    return p;
}


template<typename... T>
void make_path(string base, T... comps) {
    struct stat s;
    string path = base;
    vector<string> components = { comps... };

    for (auto c : components) {
        path = join_paths(path, c);
        if(stat(path.c_str(), &s) == 0) {   // If path exists
            if (S_ISDIR(s.st_mode) == 0)    //   If not a directory
                die(ERR_NOT_A_DIR + path);  //     Error
        } else {                            // If path doesn't exist
            if (mkdir(path.c_str(), 0770))  //   Attempt to make directory
                die(ERR_MKDIR + path);      //     Error if failed
        }
    }
}


bool path_is_valid(string& path) {
    if (path.find("..") != string::npos)
        return false;
    if (path.find("/") != string::npos)
        return false;
    return true;
}


void rmdir(string& path) {
    ExecArgs ea("rm", "-rf", path);
    int ret;

    pid_t pid = fork();
    if (pid < 0) {
        // Error forking
        die(ERR_FORK);
    } else if (pid == 0) {
        // Child
        execv("/bin/rm", ea.prepare());
        _exit(1);
    } else {
        // Parent
        wait(&ret);
    }
}


template<typename... T>
void verify_paths(string comp1, T... ts) {
    vector<string> paths = { comp1, ts... };
    for (auto p : paths) {
        if (not path_is_valid(p))
            die(ERR_INVALID_PARAM + p);
    }
}


void verify_dir_exists(string dirpath) {
    struct stat s;
    if (stat(dirpath.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode) == 0)
            die(ERR_NOT_A_DIR + dirpath);
    } else {
        die(ERR_NOT_A_DIR + dirpath);
    }
}


int main(int argc, char **argv) {
    if (argc != 6)
        die(USAGE);

    string submit_root = "/group/cs1050/submissions";
    string course      = argv[1];
    string lab         = argv[2];
    string assignment  = argv[3];
    string username    = argv[4];
    string source_d    = argv[5];

    verify_dir_exists(submit_root);
    verify_paths(course, lab, assignment, username);
    verify_dir_exists(source_d);

    // submit_root/course/assignment/username
    string submit_d = join_paths(
        submit_root, course, lab, assignment, username);

    struct stat s;
    if (stat(submit_d.c_str(), &s) == 0)
        rmdir(submit_d);

    make_path(submit_root, course, lab, assignment, username);

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
