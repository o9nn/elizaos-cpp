// oc::persist — Header-only C++11 reimplementation of opencog/atomspace-cog
// Distributed AtomSpace persistence: StorageNode, Serializer, ProxyNode,
// FrameServer, CogChannel (simulated networking over in-memory buffers)
// Zero external dependencies
// SPDX-License-Identifier: AGPL-3.0
#ifndef OC_PERSIST_HPP
#define OC_PERSIST_HPP

#include "../atomspace/atomspace.hpp"

namespace oc {
namespace persist {

// ─────────────────────────────────────────────────────────────────────────────
// Serializer — Serialize/deserialize atoms to/from s-expression strings
// ─────────────────────────────────────────────────────────────────────────────
class Serializer {
public:
    explicit Serializer(const AtomSpace& as) : as_(as) {}

    // Serialize an atom to s-expression string
    std::string serialize(Handle h) const {
        const Atom* a = as_.get_atom(h);
        if (!a) return "(null)";
        return atom_to_sexpr(*a);
    }

    // Serialize with truth value
    std::string serialize_full(Handle h) const {
        const Atom* a = as_.get_atom(h);
        if (!a) return "(null)";
        std::string s = atom_to_sexpr(*a);
        if (!a->tv.is_default()) {
            s += " (stv " + std::to_string(a->tv.mean) + " "
                 + std::to_string(a->tv.confidence) + ")";
        }
        return s;
    }

    // Deserialize an s-expression into an AtomSpace
    Handle deserialize(const std::string& sexpr, AtomSpace& target) const {
        size_t pos = 0;
        return parse_sexpr(sexpr, pos, target);
    }

    // Serialize entire AtomSpace
    std::string serialize_atomspace() const {
        std::ostringstream os;
        as_.foreach_atom([&](const Atom& a) {
            // Only serialize top-level atoms (not referenced by others)
            if (a.incoming.empty()) {
                os << serialize_full(a.handle) << "\n";
            }
        });
        return os.str();
    }

private:
    const AtomSpace& as_;

    std::string atom_to_sexpr(const Atom& a) const {
        std::string type_name = TypeRegistry::instance().get_name(a.type);
        if (a.is_node()) {
            return "(" + type_name + " \"" + escape(a.name) + "\")";
        }
        // Link
        std::string s = "(" + type_name;
        for (auto oh : a.outgoing) {
            const Atom* child = as_.get_atom(oh);
            if (child) s += " " + atom_to_sexpr(*child);
        }
        s += ")";
        return s;
    }

    static std::string escape(const std::string& s) {
        std::string r;
        for (char c : s) {
            if (c == '"') r += "\\\"";
            else if (c == '\\') r += "\\\\";
            else r += c;
        }
        return r;
    }

    static std::string unescape(const std::string& s) {
        std::string r;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\\' && i + 1 < s.size()) {
                r += s[i + 1];
                i++;
            } else {
                r += s[i];
            }
        }
        return r;
    }

    Handle parse_sexpr(const std::string& s, size_t& pos,
                        AtomSpace& target) const {
        skip_ws(s, pos);
        if (pos >= s.size() || s[pos] != '(') return UNDEFINED_HANDLE;
        pos++;  // skip '('

        skip_ws(s, pos);
        std::string type_name = read_token(s, pos);
        Type t = TypeRegistry::instance().get_type(type_name);
        if (t == types::NOTYPE) return UNDEFINED_HANDLE;

        skip_ws(s, pos);

        // Check if it's a node (next char is '"')
        if (pos < s.size() && s[pos] == '"') {
            pos++;  // skip opening quote
            std::string name;
            while (pos < s.size() && s[pos] != '"') {
                if (s[pos] == '\\' && pos + 1 < s.size()) {
                    name += s[pos + 1];
                    pos += 2;
                } else {
                    name += s[pos];
                    pos++;
                }
            }
            if (pos < s.size()) pos++;  // skip closing quote
            skip_ws(s, pos);

            // Check for truth value
            TruthValue tv;
            if (pos < s.size() && s[pos] == ')') {
                pos++;
            } else {
                // Might have truth value
                pos++;  // skip ')'
            }
            // Look ahead for (stv ...)
            skip_ws(s, pos);
            if (pos + 4 < s.size() && s.substr(pos, 4) == "(stv") {
                tv = parse_tv(s, pos);
            }

            return target.add_node(t, name, tv);
        }

        // It's a link — parse outgoing atoms
        HandleSeq outgoing;
        while (pos < s.size()) {
            skip_ws(s, pos);
            if (pos >= s.size() || s[pos] == ')') break;
            Handle child = parse_sexpr(s, pos, target);
            if (child != UNDEFINED_HANDLE) outgoing.push_back(child);
        }
        if (pos < s.size()) pos++;  // skip ')'

        // Check for truth value
        TruthValue tv;
        skip_ws(s, pos);
        if (pos + 4 < s.size() && s.substr(pos, 4) == "(stv") {
            tv = parse_tv(s, pos);
        }

        return target.add_link(t, outgoing, tv);
    }

