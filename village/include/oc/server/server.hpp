// oc::server — Header-only C++11 reimplementation of opencog/cogserver
// CogServer, RequestManager, ModuleManager, Shell, SchemeShell
// Zero external dependencies (no actual networking; simulated command dispatch)
// SPDX-License-Identifier: AGPL-3.0
#ifndef OC_SERVER_HPP
#define OC_SERVER_HPP

#include "../atomspace/atomspace.hpp"

namespace oc {
namespace server {

// ─────────────────────────────────────────────────────────────────────────────
// Request — Abstract command (mirrors opencog::Request)
// ─────────────────────────────────────────────────────────────────────────────
class Request {
public:
    virtual ~Request() {}
    virtual bool execute() = 0;
    virtual std::string name() const = 0;
    virtual std::string description() const { return ""; }

    void set_args(const std::vector<std::string>& args) { args_ = args; }
    const std::vector<std::string>& args() const { return args_; }

    void set_output(std::string* out) { output_ = out; }

protected:
    std::vector<std::string> args_;
    std::string* output_ = nullptr;

    void write(const std::string& s) {
        if (output_) *output_ += s;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// RequestFactory — Factory method pattern for requests
// ─────────────────────────────────────────────────────────────────────────────
class AbstractRequestFactory {
public:
    virtual ~AbstractRequestFactory() {}
    virtual Request* create() const = 0;
    virtual std::string description() const = 0;
};

template<typename T>
class RequestFactory : public AbstractRequestFactory {
public:
    explicit RequestFactory(const std::string& desc = "") : desc_(desc) {}
    Request* create() const override { return new T(); }
    std::string description() const override { return desc_; }
private:
    std::string desc_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Module — Abstract loadable module (mirrors opencog::Module)
// ─────────────────────────────────────────────────────────────────────────────
class CogServer;  // Forward declaration

class Module {
public:
    virtual ~Module() {}
    virtual void init(CogServer& server) = 0;
    virtual std::string name() const = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// RequestManager — Registry and dispatch queue for requests
// ─────────────────────────────────────────────────────────────────────────────
class RequestManager {
public:
    RequestManager() = default;
    ~RequestManager() {
        for (auto& kv : factories_) delete kv.second;
    }

    void register_request(const std::string& name,
                           AbstractRequestFactory* factory) {
        factories_[name] = factory;
    }

    void unregister_request(const std::string& name) {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            delete it->second;
            factories_.erase(it);
        }
    }

    Request* create_request(const std::string& name) const {
        auto it = factories_.find(name);
        if (it == factories_.end()) return nullptr;
        return it->second->create();
    }

    std::vector<std::string> list_requests() const {
        std::vector<std::string> names;
        for (auto& kv : factories_) names.push_back(kv.first);
        return names;
    }

    std::string get_description(const std::string& name) const {
        auto it = factories_.find(name);
        if (it == factories_.end()) return "";
        return it->second->description();
    }

    // Queue a request for execution
    void enqueue(Request* req) {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(req);
    }

    // Process one request from the queue
    bool process_one() {
        Request* req = nullptr;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (queue_.empty()) return false;
            req = queue_.front();
            queue_.pop();
        }
        if (req) {
            req->execute();
            delete req;
            return true;
        }
        return false;
    }

    // Process all pending requests
    size_t process_all() {
        size_t count = 0;
        while (process_one()) count++;
        return count;
    }

    size_t queue_size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();
    }

private:
    std::map<std::string, AbstractRequestFactory*> factories_;
    mutable std::mutex mtx_;
    std::queue<Request*> queue_;
};

// ─────────────────────────────────────────────────────────────────────────────
// ModuleManager — Dynamic module registry (simulated, no dlopen)
// ─────────────────────────────────────────────────────────────────────────────
class ModuleManager {
public:
    ModuleManager() = default;
    ~ModuleManager() {
        for (auto& kv : modules_) delete kv.second;
    }

    bool load_module(Module* mod) {
        if (!mod) return false;
        std::string n = mod->name();
        if (modules_.count(n)) return false;
        modules_[n] = mod;
        return true;
    }

    bool unload_module(const std::string& name) {
        auto it = modules_.find(name);
        if (it == modules_.end()) return false;
        delete it->second;
        modules_.erase(it);
        return true;
    }

    Module* get_module(const std::string& name) const {
        auto it = modules_.find(name);
        return (it != modules_.end()) ? it->second : nullptr;
    }

    std::vector<std::string> list_modules() const {
        std::vector<std::string> names;
        for (auto& kv : modules_) names.push_back(kv.first);
        return names;
    }

    size_t module_count() const { return modules_.size(); }

private:
    std::map<std::string, Module*> modules_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Shell — Interactive command shell (text-based, no actual networking)
// ─────────────────────────────────────────────────────────────────────────────
class Shell {
public:
    Shell(AtomSpace& as, RequestManager& rm)
        : as_(as), rm_(rm), prompt_("opencog> ") {}

    void set_prompt(const std::string& p) { prompt_ = p; }
    const std::string& prompt() const { return prompt_; }

