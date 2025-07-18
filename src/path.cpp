#include "path.hpp"


unique_ptr<struct stat> Path::stat() const {
    unique_ptr<struct stat> s(new struct stat);
    if (::stat(this->m_path.c_str(), s.get()) < 0)
        throw mucs_exception("Failed to stat file:", this->m_path);
    return s;
}


Path::Path(const string& s) {
    this->m_path = s;

    // Trim trailing slashes
    size_t end_path = s.find_last_not_of('/');
    if (end_path != string::npos)
        this->m_path.erase(end_path+1);
}


Path::operator string() const {
    return this->m_path;
}


Path operator+(const Path& p, const string& ext) {
    Path other(p.m_path + ext);
    return other;
}


Path& operator+=(Path& p, const string& ext) {
    p.append(ext);
    return p;
}


Path operator/(const Path& p, const string& rel_path) {
    Path other(p.m_path);
    other.join(rel_path);
    return other;
}


Path operator/(const Path& a, const Path& b) {
    return a / b.m_path;
}


Path& operator/=(Path& p, const string& rel_path) {
    p.join(rel_path);
    return p;
}


Path& operator/=(Path& a, const Path& b) {
    return a /= b.m_path;
}


string Path::str() const {
    return this->m_path;
}


string Path::basename() const {
    size_t part = this->m_path.find_last_of('/');
    return part == string::npos || part == this->m_path.size() - 1
        ? this->m_path
        : this->m_path.substr(part + 1);
}


Path& Path::append(const string& ext) {
    this->m_path += ext;
    return *this;
}


Path& Path::join(const string& rel_path) {
    this->m_path = join_paths(this->m_path, rel_path);
    return *this;
}


bool Path::exists() const {
    struct stat s;
    return ::lstat(this->m_path.c_str(), &s) == 0;
}


int Path::type() const {
    auto s = this->stat();
    return (S_ISREG(s->st_mode) ? Path::File : 0)
         | (S_ISDIR(s->st_mode) ? Path::Dir  : 0)
         | (S_ISLNK(s->st_mode) ? Path::Link : 0)
         ;
}


/**
 * A file is considered executable if the execution bit is set for either the
 * current UID or the current GID.
 */
bool Path::is_exe() const {
    auto s = this->stat();
    return s->st_mode & S_IXUSR || s->st_mode & S_IXGRP;
}


bool Path::is_file() const {
    return S_ISREG(this->stat()->st_mode);
}


bool Path::is_dir() const {
    return S_ISDIR(this->stat()->st_mode);
}


struct utimbuf Path::get_times() const {
    auto s = this->stat();
    return {s->st_atim.tv_sec, s->st_mtim.tv_sec};
}


void Path::set_times(struct utimbuf times) {
    if (utime(this->m_path.c_str(), &times) < 0)
        throw mucs_exception("Failed to set times for file:", this->m_path);
}


void Path::chmod(mode_t mode) const {
    if (::chmod(this->m_path.c_str(), mode) < 0)
        throw mucs_exception("Unable to chmod file:", this->m_path);
}


bool Path::link_to(const Path& target) const {
    if (this->exists())
        this->rm();
    return symlink(target.str().c_str(), this->m_path.c_str()) == 0;
}


void Path::rm() const {
    if (::remove(this->m_path.c_str()) < 0)
        throw mucs_exception("Failed to remove file:", this->m_path);
}


void Path::rm_recurse() const {
    if (this->is_dir())
        for (const string& child : this->ls())
            (*this / child).rm_recurse();
    this->rm();
}


string Path::read() const {
    ifstream is(this->m_path);
    try {
        stringstream buf;
        buf << is.rdbuf();
        is.close();
        return buf.str();
    } catch (const exception& e) {
        is.close();
        throw;
    }
}


vector<string> Path::read_lines() const {
    ifstream is(this->m_path);
    vector<string> lines;
    do {
        try {
            vector<string>::iterator it = lines.emplace(lines.end());
            getline(is, *it);
        } catch (const exception& e) {
            is.close();
            throw;
        }
    } while (not is.eof());
    return lines;
}


void Path::copy_into(const Path& dir, mode_t mode, bool preserve_times) const {
    if (dir.exists() == false)
        dir.mkdir_recurse();

    if (dir.is_dir() == false)
        throw mucs_exception(
            "Destination exists but is not a directory:", dir.m_path);

    Path dest = dir / this->basename();

    ifstream src_fs(this->m_path, ios::in | ios::binary);
    ofstream dest_fs(dest.m_path, ios::out | ios::binary | ios::trunc);

    dest_fs << src_fs.rdbuf();

    src_fs.close();
    dest_fs.close();

    if (preserve_times)
        dest.set_times(this->get_times());

    dest.chmod(mode);
}


vector<string> Path::ls() const {
    return stl_transform_into<vector<string>>(
        this->ls_base(),
        bind(join_paths<>, this->m_path, _1)
    );
}


vector<string> Path::ls_base() const {
    DIR *dir = opendir(this->m_path.c_str());
    if (not dir)
        throw mucs_exception("Could not list directory");

    vector<string> children;
    struct dirent *ent;
    string name;
    Path child;

    while ((ent = readdir(dir)) != nullptr) {
        name = string(ent->d_name);

        if (name == "." || name == "..")
            continue;

        children.push_back(name);
    }

    closedir(dir);
    return children;
}


void Path::chmod_recurse(mode_t mode, int filter) const {
    if (filter & Path::Dir)
        this->chmod(mode);
    for (const Path& child : this->ls()) {
        if (filter & child.type())
            child.chmod(mode);
        if (child.is_dir())
            child.chmod_recurse(mode, filter);
    }
}


void Path::mkdir(mode_t mode) const {
    if (::mkdir(this->m_path.c_str(), mode) < 0)
        throw mucs_exception("Failed to make directory:", this->m_path);
}


void Path::mkdir_recurse(mode_t mode) const {
    string p = this->m_path;

    if (p.empty())
        throw mucs_exception("Path is invalid:", p);

    vector<string> components = string_split(p, "/");
    auto b = components.begin();

    Path cur(b->empty() ? "/" : *b);
    for (auto it=b+1; it!=components.end(); it++) {
        cur /= *it;
        if (cur.exists()) {
            if (cur.is_dir() == false)
                throw mucs_exception(
                    "File exists but is not a directory:", cur.m_path);
        } else {
            cur.mkdir(mode);
        }
    }
}


MockPath::MockPath() : Mock<Path>() {
    When(Method((*this), exists)).AlwaysReturn(true);
    When(Method((*this), is_file)).AlwaysReturn(true);
    When(Method((*this), is_dir)).AlwaysReturn(false);
}


MockPath::MockPath(const string& p) : MockPath() {
    When(Method((*this), str)).AlwaysReturn(p);
}


Path& MockPath::get() {
    When(Method((*this), read)).AlwaysReturn(this->file_contents.str());
    return Mock<Path>::get();
}


MockPath& MockPath::operator<<(const string& data) {
    this->file_contents << data;
    return *this;
}