    TruthValue parse_tv(const std::string& s, size_t& pos) const {
        // Parse (stv mean confidence)
        if (s.substr(pos, 4) != "(stv") return DEFAULT_TV;
        pos += 4;
        skip_ws(s, pos);
        std::string mean_s = read_token(s, pos);
        skip_ws(s, pos);
        std::string conf_s = read_token(s, pos);
        skip_ws(s, pos);
        if (pos < s.size() && s[pos] == ')') pos++;
        try {
            return TruthValue(std::stod(mean_s), std::stod(conf_s));
        } catch (...) {
            return DEFAULT_TV;
        }
    }

    static void skip_ws(const std::string& s, size_t& pos) {
        while (pos < s.size() && isspace(s[pos])) pos++;
    }

    static std::string read_token(const std::string& s, size_t& pos) {
        std::string tok;
        while (pos < s.size() && !isspace(s[pos]) && s[pos] != '(' && s[pos] != ')' && s[pos] != '"') {
            tok += s[pos++];
        }
        return tok;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// StorageNode — Abstract storage backend interface
// ─────────────────────────────────────────────────────────────────────────────
class StorageNode {
public:
    virtual ~StorageNode() {}

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool is_open() const = 0;

    // Store an atom
    virtual void store_atom(Handle h) = 0;

    // Fetch an atom by type and name (for nodes)
    virtual Handle fetch_node(Type t, const std::string& name) = 0;

    // Fetch all atoms of a given type
    virtual HandleSeq fetch_type(Type t) = 0;

    // Store entire AtomSpace
    virtual void store_atomspace(const AtomSpace& as) = 0;

    // Load entire AtomSpace
    virtual void load_atomspace(AtomSpace& as) = 0;

    // Barrier: ensure all pending writes are flushed
    virtual void barrier() = 0;

    virtual std::string name() const = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// MemoryStorageNode — In-memory storage backend (for testing)
// ─────────────────────────────────────────────────────────────────────────────
class MemoryStorageNode : public StorageNode {
public:
    explicit MemoryStorageNode(AtomSpace& source, const std::string& name = "mem")
        : source_(source), name_(name), serializer_(source), open_(false) {}

    bool open() override { open_ = true; return true; }
    void close() override { open_ = false; }
    bool is_open() const override { return open_; }

    void store_atom(Handle h) override {
        if (!open_) return;
        std::lock_guard<std::mutex> lock(mtx_);
        store_[h] = serializer_.serialize_full(h);
    }

    Handle fetch_node(Type t, const std::string& name) override {
        if (!open_) return UNDEFINED_HANDLE;
        std::lock_guard<std::mutex> lock(mtx_);
        // Search stored atoms
        for (auto& kv : store_) {
            const Atom* a = source_.get_atom(kv.first);
            if (a && a->type == t && a->name == name) return kv.first;
        }
        return UNDEFINED_HANDLE;
    }

    HandleSeq fetch_type(Type t) override {
        HandleSeq result;
        if (!open_) return result;
        std::lock_guard<std::mutex> lock(mtx_);
        for (auto& kv : store_) {
            const Atom* a = source_.get_atom(kv.first);
            if (a && a->type == t) result.push_back(kv.first);
        }
        return result;
    }

    void store_atomspace(const AtomSpace& as) override {
        if (!open_) return;
        std::lock_guard<std::mutex> lock(mtx_);
        Serializer ser(as);
        as.foreach_atom([&](const Atom& a) {
            store_[a.handle] = ser.serialize_full(a.handle);
        });
    }

    void load_atomspace(AtomSpace& target) override {
        if (!open_) return;
        std::lock_guard<std::mutex> lock(mtx_);
        for (auto& kv : store_) {
            serializer_.deserialize(kv.second, target);
        }
    }

    void barrier() override {}

    std::string name() const override { return name_; }
    size_t stored_count() const { return store_.size(); }

private:
    AtomSpace& source_;
    std::string name_;
    Serializer serializer_;
    bool open_;
    std::mutex mtx_;
    std::map<Handle, std::string> store_;
};

// ─────────────────────────────────────────────────────────────────────────────
// CogChannel — Simulated network channel for distributed AtomSpace
// ─────────────────────────────────────────────────────────────────────────────
class CogChannel {
public:
    CogChannel() = default;

    // Send a serialized command
    void send(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        outbox_.push(msg);
    }

    // Receive a message (blocking simulation)
    bool receive(std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (inbox_.empty()) return false;
        msg = inbox_.front();
        inbox_.pop();
        return true;
    }

    // Simulate delivery: move outbox to peer's inbox
    void deliver_to(CogChannel& peer) {
        std::lock_guard<std::mutex> lock(mtx_);
        while (!outbox_.empty()) {
            peer.inject(outbox_.front());
            outbox_.pop();
        }
    }

    void inject(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        inbox_.push(msg);
    }

    size_t outbox_size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return outbox_.size();
    }

    size_t inbox_size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return inbox_.size();
    }

private:
    mutable std::mutex mtx_;
    std::queue<std::string> outbox_;
    std::queue<std::string> inbox_;
};

// ─────────────────────────────────────────────────────────────────────────────
// ProxyNode — Proxy to a remote AtomSpace via CogChannel
// ─────────────────────────────────────────────────────────────────────────────
class ProxyNode : public StorageNode {
public:
    ProxyNode(AtomSpace& local, CogChannel& channel,
              const std::string& remote_name = "remote")
        : local_(local), channel_(channel), remote_name_(remote_name),
          serializer_(local), open_(false) {}

