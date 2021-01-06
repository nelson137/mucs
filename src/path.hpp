#ifndef PATH_HPP
#define PATH_HPP


#include <fstream>
#include <functional>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <dirent.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fakeit.hpp"

#include "util.hpp"

using namespace fakeit;
using namespace std;

using json = nlohmann::json;


class Path {

protected:
    string m_path;

    unique_ptr<struct stat> stat() const;

public:
    enum Type {
        File = 1 << 0,
        Dir  = 1 << 1,
        Link = 1 << 2,
    };

    Path() = default;
    Path(const string& s);

    operator string() const;

    /**
     * Append ext to the end of p with no delimeter.
     */
    friend Path  operator+ (const Path& p, const string& ext);
    friend Path& operator+=(      Path& p, const string& ext);

    /**
     * Join two paths with a slash.
     */
    friend Path  operator/ (const Path& p, const string& rel_path);
    friend Path  operator/ (const Path& a, const   Path&        b);
    friend Path& operator/=(      Path& p, const string& rel_path);
    friend Path& operator/=(      Path& a, const   Path&        b);

    /**
     * Return the path as a string.
     */
    virtual string str() const;

    /**
     * Return the basename, the last component, of the path.
     *
     * e.g. the basename of `/home/nwewnh` is `nwewnh`.
     */
    virtual string basename() const;

    /**
     * Append ext to the end of this path with no delimeter.
     */
    virtual Path& append(const string& ext);

    /**
     * Join rel_path with this path, i.e. append using `/` as a delimiter.
     */
    virtual Path& join(const string& rel_path);

    /**
     * Return whether this path exists.
     */
    virtual bool exists() const;

    /**
     * Return a Path::Type describing what this path is.
     */
    virtual int type() const;

    /**
     * Return whether this path is executable.
     */
    virtual bool is_exe() const;

    /**
     * Return whether this path is a regular file.
     */
    virtual bool is_file() const;

    /**
     * Return whether this path is a directory.
     */
    virtual bool is_dir() const;

    /**
     * Set the mode of this path to mode.
     */
    virtual void chmod(mode_t mode) const;

    /**
     * Create a symbolic link at this path pointing to target.
     */
    virtual bool link_to(const Path& target) const;

    /**
     * Delete this path.
     * Works for files and directories (only if they are empty).
     */
    virtual void rm() const;

    /**
     * Delete this path, recursively deletes any child directories and files.
     * Works for files and directories.
     */
    virtual void rm_recurse() const;

    /*************************************************
     * File methods
     ************************************************/

    /**
     * Return the entire contents of this path as a string.
     */
    virtual string read() const;

    /**
     * Read this path one line at a time, calling unary_op with each line.
     */
    virtual void for_each_line(function<void(const string&)> unary_op) const;

    /**
     * Copy this path into dir with mode mode.
     */
    virtual void copy_into(const Path& dir, mode_t mode = 0644) const;

    /*************************************************
     * Directory methods
     ************************************************/

    /**
     * Return a list of the absolute paths of the direct children of this path.
     */
    virtual vector<string> ls() const;

    /**
     * Return a list of the basenames of the direct children of this path.
     */
    virtual vector<string> ls_base() const;

    /**
     * Recursively set the mode of this node and all child nodes to mode if the
     * node is included by filter.
     */
    virtual void chmod_recurse(mode_t mode, int filter = File|Dir) const;

    /**
     * Create a directory at this path.
     */
    virtual void mkdir(mode_t mode = 0775) const;

    /**
     * Create a directory at this path, creating any parent directories as
     * needed.
     */
    virtual void mkdir_recurse(mode_t mode = 0775) const;

};


struct MockPath : public Mock<Path> {

    // The string to return when Path::read is called
    ostringstream file_contents;

    MockPath();
    MockPath(const string& p);

    Path& get();

    /**
     * Append data to file_contents.
     */
    MockPath& operator<<(const string& data);

};


#endif
