// oc::util — Header-only C++11 reimplementation of opencog/cogutil
// Logger, RandGen, Config, concurrent_queue, oc_assert, lazy_random_selector, SigSlot
// Zero external dependencies
// SPDX-License-Identifier: AGPL-3.0
#ifndef OC_UTIL_HPP
#define OC_UTIL_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <set>
#include <memory>
#include <functional>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <cassert>
#include <type_traits>
#include <limits>
#include <utility>
#include <numeric>
#include <array>
#include <random>

namespace oc {
namespace util {

// ─────────────────────────────────────────────────────────────────────────────
// oc_assert — OpenCog assertion with message
// ─────────────────────────────────────────────────────────────────────────────
struct AssertionException : public std::runtime_error {
    AssertionException(const std::string& msg) : std::runtime_error(msg) {}
};

inline void oc_assert(bool cond, const char* msg = "assertion failed") {
    if (!cond) throw AssertionException(msg);
}

inline void oc_assert(bool cond, const std::string& msg) {
    if (!cond) throw AssertionException(msg);
}

// ─────────────────────────────────────────────────────────────────────────────
// Logger — Thread-safe hierarchical logger (mirrors opencog::Logger)
// ─────────────────────────────────────────────────────────────────────────────
enum class LogLevel : int {
    NONE  = 0,
    ERROR = 1,
    WARN  = 2,
    INFO  = 3,
    DEBUG = 4,
    FINE  = 5
};

inline const char* log_level_name(LogLevel lv) {
    switch (lv) {
        case LogLevel::NONE:  return "NONE";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::FINE:  return "FINE";
        default:              return "UNKNOWN";
    }
}

class Logger {
public:
    Logger() : level_(LogLevel::INFO), enabled_(true) {}

    void set_level(LogLevel lv) { level_ = lv; }
    LogLevel get_level() const { return level_; }
    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }
    bool is_enabled() const { return enabled_; }

    bool should_log(LogLevel lv) const {
        return enabled_ && static_cast<int>(lv) <= static_cast<int>(level_);
    }

    void log(LogLevel lv, const std::string& msg) {
        if (!should_log(lv)) return;
        std::lock_guard<std::mutex> lock(mtx_);
        entries_.push_back({lv, msg, std::chrono::steady_clock::now()});
        if (print_fn_) print_fn_(lv, msg);
    }

    void error(const std::string& msg) { log(LogLevel::ERROR, msg); }
    void warn(const std::string& msg)  { log(LogLevel::WARN, msg); }
    void info(const std::string& msg)  { log(LogLevel::INFO, msg); }
    void debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
    void fine(const std::string& msg)  { log(LogLevel::FINE, msg); }

    struct Entry {
        LogLevel level;
        std::string message;
        std::chrono::steady_clock::time_point timestamp;
    };

    std::vector<Entry> get_entries() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return entries_;
    }

    size_t entry_count() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return entries_.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mtx_);
        entries_.clear();
    }

    // Set a custom print function (e.g., to stderr or file)
    void set_print_fn(std::function<void(LogLevel, const std::string&)> fn) {
        print_fn_ = fn;
    }

    // Singleton access
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

private:
    LogLevel level_;
    bool enabled_;
    mutable std::mutex mtx_;
    std::vector<Entry> entries_;
    std::function<void(LogLevel, const std::string&)> print_fn_;
};

// ─────────────────────────────────────────────────────────────────────────────
// RandGen — Mersenne Twister random number generator
// ─────────────────────────────────────────────────────────────────────────────
class RandGen {
public:
    explicit RandGen(uint64_t seed = 0) : engine_(seed) {}

    void seed(uint64_t s) { engine_.seed(s); }

    // Uniform int in [lo, hi]
    int randint(int lo, int hi) {
        std::uniform_int_distribution<int> dist(lo, hi);
        return dist(engine_);
    }

    // Uniform float in [0, 1)
    float randfloat() {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(engine_);
    }

    // Uniform double in [0, 1)
    double randdouble() {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(engine_);
    }

