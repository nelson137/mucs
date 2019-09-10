/**
 * Usage: mucs-submit SUBMIT_DIR COURSE ASSIGNMENT USERNAME SOURCES...
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
 *        SOURCES       List of source files to submit
 *
 * Behavior
 *        When mucs-submit is run it will first verify that
 *
 * Examples
 *        mucs-submit /group/cs1050/submissions 1050 lab3 jer676 m_lab3.c
 */


#include <iostream>
#include <string>
#include <vector>

#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

using namespace std;


#define USAGE              "Usage: mucs-submit SUBMIT_DIR ASSIGNMENT " \
                               "USERNAME SOURCES..."
#define ERR_INVALID_PARAM  "Parameters may not contain '..' or '/': "
#define ERR_FILE_NOT_DIR   "Path exists but is not directory: "
#define ERR_MKDIR          "Unable to make directory: "
#define ERR_NOT_A_FILE     "Unable to submit non-file: "
#define ERR_CANT_READ      "Unable to read file: "
#define ERR_FILE_NOT_FOUND "File not found: "


void die(string msg) {
    cerr << msg << endl;
    exit(1);
}


string make_tree(string path, vector<string> descendants) {
    struct stat s;
    for (auto d : descendants) {
        path += "/" + d;
        // If path already exists
        if(stat(path.c_str(), &s) == 0) {
            // Error if not a directory
            if (S_ISDIR(s.st_mode) == 0)
                die(ERR_FILE_NOT_DIR + path);
        // If path doesn't exist
        } else {
            // Attempt to make directory
            if (mkdir(path.c_str(), 0770))
                die(ERR_MKDIR + path);
        }
    }
    return path;
}


bool path_is_valid(string& p) {
    if (p.find("..") != string::npos)
        return false;
    if (p.find("/") != string::npos)
        return false;
    return true;
}


void verify_files(vector<string> files) {
    struct stat s;
    for (auto f : files) {
        if (stat(f.c_str(), &s) == 0) {
            if (S_ISREG(s.st_mode) == 0)
                die(ERR_NOT_A_FILE + f);
            if ((s.st_mode & S_IROTH) == 0)
                die(ERR_CANT_READ + f);
        } else {
            die(ERR_FILE_NOT_FOUND + f);
        }
    }
}


void verify_paths(vector<string> paths) {
    for (auto p : paths) {
        if (path_is_valid(p) == false)
            die(ERR_INVALID_PARAM + p);
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
    if (argc < 5)
        die(USAGE);

    string submit_base_d = "/group/cs1050/submissions";
    string course = argv[1];
    string assignment = argv[2];
    string username = argv[3];

    vector<string> files(argv+4, argv+argc);
    vector<string> components = { course, assignment, username };

    verify_files(files);
    verify_paths(components);

    string submit_d = make_tree(submit_base_d, components);

    vector<string> cmd_argv;
    cmd_argv.reserve(8 + files.size());
    cmd_argv = {
        "install", "-C",
        "-g", "cs1050-ta",
        "-m", "660",
        "-t", submit_d
    };
    cmd_argv.insert(cmd_argv.end(), files.begin(), files.end());

    ExecArgs ea(cmd_argv);
    return execv("/usr/bin/install", ea.get());
}