    // Process a line of input, return output
    std::string eval(const std::string& line) {
        auto tokens = util::tokenize(line);
        if (tokens.empty()) return "";

        std::string cmd = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());

        // Built-in commands
        if (cmd == "help") return do_help();
        if (cmd == "list") return do_list();
        if (cmd == "quit" || cmd == "exit") return "Bye.\n";
        if (cmd == "stats") return do_stats();

        // Try registered requests
        Request* req = rm_.create_request(cmd);
        if (req) {
            req->set_args(args);
            std::string output;
            req->set_output(&output);
            req->execute();
            delete req;
            return output;
        }

        return "Unknown command: " + cmd + "\n";
    }

private:
    AtomSpace& as_;
    RequestManager& rm_;
    std::string prompt_;

    std::string do_help() {
        std::ostringstream os;
        os << "Available commands:\n";
        os << "  help   - Show this help\n";
        os << "  list   - List registered requests\n";
        os << "  stats  - Show AtomSpace statistics\n";
        os << "  quit   - Exit shell\n";
        auto reqs = rm_.list_requests();
        for (auto& r : reqs) {
            os << "  " << r << " - " << rm_.get_description(r) << "\n";
        }
        return os.str();
    }

    std::string do_list() {
        std::ostringstream os;
        auto reqs = rm_.list_requests();
        for (auto& r : reqs) os << r << "\n";
        return os.str();
    }

    std::string do_stats() {
        std::ostringstream os;
        os << "AtomSpace size: " << as_.size() << " atoms\n";
        return os.str();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// SchemeShell — S-expression evaluator over AtomSpace
// ─────────────────────────────────────────────────────────────────────────────
class SchemeShell {
public:
    explicit SchemeShell(AtomSpace& as) : as_(as) {}

    std::string eval(const std::string& expr) {
        // Minimal Scheme-like evaluator for AtomSpace operations
        std::string trimmed = trim(expr);
        if (trimmed.empty()) return "";

        // Parse the outermost s-expression
        if (trimmed[0] != '(') return "Error: expected s-expression";

        auto tokens = tokenize_sexpr(trimmed);
        if (tokens.empty()) return "Error: empty expression";

        std::string fn = tokens[0];

        if (fn == "cog-new-node") {
            return do_new_node(tokens);
        } else if (fn == "cog-new-link") {
            return do_new_link(tokens);
        } else if (fn == "cog-atomspace-size") {
            return std::to_string(as_.size());
        } else if (fn == "cog-get-atoms") {
            return do_get_atoms(tokens);
        } else if (fn == "cog-incoming-set") {
            return do_incoming(tokens);
        } else if (fn == "cog-set-tv!") {
            return do_set_tv(tokens);
        } else if (fn == "cog-tv") {
            return do_get_tv(tokens);
        } else if (fn == "cog-prt-atomspace") {
            return do_print_as();
        }

        return "Error: unknown function '" + fn + "'";
    }

private:
    AtomSpace& as_;

    static std::string trim(const std::string& s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) return "";
        size_t b = s.find_last_not_of(" \t\r\n");
        return s.substr(a, b - a + 1);
    }

    // Tokenize an s-expression into function name and arguments
    std::vector<std::string> tokenize_sexpr(const std::string& expr) {
        std::vector<std::string> tokens;
        // Strip outer parens
        std::string inner = expr.substr(1, expr.size() - 2);
        inner = trim(inner);

        size_t pos = 0;
        while (pos < inner.size()) {
            // Skip whitespace
            while (pos < inner.size() && isspace(inner[pos])) pos++;
            if (pos >= inner.size()) break;

            if (inner[pos] == '\'') {
                // Quoted symbol
                pos++;
                size_t start = pos;
                while (pos < inner.size() && !isspace(inner[pos]) && inner[pos] != ')') pos++;
                tokens.push_back(inner.substr(start, pos - start));
            } else if (inner[pos] == '"') {
                // String literal
                pos++;
                size_t start = pos;
                while (pos < inner.size() && inner[pos] != '"') pos++;
                tokens.push_back(inner.substr(start, pos - start));
                if (pos < inner.size()) pos++;
            } else if (inner[pos] == '(') {
                // Nested expression
                int depth = 1;
                size_t start = pos;
                pos++;
                while (pos < inner.size() && depth > 0) {
                    if (inner[pos] == '(') depth++;
                    else if (inner[pos] == ')') depth--;
                    pos++;
                }
                tokens.push_back(inner.substr(start, pos - start));
            } else {
                // Regular token
                size_t start = pos;
                while (pos < inner.size() && !isspace(inner[pos]) && inner[pos] != ')') pos++;
                tokens.push_back(inner.substr(start, pos - start));
            }
        }
        return tokens;
    }

    std::string do_new_node(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) return "Error: (cog-new-node 'TypeName \"name\")";
        Type t = TypeRegistry::instance().get_type(tokens[1]);
        if (t == types::NOTYPE) return "Error: unknown type '" + tokens[1] + "'";
        Handle h = as_.add_node(t, tokens[2]);
        return std::to_string(h);
    }

