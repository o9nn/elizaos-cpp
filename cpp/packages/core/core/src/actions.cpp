// Action / Provider registries — canonical promotion of orphan action intent.
//
// Pre-quarantine packages/core/core/src/action(s).cpp were TypeScript residue
// (object::pair / express-style). This translation unit re-implements the
// product surface declared in include/core.hpp as production C++17.

#include "elizaos/core.hpp"

#include <utility>

namespace elizaos {

// ---------------------------------------------------------------------------
// FunctionAction
// ---------------------------------------------------------------------------

FunctionAction::FunctionAction(std::string name, Validator validator, Executor executor)
    : name_(std::move(name)),
      validator_(std::move(validator)),
      executor_(std::move(executor)) {}

std::string FunctionAction::getName() const {
    return name_;
}

bool FunctionAction::validate(const State& state, std::shared_ptr<Memory> message) const {
    if (!validator_) {
        return false;
    }
    return validator_(state, message);
}

bool FunctionAction::execute(State& state, std::shared_ptr<Memory> message) {
    if (!executor_) {
        return false;
    }
    return executor_(state, message);
}

// ---------------------------------------------------------------------------
// ActionRegistry
// ---------------------------------------------------------------------------

bool ActionRegistry::registerAction(std::shared_ptr<Action> action) {
    if (!action) {
        return false;
    }
    const std::string name = action->getName();
    if (name.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    actions_[name] = std::move(action);
    return true;
}

bool ActionRegistry::unregisterAction(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    return actions_.erase(name) > 0;
}

std::shared_ptr<Action> ActionRegistry::get(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = actions_.find(name);
    if (it == actions_.end()) {
        return nullptr;
    }
    return it->second;
}

bool ActionRegistry::contains(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return actions_.find(name) != actions_.end();
}

std::vector<std::string> ActionRegistry::listNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(actions_.size());
    for (const auto& entry : actions_) {
        names.push_back(entry.first);
    }
    return names;
}

size_t ActionRegistry::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return actions_.size();
}

std::vector<std::string> ActionRegistry::matchingActions(
    const State& state, std::shared_ptr<Memory> message) const {
    std::vector<std::shared_ptr<Action>> snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        snapshot.reserve(actions_.size());
        for (const auto& entry : actions_) {
            snapshot.push_back(entry.second);
        }
    }

    std::vector<std::string> matched;
    for (const auto& action : snapshot) {
        if (action && action->validate(state, message)) {
            matched.push_back(action->getName());
        }
    }
    return matched;
}

size_t ActionRegistry::executeMatching(State& state, std::shared_ptr<Memory> message) {
    std::vector<std::shared_ptr<Action>> snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        snapshot.reserve(actions_.size());
        for (const auto& entry : actions_) {
            snapshot.push_back(entry.second);
        }
    }

    size_t succeeded = 0;
    for (const auto& action : snapshot) {
        if (!action) {
            continue;
        }
        if (action->validate(state, message) && action->execute(state, message)) {
            ++succeeded;
        }
    }
    return succeeded;
}

bool ActionRegistry::executeNamed(
    const std::string& name, State& state, std::shared_ptr<Memory> message) {
    auto action = get(name);
    if (!action) {
        return false;
    }
    if (!action->validate(state, message)) {
        return false;
    }
    return action->execute(state, message);
}

void ActionRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    actions_.clear();
}

// ---------------------------------------------------------------------------
// FunctionProvider
// ---------------------------------------------------------------------------

FunctionProvider::FunctionProvider(std::string name, Getter getter)
    : name_(std::move(name)), getter_(std::move(getter)) {}

std::string FunctionProvider::getName() const {
    return name_;
}

std::unordered_map<std::string, std::string> FunctionProvider::get(
    const State& state, std::shared_ptr<Memory> message) {
    if (!getter_) {
        return {};
    }
    return getter_(state, message);
}

// ---------------------------------------------------------------------------
// ProviderRegistry
// ---------------------------------------------------------------------------

bool ProviderRegistry::registerProvider(std::shared_ptr<Provider> provider) {
    if (!provider) {
        return false;
    }
    const std::string name = provider->getName();
    if (name.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    providers_[name] = std::move(provider);
    return true;
}

bool ProviderRegistry::unregisterProvider(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    return providers_.erase(name) > 0;
}

std::shared_ptr<Provider> ProviderRegistry::get(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = providers_.find(name);
    if (it == providers_.end()) {
        return nullptr;
    }
    return it->second;
}

bool ProviderRegistry::contains(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return providers_.find(name) != providers_.end();
}

std::vector<std::string> ProviderRegistry::listNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(providers_.size());
    for (const auto& entry : providers_) {
        names.push_back(entry.first);
    }
    return names;
}

size_t ProviderRegistry::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return providers_.size();
}

std::unordered_map<std::string, std::string> ProviderRegistry::compose(
    const State& state, std::shared_ptr<Memory> message) const {
    std::vector<std::shared_ptr<Provider>> snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        snapshot.reserve(providers_.size());
        for (const auto& entry : providers_) {
            snapshot.push_back(entry.second);
        }
    }

    std::unordered_map<std::string, std::string> merged;
    for (const auto& provider : snapshot) {
        if (!provider) {
            continue;
        }
        auto bag = provider->get(state, message);
        for (auto& kv : bag) {
            merged[std::move(kv.first)] = std::move(kv.second);
        }
    }
    return merged;
}

void ProviderRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    providers_.clear();
}

} // namespace elizaos
