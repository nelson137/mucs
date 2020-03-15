#ifndef PATH_HPP
#define PATH_HPP


#include <fstream>
#include <ostream>
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
    int m_stat_ret;
    struct stat m_stat;

    virtual void stat() = 0;

public:
    virtual string str() const = 0;

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

private:
    void stat();

public:
    Path();
    Path(const string& s);

    Path  operator+ (const string& s) const;
    Path& operator+=(const string& s);

    Path  operator/ (const string& rel_path) const;
    Path  operator/ (const   Path&    other) const;
    Path& operator/=(const string& rel_path);
    Path& operator/=(const   Path&    other);

    string str() const;

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

};


struct MockPath : public Mock<IPath> {

    MockPath();
    MockPath(const string& p);

    MockPath& operator<<(const string& s);
    MockPath& operator<<(const json& j);

};


#endif
