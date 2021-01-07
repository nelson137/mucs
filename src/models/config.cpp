#include "config.hpp"


Config::Config() {
}


Config::Config(const Path& config_p) : filename(config_p.str()) {
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


/**
 * Types used:
 *   NlohmannJsonAdapter: wrapper for nlohmann::json type; they are different
 *     libraries so all nlohmann::json objects have to be adapted for the
 *     validator library
 *   Schema: contains rules for json validation
 *   SchemaParser: parses json objects into schemas of a specific version
 *   Validator: validates the target json against a Schema object, puts errors
 *     in a ValidationResults object
 *   ValidationResults: contains error messages and traceback info
 */
Config& Config::validate(const Path& schema_p) {
    // Make sure schema file exists
    if (not schema_p.exists())
        throw mucs_exception("Schema does not exist: " + schema_p.str());
    if (not schema_p.is_file())
        throw mucs_exception(
            "Schema path must be a regular file: " + schema_p.str());

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


Config& Config::deserialize() {
    this->j_root["course_id"].get_to(this->course_id);
    this->j_root["admin_hash"].get_to(this->admin_hash);
    this->j_root["homeworks"].get_to(this->homeworks);
    this->j_root["lab-sessions"].get_to(this->lab_sessions);
    this->j_root["lab-assignments"].get_to(this->lab_assignments);

    return *this;
}


void Config::load_roster(const Path& roster_d) {
    if (not roster_d.exists())
        throw mucs_exception("Roster directory does not exist:", roster_d);
    if (not roster_d.is_dir())
        throw mucs_exception("Roster path must be a directory:", roster_d);

    for (const Path& lab_roster_p : roster_d.ls()) {
        string lab_id = lab_roster_p.basename();
        lab_roster_p.for_each_line([&] (const string& line) {
            string user = string_strip(line);
            if (user.size())
                this->roster.safe_insert(user, lab_id);
        });
    }
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


const IAssignment& Config::validate_and_get_asgmt(const string& name) const {
    for (const LabAsgmt& la : this->lab_assignments)
        if (la.name == name)
            return la;

    for (const Hw& hw : this->homeworks)
        if (hw.name == name)
            return hw;

    throw mucs_exception("No such assignment exists: " + name);
}


const LabSesh& Config::validate_and_get_lab(const string& user) const {
    const string& id = this->roster.safe_get(user);

    auto it = this->lab_sessions.find(id);
    if (it == this->lab_sessions.end())
        throw mucs_exception(
            "Student '" + user + "' has invalid lab: " + id);
    const LabSesh& lab = it->second;

    if (lab.is_active() == false)
        throw mucs_exception(lab.format(
            "Lab {id} is not in session: {weekday} from {start} to {end}"
        ));

    return lab;
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
}


void to_json(json& j, const Config& c) {
    j = {
        {"course_id", c.course_id},
        {"admin_hash", c.admin_hash},
        {"homeworks", c.homeworks},
        {"lab-sessions", c.lab_sessions},
        {"lab-assignments", c.lab_assignments},
    };
}