    std::string do_new_link(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) return "Error: (cog-new-link 'TypeName atoms...)";
        Type t = TypeRegistry::instance().get_type(tokens[1]);
        if (t == types::NOTYPE) return "Error: unknown type '" + tokens[1] + "'";
        HandleSeq outgoing;
        for (size_t i = 2; i < tokens.size(); ++i) {
            // Each arg could be a handle number or a nested expression
            if (tokens[i][0] == '(') {
                // Evaluate nested expression
                std::string result = eval(tokens[i]);
                try { outgoing.push_back(std::stoull(result)); }
                catch (...) { return "Error: invalid handle from nested expr"; }
            } else {
                try { outgoing.push_back(std::stoull(tokens[i])); }
                catch (...) { return "Error: invalid handle '" + tokens[i] + "'"; }
            }
        }
        Handle h = as_.add_link(t, outgoing);
        return std::to_string(h);
    }

    std::string do_get_atoms(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) return "Error: (cog-get-atoms 'TypeName)";
        Type t = TypeRegistry::instance().get_type(tokens[1]);
        auto handles = as_.get_by_type(t, true);
        std::ostringstream os;
        os << "(";
        for (size_t i = 0; i < handles.size(); ++i) {
            if (i > 0) os << " ";
            os << as_.atom_to_string(handles[i]);
        }
        os << ")";
        return os.str();
    }

    std::string do_incoming(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) return "Error: (cog-incoming-set handle)";
        Handle h = 0;
        try { h = std::stoull(tokens[1]); }
        catch (...) { return "Error: invalid handle"; }
        auto inc = as_.get_incoming(h);
        std::ostringstream os;
        os << "(";
        for (size_t i = 0; i < inc.size(); ++i) {
            if (i > 0) os << " ";
            os << inc[i];
        }
        os << ")";
        return os.str();
    }

    std::string do_set_tv(const std::vector<std::string>& tokens) {
        if (tokens.size() < 4) return "Error: (cog-set-tv! handle mean conf)";
        Handle h = 0;
        try { h = std::stoull(tokens[1]); } catch (...) { return "Error"; }
        double m = std::stod(tokens[2]);
        double c = std::stod(tokens[3]);
        as_.set_tv(h, TruthValue(m, c));
        return "ok";
    }

    std::string do_get_tv(const std::vector<std::string>& tokens) {
        if (tokens.size() < 2) return "Error: (cog-tv handle)";
        Handle h = 0;
        try { h = std::stoull(tokens[1]); } catch (...) { return "Error"; }
        const Atom* a = as_.get_atom(h);
        if (!a) return "(null)";
        return a->tv.to_string();
    }

    std::string do_print_as() {
        std::ostringstream os;
        as_.foreach_atom([&](const Atom& a) {
            os << as_.atom_to_string(a.handle) << "\n";
        });
        return os.str();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// CogServer — Main server combining all managers
// ─────────────────────────────────────────────────────────────────────────────
class CogServer {
public:
    explicit CogServer(AtomSpace& as)
        : as_(as), shell_(as, request_mgr_), scheme_shell_(as),
          running_(false), cycle_count_(0) {}

    AtomSpace& atomspace() { return as_; }
    RequestManager& request_manager() { return request_mgr_; }
    ModuleManager& module_manager() { return module_mgr_; }
    Shell& shell() { return shell_; }
    SchemeShell& scheme_shell() { return scheme_shell_; }

    // Initialize all loaded modules
    void init_modules() {
        auto names = module_mgr_.list_modules();
        for (auto& n : names) {
            Module* m = module_mgr_.get_module(n);
            if (m) m->init(*this);
        }
    }

    // Server loop: process requests
    void server_loop(int max_cycles = -1) {
        running_ = true;
        while (running_) {
            request_mgr_.process_all();
            cycle_count_++;
            if (max_cycles > 0 && cycle_count_ >= static_cast<uint64_t>(max_cycles)) {
                running_ = false;
            }
        }
    }

    void stop() { running_ = false; }
    bool is_running() const { return running_; }
    uint64_t cycle_count() const { return cycle_count_; }

private:
    AtomSpace& as_;
    RequestManager request_mgr_;
    ModuleManager module_mgr_;
    Shell shell_;
    SchemeShell scheme_shell_;
    bool running_;
    uint64_t cycle_count_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Built-in requests
// ─────────────────────────────────────────────────────────────────────────────
class ListAtomsRequest : public Request {
public:
    std::string name() const override { return "list-atoms"; }
    std::string description() const override { return "List all atoms in the AtomSpace"; }
    bool execute() override {
        write("(use AtomSpace directly)\n");
        return true;
    }
};

class ShutdownRequest : public Request {
public:
    std::string name() const override { return "shutdown"; }
    std::string description() const override { return "Shut down the CogServer"; }
    bool execute() override {
        write("Shutting down...\n");
        return true;
    }
};

} // namespace server
} // namespace oc

#endif // OC_SERVER_HPP
