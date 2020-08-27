#include "config.hpp"


Config::Config() {
}


Config::Config(const IPath& config_p) {
    if (not config_p.exists())
        throw mucs_exception("Config file does not exist: " + config_p.str());
    if (not config_p.is_file())
        throw mucs_exception(
            "Config path must be a regular file: " + config_p.str());

    json data;
    try {
        data = json::parse(config_p.read());
    } catch (const json::parse_error& pe) {
        throw mucs_exception(
            "Failed to parse config: " + config_p.str() + "\n" + pe.what());
    }

    this->parse(data, config_p.str());
}


Config& Config::parse(const json& root, const string& filename) {
    this->filename = filename;

    this->validate_config(root);

    root["course_id"].get_to(this->course_id);
    root["admin_hash"].get_to(this->admin_hash);
    root["homeworks"].get_to(this->homeworks);
    root["lab-sessions"].get_to(this->lab_sessions);
    root["lab-assignments"].get_to(this->lab_assignments);
    root["roster"].get_to(this->roster);

    return *this;
}


void Config::validate_config(const json& root) const {
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
    NlohmannJsonAdapter adapter(root);
    if (not validator.validate(schema, adapter, &results)) {
        ostringstream msg;
        ValidationResults::Error e;
        msg << "Invalid config: " << filename;
        while (results.numErrors() > 0) {
            results.popError(e);
            msg << '\n';
            for (const string& comp : e.context)
                msg << comp;
            msg << ": " << e.description;
        }
        throw mucs_exception(msg.str());
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


string Config::get_assignment(const string& type) const {
    if (type == "lab")
        return this->get_current_lab();
    else if (type == "hw")
        return this->get_current_hw();

    throw mucs_exception("Assignment type not recognized: " + type);
}


string Config::get_current_lab() const {
    auto today = get_day();
    for (auto& e : this->lab_assignments)
        if (e.second.start <= today && today < e.second.end)
            return e.first;

    throw mucs_exception(
        "No open lab assignments for course: " + this->course_id);
}


string Config::get_current_hw() const {
    for (const Hw& hw : this->homeworks)
        if (NOW < hw.duedate)
            return hw.name;

    throw mucs_exception(
        "No open homework assignments for course: " + this->course_id);
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
