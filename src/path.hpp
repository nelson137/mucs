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

using json = nlohmann::json;


class IPath {

protected:
    string m_path;

    virtual unique_ptr<struct stat> stat() const = 0;

public:
    enum {
        File = 1 << 0,
        Dir  = 1 << 1,
        Link = 1 << 2,
    };

    virtual string str() const = 0;
    virtual string basename() const = 0;

    virtual IPath& append(const string& ext) = 0;
    virtual IPath& join(const string& rel_path) = 0;

    virtual bool exists() const = 0;
    virtual int type() const = 0;
    virtual bool is_exe() const = 0;
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
    virtual vector<string> ls_base() const = 0;

    virtual void chmod_recurse(mode_t mode, int filter = File|Dir) const = 0;

    virtual void mkdir(mode_t mode = 0775) const = 0;
    virtual void mkdir_recurse(mode_t mode = 0775) const = 0;

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
    int type() const override;
    bool is_exe() const override;
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
    vector<string> ls_base() const override;

    void chmod_recurse(mode_t mode, int filter = File|Dir) const override;

    void mkdir(mode_t mode = 0775) const override;
    void mkdir_recurse(mode_t mode = 0775) const override;

};


struct MockPath : public Mock<IPath> {

    ostringstream data;

    MockPath();
    MockPath(const string& p);

    IPath& get();

    MockPath& operator<<(const string& data);

};


#endif
