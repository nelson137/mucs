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


IPath& Path::append(const string& ext) {
    this->m_path += ext;
    return *this;
}


IPath& Path::join(const string& rel_path) {
    this->m_path = join_paths(this->m_path, rel_path);
    return *this;
}


bool Path::exists() const {
    struct stat s;
    return ::stat(this->m_path.c_str(), &s) == 0;
}


bool Path::is_file() const {
    return S_ISREG(this->stat()->st_mode);
}


bool Path::is_dir() const {
    return S_ISDIR(this->stat()->st_mode);
}


bool Path::link_to(const IPath& target) const {
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


void Path::copy_into(const Path& dir, mode_t mode) const {
    if (dir.exists() == false)
        dir.mkdir_recurse();

    if (dir.is_dir() == false)
        throw mucs_exception(
            "Destination exists but is not a directory:", dir.m_path);

    const Path& dest_p = dir / this->basename();

    ifstream src(this->m_path, ios::in | ios::binary);
    ofstream dest(dest_p.m_path, ios::out | ios::binary | ios::trunc);

    dest << src.rdbuf();

    src.close();
    dest.close();

    if (chmod(dest_p.m_path.c_str(), mode) < 0)
        throw mucs_exception("Unable to chmod file:", dest_p.m_path);
}


vector<string> Path::ls() const {
    DIR *dir = opendir(this->m_path.c_str());
    if (dir == nullptr)
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


int Path::mkdir() const {
    return ::mkdir(this->m_path.c_str(), 0775);
}


int Path::mkdir_recurse() const {
    string p = this->m_path;

    // Trim leading and trailing slashes
    size_t begin = p.find_first_not_of('/');
    size_t end = p.find_last_not_of('/');
    if (begin == string::npos || end == string::npos)
        return -1;
    p = p.substr(begin, end-begin+1);

    Path cur("/");
    for (auto& c : string_split(p, "/")) {
        cur /= c;
        if (cur.exists()) {
            if (not cur.is_dir())
                return -1;
        } else {
            if (cur.mkdir() < 0)
                return -1;
        }
    }

    return 0;
}


MockPath::MockPath() : Mock<IPath>() {
    When(Method((*this), exists)).Return(true);
    When(Method((*this), is_file)).Return(true);
    When(Method((*this), is_dir)).Return(false);
}


MockPath::MockPath(const string& p) : MockPath() {
    When(Method((*this), str)).Return(p);
}


MockPath& MockPath::operator<<(const string& s) {
    When(Method((*this), read)).Return(s);
    return *this;
}


MockPath& MockPath::operator<<(const json& j) {
    return this->operator<<(j.dump());
}
