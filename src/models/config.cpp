#include "config.hpp"


Config::Config() {
}


Config::Config(const IPath& config_p) : filename(config_p.str()) {
    if (not config_p.exists())
        throw mucs_exception("Config file does not exist: " + config_p.str());
    if (not config_p.is_file())
        throw mucs_exception(
            "Config path must be a regular file: " + config_p.str());

    try {
        this->j_root = json::parse(config_p.read());
    } catch (const json::parse_error& pe) {
        throw mucs_exception(
            "Failed to parse config: " + config_p.str() + "\n" + pe.what());
    }
}


Config::Config(json root, string fn) : j_root(root), filename(fn) {
}


Config& Config::parse() {
    this->j_root["course_id"].get_to(this->course_id);
    this->j_root["admin_hash"].get_to(this->admin_hash);
    this->j_root["homeworks"].get_to(this->homeworks);
    this->j_root["lab-sessions"].get_to(this->lab_sessions);
    this->j_root["lab-assignments"].get_to(this->lab_assignments);
    this->j_root["roster"].get_to(this->roster);

    return *this;
}


Config& Config::validate() {
    Path schema_p = Path(SCHEMA_PATH);

    // Make sure schema file exists
    if (not schema_p.exists())
        throw mucs_exception("Schema does not exist: " + schema_p.str());
    if (not schema_p.is_file())
        throw mucs_exception(
            "Schema must be a regular file: " + schema_p.str());

    // Deserialize schema
    json schema_doc;
    try {
        schema_doc = json::parse(schema_p.read());
    } catch (const json::parse_error& pe) {
        throw mucs_exception(
            "Failed to parse schema: " + schema_p.str() + "\n" + pe.what());
    }

    // Create schema object
    Schema schema;
    SchemaParser schema_parser(SchemaParser::kDraft7);
    NlohmannJsonAdapter schema_adapter(schema_doc);
    schema_parser.populateSchema(schema_adapter, schema);

    // Validate config
    Validator validator;
    ValidationResults results;
    NlohmannJsonAdapter adapter(this->j_root);
    if (not validator.validate(schema, adapter, &results)) {
        ostringstream msg;
        ValidationResults::Error e;
        msg << "Invalid config: " << this->filename;
        while (results.numErrors() > 0) {
            results.popError(e);
            msg << '\n';
            for (const string& comp : e.context)
                msg << comp;
            msg << ": " << e.description;
        }
        throw mucs_exception(msg.str());
    }

    return *this;
}


mucs_exception Config::error(
    const string& msg,
    const initializer_list<string>& keys
) {
    ostringstream ret;
    ret << msg << ": {filename}";
    for (const auto& k : keys)
        ret << "[\"" << k << "\"]";
    return ret.str();
}


const IAssignment& Config::get_assignment(const string& name) const {
    for (const LabAsgmt& la : this->lab_assignments)
        if (la.name == name)
            return la;

    for (const Hw& hw : this->homeworks)
        if (hw.name == name)
            return hw;

    throw mucs_exception("No such assignment exists: " + name);
}


LabSesh Config::get_lab(const string& user) const {
    const vector<LabSesh>& user_labs = this->get_user_labs(user);
    LabSesh lab;

    if (user_labs.size() == 1) {
        lab = user_labs[0];
        if (lab.is_active() == false)
            throw mucs_exception(lab.format(
                "Lab {id} is not in session: {weekday} from {start} to {end}"
            ));
    } else {
        auto active_lab = stl_find_if(user_labs, mem_fn(&LabSesh::is_active));
        if (active_lab == user_labs.end())
            throw mucs_exception(
                "None of your labs are in session:", stl_join(user_labs));
        lab = *active_lab;
    }

    return lab;
}


vector<LabSesh> Config::get_user_labs(const string& user) const {
    const vector<string>& user_lab_ids = this->roster.at(user);
    vector<LabSesh> user_labs;
    user_labs.reserve(user_lab_ids.size());

    transform(
        user_lab_ids.begin(),
        user_lab_ids.end(),
        back_inserter(user_labs),
        [&] (const string& id) -> const LabSesh& {
            try {
                return this->lab_sessions.at(id);
            } catch (const out_of_range& e) {
                throw mucs_exception(
                    "User '" + user + "' has invalid lab: " + id);
            }
        }
    );

    return user_labs;
}


void from_json(const json& j, Config& c) {
    if (j.count("course_id") > 0)
        j["course_id"].get_to(c.course_id);

    if (j.count("admin_hash") > 0)
        j["admin_hash"].get_to(c.admin_hash);

    if (j.count("homeworks") > 0)
        j["homeworks"].get_to(c.homeworks);

    if (j.count("lab-sessions") > 0)
        j["lab-sessions"].get_to(c.lab_sessions);

    if (j.count("lab-assignments") > 0)
        j["lab-assignments"].get_to(c.lab_assignments);

    if (j.count("roster") > 0)
        j["roster"].get_to(c.roster);
}


void to_json(json& j, const Config& c) {
    j = {
        {"course_id", c.course_id},
        {"admin_hash", c.admin_hash},
        {"homeworks", c.homeworks},
        {"lab-sessions", c.lab_sessions},
        {"lab-assignments", c.lab_assignments},
        {"roster", c.roster},
    };
}