    bool open() override { open_ = true; return true; }
    void close() override { open_ = false; }
    bool is_open() const override { return open_; }

    void store_atom(Handle h) override {
        if (!open_) return;
        std::string sexpr = serializer_.serialize_full(h);
        channel_.send("STORE " + sexpr);
    }

    Handle fetch_node(Type t, const std::string& name) override {
        if (!open_) return UNDEFINED_HANDLE;
        std::string type_name = TypeRegistry::instance().get_name(t);
        channel_.send("FETCH (" + type_name + " \"" + name + "\")");
        // In a real implementation, this would block waiting for response
        return UNDEFINED_HANDLE;
    }

    HandleSeq fetch_type(Type t) override {
        if (!open_) return {};
        std::string type_name = TypeRegistry::instance().get_name(t);
        channel_.send("FETCH-TYPE " + type_name);
        return {};
    }

    void store_atomspace(const AtomSpace& as) override {
        if (!open_) return;
        Serializer ser(as);
        as.foreach_atom([&](const Atom& a) {
            channel_.send("STORE " + ser.serialize_full(a.handle));
        });
    }

    void load_atomspace(AtomSpace& target) override {
        if (!open_) return;
        channel_.send("LOAD-ALL");
        // Process incoming messages
        std::string msg;
        while (channel_.receive(msg)) {
            if (msg.substr(0, 5) == "ATOM ") {
                serializer_.deserialize(msg.substr(5), target);
            }
        }
    }

    void barrier() override {
        channel_.send("BARRIER");
    }

    std::string name() const override { return remote_name_; }

private:
    AtomSpace& local_;
    CogChannel& channel_;
    std::string remote_name_;
    Serializer serializer_;
    bool open_;
};

// ─────────────────────────────────────────────────────────────────────────────
// FrameServer — Serve an AtomSpace over a CogChannel
// ─────────────────────────────────────────────────────────────────────────────
class FrameServer {
public:
    FrameServer(AtomSpace& as, CogChannel& channel)
        : as_(as), channel_(channel), serializer_(as) {}

    // Process one incoming command
    bool process_one() {
        std::string msg;
        if (!channel_.receive(msg)) return false;

        if (msg.substr(0, 6) == "STORE ") {
            std::string sexpr = msg.substr(6);
            serializer_.deserialize(sexpr, as_);
        } else if (msg.substr(0, 6) == "FETCH ") {
            std::string sexpr = msg.substr(6);
            // Parse and look up
            Handle h = serializer_.deserialize(sexpr, as_);
            if (h != UNDEFINED_HANDLE) {
                channel_.send("ATOM " + serializer_.serialize_full(h));
            }
        } else if (msg == "LOAD-ALL") {
            as_.foreach_atom([&](const Atom& a) {
                channel_.send("ATOM " + serializer_.serialize_full(a.handle));
            });
        } else if (msg == "BARRIER") {
            // No-op for in-memory
        }
        return true;
    }

    // Process all pending commands
    size_t process_all() {
        size_t count = 0;
        while (process_one()) count++;
        return count;
    }

private:
    AtomSpace& as_;
    CogChannel& channel_;
    Serializer serializer_;
};

// ─────────────────────────────────────────────────────────────────────────────
// DistributedAtomSpace — Multi-node AtomSpace with replication
// ─────────────────────────────────────────────────────────────────────────────
class DistributedAtomSpace {
public:
    DistributedAtomSpace() = default;

    // Add a peer AtomSpace
    void add_peer(const std::string& name, AtomSpace& peer_as) {
        auto ch = std::make_shared<CogChannel>();
        auto srv = std::make_shared<FrameServer>(peer_as, *ch);
        peers_.push_back({name, &peer_as, ch, srv});
    }

    // Replicate an atom to all peers
    void replicate(Handle h, const AtomSpace& source) {
        Serializer ser(source);
        std::string sexpr = ser.serialize_full(h);
        for (auto& peer : peers_) {
            peer.channel->inject("STORE " + sexpr);
            peer.server->process_all();
        }
    }

    // Replicate entire AtomSpace to all peers
    void replicate_all(const AtomSpace& source) {
        Serializer ser(source);
        source.foreach_atom([&](const Atom& a) {
            std::string sexpr = ser.serialize_full(a.handle);
            for (auto& peer : peers_) {
                peer.channel->inject("STORE " + sexpr);
            }
        });
        // Process all
        for (auto& peer : peers_) {
            peer.server->process_all();
        }
    }

    size_t peer_count() const { return peers_.size(); }

private:
    struct Peer {
        std::string name;
        AtomSpace* as;
        std::shared_ptr<CogChannel> channel;
        std::shared_ptr<FrameServer> server;
    };
    std::vector<Peer> peers_;
};

} // namespace persist
} // namespace oc

#endif // OC_PERSIST_HPP
