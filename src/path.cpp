#include "path.hpp"


inline void Path::stat() {
    this->m_stat_ret = ::stat(this->m_path.c_str(), &this->m_stat);
}


inline Path::Path() {
    memset(&this->m_stat, 0, sizeof(this->m_stat));
}


Path::Path(const string& s) : Path() {
    this->m_path = s;

    // Trim trailing slashes
    size_t end_path = s.find_last_not_of('/');
    if (end_path != string::npos)
        this->m_path.erase(end_path+1);

    this->stat();
}


Path Path::operator/(const string& rel_path) const {
    return Path(join_paths(this->m_path, rel_path));
}


Path Path::operator/(const Path& other) const {
    return this->operator/(other.str());
}


Path& Path::operator/=(const string& rel_path) {
    this->m_path = join_paths(this->m_path, rel_path);
    this->stat();
    return *this;
}


Path& Path::operator/=(const Path& other) {
    return this->operator/=(other.str());
}


ostream& operator<<(ostream& os, const Path& p) {
    return os << p.str();
}


inline string Path::str() const {
    return this->m_path;
}


inline int Path::chdir() const {
    return ::chdir(this->m_path.c_str()) == 0;
}


bool Path::exists() const {
    return this->m_stat_ret == 0;
}


inline bool Path::is_dir() const {
    return S_ISDIR(this->m_stat.st_mode);
}


inline bool Path::is_file() const {
    return S_ISREG(this->m_stat.st_mode);
}


vector<string> Path::ls() const {
    DIR *dir = opendir(this->m_path.c_str());
    if (dir == nullptr)
        cerr << "Could not list config directory" << endl;
        // throw mucs_exception("Could not list config directory");

    vector<string> children;
    struct dirent *ent;
    string name;
    Path child;

    while ((ent = readdir(dir)) != nullptr) {
        name = string(ent->d_name);

        if (name == "." || name == "..")
            continue;

        children.push_back(name);

        // // Recurse into sub-directories
        // child = Path(join_paths(this->m_path, name));
        // if (child.is_dir()) {
            // vector<string> grandchildren = child.ls();
            // stl_extend(children, grandchildren);
            // children.insert(
                // children.end(), grandchildren.begin(), grandchildren.end());
        // } else if (child.is_file()) {
            // children.push_back(child);
        // }
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


int Path::rm() const {
    return ::remove(this->m_path.c_str());
}


int Path::rm_recurse() const {
    Path cur = *this;
    int ret;

    if (this->is_dir()) {
        for (auto& child : this->ls()) {
            cur /= child;
            if ((ret = cur.rm()) < 0)
                return ret;
        }
    } else if (this->is_file()) {
        return ::remove(this->m_path.c_str());
    }

    return 0;
}
