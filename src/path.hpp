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
    virtual bool is_file() const = 0;
    virtual bool is_dir() const = 0;

    virtual void chmod(mode_t mode) const = 0;

    virtual bool link_to(const IPath& target) const = 0;

    virtual void rm() const = 0;
    virtual void rm_recurse() const = 0;

    /**
     * Files only
     */

    virtual string read() const = 0;

    /**
     * Directories only
     */

    virtual vector<string> ls() const = 0;

    virtual int mkdir() const = 0;
    virtual int mkdir_recurse() const = 0;

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

    string str() const override;
    string basename() const override;

    IPath& append(const string& ext) override;
    IPath& join(const string& rel_path) override;

    bool exists() const override;
    bool is_file() const override;
    bool is_dir() const override;

    void chmod(mode_t mode) const override;

    bool link_to(const IPath& target) const override;

    void rm() const override;
    void rm_recurse() const override;

    /**
     * Files only
     */

    string read() const override;

    void copy_into(const Path& dir, mode_t mode = 0644) const;

    /**
     * Directories only
     */

    vector<string> ls() const override;

    int mkdir() const override;
    int mkdir_recurse() const override;

};


struct MockPath : public Mock<IPath> {

    MockPath();
    MockPath(const string& p);

    MockPath& operator<<(const string& s);
    MockPath& operator<<(const json& j);

};


#endif