    // Gaussian
    double gauss(double mean, double stddev) {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(engine_);
    }

    // Random bool with probability p
    bool randbool(double p = 0.5) { return randdouble() < p; }

    // Pick random element from vector
    template<typename T>
    const T& pick(const std::vector<T>& v) {
        oc_assert(!v.empty(), "pick from empty vector");
        return v[randint(0, static_cast<int>(v.size()) - 1)];
    }

    std::mt19937_64& engine() { return engine_; }

    static RandGen& instance() {
        static RandGen rng(42);
        return rng;
    }

private:
    std::mt19937_64 engine_;
};

// ─────────────────────────────────────────────────────────────────────────────
// lazy_random_selector — Select k unique random indices from [0, n)
// ─────────────────────────────────────────────────────────────────────────────
class lazy_random_selector {
public:
    lazy_random_selector(size_t n, RandGen& rng = RandGen::instance())
        : n_(n), count_(0), rng_(rng) {
        mapping_.reserve(n);
    }

    bool empty() const { return count_ >= n_; }
    size_t remaining() const { return n_ - count_; }

    size_t select() {
        oc_assert(!empty(), "lazy_random_selector exhausted");
        size_t idx = rng_.randint(static_cast<int>(count_),
                                   static_cast<int>(n_) - 1);
        // Fisher-Yates style lazy swap
        auto it_idx = mapping_.find(idx);
        auto it_cnt = mapping_.find(count_);
        size_t val_idx = (it_idx != mapping_.end()) ? it_idx->second : idx;
        size_t val_cnt = (it_cnt != mapping_.end()) ? it_cnt->second : count_;
        mapping_[idx] = val_cnt;
        mapping_[count_] = val_idx;
        count_++;
        return val_idx;
    }

    void reset() { count_ = 0; mapping_.clear(); }

private:
    size_t n_;
    size_t count_;
    RandGen& rng_;
    std::unordered_map<size_t, size_t> mapping_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Config — Key-value configuration store
// ─────────────────────────────────────────────────────────────────────────────
class Config {
public:
    Config() = default;

    void set(const std::string& key, const std::string& value) {
        store_[key] = value;
    }

    std::string get(const std::string& key,
                    const std::string& default_val = "") const {
        auto it = store_.find(key);
        return (it != store_.end()) ? it->second : default_val;
    }

    int get_int(const std::string& key, int default_val = 0) const {
        auto it = store_.find(key);
        if (it == store_.end()) return default_val;
        try { return std::stoi(it->second); }
        catch (...) { return default_val; }
    }

    double get_double(const std::string& key, double default_val = 0.0) const {
        auto it = store_.find(key);
        if (it == store_.end()) return default_val;
        try { return std::stod(it->second); }
        catch (...) { return default_val; }
    }

    bool get_bool(const std::string& key, bool default_val = false) const {
        auto it = store_.find(key);
        if (it == store_.end()) return default_val;
        return (it->second == "true" || it->second == "1" ||
                it->second == "yes" || it->second == "TRUE");
    }

    bool has(const std::string& key) const {
        return store_.find(key) != store_.end();
    }

    void remove(const std::string& key) { store_.erase(key); }
    void clear() { store_.clear(); }
    size_t size() const { return store_.size(); }

    // Parse "key = value" lines
    void load_from_string(const std::string& text) {
        std::istringstream iss(text);
        std::string line;
        while (std::getline(iss, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = trim(line.substr(0, eq));
            std::string val = trim(line.substr(eq + 1));
            if (!key.empty()) store_[key] = val;
        }
    }

    std::string dump() const {
        std::ostringstream os;
        for (auto& kv : store_) {
            os << kv.first << " = " << kv.second << "\n";
        }
        return os.str();
    }

    static Config& instance() {
        static Config cfg;
        return cfg;
    }

private:
    std::map<std::string, std::string> store_;

    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// concurrent_queue — Thread-safe unbounded MPMC queue
// ─────────────────────────────────────────────────────────────────────────────
struct CanceledException : public std::runtime_error {
    CanceledException() : std::runtime_error("queue canceled") {}
};

template<typename T>
class concurrent_queue {
public:
    concurrent_queue() : canceled_(false) {}

    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (canceled_) throw CanceledException();
        queue_.push(item);
        cv_.notify_one();
    }

    void push(T&& item) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (canceled_) throw CanceledException();
        queue_.push(std::move(item));
        cv_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]{ return !queue_.empty() || canceled_; });
        if (canceled_ && queue_.empty()) throw CanceledException();
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) return false;
        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();
    }

    void cancel() {
        std::lock_guard<std::mutex> lock(mtx_);
        canceled_ = true;
        cv_.notify_all();
    }

    bool is_canceled() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return canceled_;
    }

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<T> queue_;
    bool canceled_;
};

