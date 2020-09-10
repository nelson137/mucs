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
    enum {
        File = 1 << 0,
        Dir  = 1 << 1,
        Link = 1 << 2,
    };

    Path() = default;
    Path(const string& s);

    operator string() const;

    friend Path  operator+ (const Path& p, const string& ext);
    friend Path& operator+=(      Path& p, const string& ext);

    friend Path  operator/ (const Path& p, const string& rel_path);
    friend Path  operator/ (const Path& a, const   Path&        b);
    friend Path& operator/=(      Path& p, const string& rel_path);
    friend Path& operator/=(      Path& a, const   Path&        b);

    virtual string str() const;
    virtual string basename() const;

    virtual Path& append(const string& ext);
    virtual Path& join(const string& rel_path);

    virtual bool exists() const;
    virtual int type() const;
    virtual bool is_exe() const;
    virtual bool is_file() const;
    virtual bool is_dir() const;

    virtual void chmod(mode_t mode) const;

    virtual bool link_to(const Path& target) const;

    virtual void rm() const;
    virtual void rm_recurse() const;

    /**
     * Files only
     */

    virtual string read() const;
    virtual void for_each_line(function<void(const string&)> unary_op) const;

    virtual void copy_into(const Path& dir, mode_t mode = 0644) const;

    /**
     * Directories only
     */

    virtual vector<string> ls() const;
    virtual vector<string> ls_base() const;

    virtual void chmod_recurse(mode_t mode, int filter = File|Dir) const;

    virtual void mkdir(mode_t mode = 0775) const;
    virtual void mkdir_recurse(mode_t mode = 0775) const;

};


struct MockPath : public Mock<Path> {

    ostringstream data;

    MockPath();
    MockPath(const string& p);

    Path& get();

    MockPath& operator<<(const string& data);

};


#endif
