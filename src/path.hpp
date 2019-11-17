#ifndef PATH_HPP
#define PATH_HPP


#include <ostream>
#include <string>
#include <vector>

#include <dirent.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "util.hpp"


class Path {

private:
    string m_path;
    int m_stat_ret;
    struct stat m_stat;

    void stat();

public:
    Path();
    Path(const string& s);

    Path  operator/ (const string& rel_path) const;
    Path  operator/ (const Path&   other   ) const;
    Path& operator/=(const string& rel_path);
    Path& operator/=(const Path&   other   );

    friend ostream& operator<<(ostream& os, const Path& p);

    string str() const;

    bool exists() const;
    bool is_dir() const;
    bool is_file() const;
    vector<string> ls() const;
    int mkdir() const;
    int mkdir_recurse() const;
    int rm() const;
    int rm_recurse() const;

};


#endif