// ─────────────────────────────────────────────────────────────────────────────
// concurrent_set — Thread-safe de-duplicating container
// ─────────────────────────────────────────────────────────────────────────────
template<typename T, typename Compare = std::less<T>>
class concurrent_set {
public:
    bool insert(const T& item) {
        std::lock_guard<std::mutex> lock(mtx_);
        return set_.insert(item).second;
    }

    bool erase(const T& item) {
        std::lock_guard<std::mutex> lock(mtx_);
        return set_.erase(item) > 0;
    }

    bool contains(const T& item) const {
        std::lock_guard<std::mutex> lock(mtx_);
        return set_.find(item) != set_.end();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return set_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return set_.empty();
    }

    std::vector<T> to_vector() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return std::vector<T>(set_.begin(), set_.end());
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mtx_);
        set_.clear();
    }

private:
    mutable std::mutex mtx_;
    std::set<T, Compare> set_;
};

// ─────────────────────────────────────────────────────────────────────────────
// SigSlot — Lightweight thread-safe signal/slot
// ─────────────────────────────────────────────────────────────────────────────
template<typename... Args>
class SigSlot {
public:
    using slot_type = std::function<void(Args...)>;
    using slot_id = uint64_t;

    SigSlot() : next_id_(1) {}

    slot_id connect(slot_type fn) {
        std::lock_guard<std::mutex> lock(mtx_);
        slot_id id = next_id_++;
        slots_[id] = fn;
        return id;
    }

    void disconnect(slot_id id) {
        std::lock_guard<std::mutex> lock(mtx_);
        slots_.erase(id);
    }

    void emit(Args... args) const {
        std::map<slot_id, slot_type> copy;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            copy = slots_;
        }
        for (auto& kv : copy) {
            kv.second(args...);
        }
    }

    void operator()(Args... args) const { emit(args...); }

    size_t num_slots() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return slots_.size();
    }

    void disconnect_all() {
        std::lock_guard<std::mutex> lock(mtx_);
        slots_.clear();
    }

private:
    mutable std::mutex mtx_;
    slot_id next_id_;
    std::map<slot_id, slot_type> slots_;
};

// ─────────────────────────────────────────────────────────────────────────────
// Counter — Thread-safe atomic counter
// ─────────────────────────────────────────────────────────────────────────────
class Counter {
public:
    Counter() : val_(0) {}
    explicit Counter(int64_t v) : val_(v) {}
    int64_t increment() { return ++val_; }
    int64_t decrement() { return --val_; }
    int64_t get() const { return val_.load(); }
    void set(int64_t v) { val_.store(v); }
private:
    std::atomic<int64_t> val_;
};

// ─────────────────────────────────────────────────────────────────────────────
// StringTokenizer — Simple string splitting utility
// ─────────────────────────────────────────────────────────────────────────────
inline std::vector<std::string> tokenize(const std::string& str,
                                          const std::string& delim = " \t") {
    std::vector<std::string> tokens;
    size_t start = 0;
    while (start < str.size()) {
        size_t pos = str.find_first_of(delim, start);
        if (pos == std::string::npos) {
            tokens.push_back(str.substr(start));
            break;
        }
        if (pos > start) tokens.push_back(str.substr(start, pos - start));
        start = pos + 1;
    }
    return tokens;
}

} // namespace util
} // namespace oc

#endif // OC_UTIL_HPP
