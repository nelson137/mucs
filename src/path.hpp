#ifndef PATH_HPP
#define PATH_HPP


#include <fstream>
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


class IPath {

protected:
    string m_path;

    virtual unique_ptr<struct stat> stat() const = 0;

public:
    virtual string str() const = 0;

    virtual string basename() const = 0;

    virtual IPath& append(const string& ext) = 0;
    virtual IPath& join(const string& rel_path) = 0;

    virtual bool exists() const = 0;
    virtual bool is_dir() const = 0;
    virtual bool is_file() const = 0;
    virtual bool link_to(const IPath& target) const = 0;
    virtual vector<string> ls() const = 0;
    virtual int mkdir() const = 0;
    virtual int mkdir_recurse() const = 0;
    virtual string read() const = 0;
    virtual int rm() const = 0;
    virtual int rm_recurse() const = 0;

};


class Path : public IPath {

protected:
    unique_ptr<struct stat> stat() const;

public:
    Path() = default;
    Path(const string& s);

    operator string() const;

    friend Path  operator+ (const Path& p, const string& ext);
    friend Path& operator+=(      Path& p, const string& ext);

    friend Path  operator/ (const Path& p, const string& rel_path);
    friend Path  operator/ (const Path& a, const   Path&        b);
    friend Path& operator/=(      Path& p, const string& rel_path);
    friend Path& operator/=(      Path& a, const   Path&        b);

    string str() const;

    string basename() const;

    IPath& append(const string& ext);
    IPath& join(const string& rel_path);

    bool exists() const;
    bool is_dir() const;
    bool is_file() const;
    bool link_to(const IPath& target) const;
    vector<string> ls() const;
    int mkdir() const;
    int mkdir_recurse() const;
    string read() const;
    int rm() const;
    int rm_recurse() const;
    void copy_into(const Path& dir, mode_t mode = 0644) const;

};


struct MockPath : public Mock<IPath> {

    MockPath();
    MockPath(const string& p);

    MockPath& operator<<(const string& s);
    MockPath& operator<<(const json& j);

};


#endif
