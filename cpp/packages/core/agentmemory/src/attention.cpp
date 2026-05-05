#include "elizaos/attention.hpp"
#include "elizaos/agentmemory.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <set>
#include <random>
#include <sstream>
#include <cctype>
namespace elizaos {

// AttentionBudget Implementation
AttentionBudget::AttentionBudget(double totalBudget) : totalBudget_(totalBudget) {
    allocatedBudget_ = 0.0;
}

bool AttentionBudget::allocateAttention(const UUID& elementId, double amount) {
    std::lock_guard<std::mutex> lock(budgetMutex_);
    
    if (amount <= 0 || allocatedBudget_.load() + amount > totalBudget_) {
        return false;
    }
    
    allocations_[elementId] += amount;
    allocatedBudget_.store(allocatedBudget_.load() + amount);
    return true;
}

bool AttentionBudget::deallocateAttention(const UUID& elementId, double amount) {
    std::lock_guard<std::mutex> lock(budgetMutex_);
    
    if (amount <= 0 || allocations_.find(elementId) == allocations_.end()) {
        return false;
    }
    
    double currentAllocation = allocations_[elementId];
    if (amount > currentAllocation) {
        amount = currentAllocation;
    }
    
    allocations_[elementId] -= amount;
    allocatedBudget_.store(allocatedBudget_.load() - amount);
    
    if (allocations_[elementId] <= 0) {
        allocations_.erase(elementId);
    }
    
    return true;
}

double AttentionBudget::getAvailableBudget() const {
    return totalBudget_ - allocatedBudget_.load();
}

void AttentionBudget::redistributeBudget() {
    std::lock_guard<std::mutex> lock(budgetMutex_);
    
    if (allocations_.empty()) return;
    
    // Calculate total current allocations
    double totalCurrentAllocations = 0.0;
    for (const auto& [id, allocation] : allocations_) {
        totalCurrentAllocations += allocation;
    }
    
    // Redistribute proportionally
    if (totalCurrentAllocations > 0) {
        for (auto& [id, allocation] : allocations_) {
            allocation = (allocation / totalCurrentAllocations) * totalBudget_;
        }
        allocatedBudget_.store(totalBudget_);
    }
}

void AttentionBudget::resetBudget() {
    std::lock_guard<std::mutex> lock(budgetMutex_);
    allocations_.clear();
    allocatedBudget_.store(0.0);
}

std::unordered_map<UUID, double> AttentionBudget::getAllocations() const {
    std::lock_guard<std::mutex> lock(budgetMutex_);
    return allocations_;
}

double AttentionBudget::getAllocation(const UUID& elementId) const {
    std::lock_guard<std::mutex> lock(budgetMutex_);
    auto it = allocations_.find(elementId);
    return it != allocations_.end() ? it->second : 0.0;
}

void AttentionBudget::adjustTotalBudget(double newBudget) {
    std::lock_guard<std::mutex> lock(budgetMutex_);
    if (newBudget <= 0) return;
    
    double scaleFactor = newBudget / totalBudget_;
    totalBudget_ = newBudget;
    
    // Scale existing allocations
    for (auto& [id, allocation] : allocations_) {
        allocation *= scaleFactor;
    }
    allocatedBudget_.store(allocatedBudget_.load() * scaleFactor);
}

// ActivationSpreadingNetwork Implementation
ActivationSpreadingNetwork::ActivationSpreadingNetwork() {}

void ActivationSpreadingNetwork::addNode(const UUID& nodeId, double initialActivation) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    if (nodes_.find(nodeId) == nodes_.end()) {
        auto node = std::make_unique<Node>();
        node->id = nodeId;
        node->activation = std::max(0.0, std::min(initialActivation, maxActivation_));
        nodes_[nodeId] = std::move(node);
    }
}

void ActivationSpreadingNetwork::addEdge(const UUID& fromNode, const UUID& toNode, double weight) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    // Ensure both nodes exist
    if (nodes_.find(fromNode) == nodes_.end()) {
        auto node = std::make_unique<Node>();
        node->id = fromNode;
        node->activation = 0.0;
        nodes_[fromNode] = std::move(node);
    }
    if (nodes_.find(toNode) == nodes_.end()) {
        auto node = std::make_unique<Node>();
        node->id = toNode;
        node->activation = 0.0;
        nodes_[toNode] = std::move(node);
    }
    
    // Add edge
    nodes_[fromNode]->outEdges[toNode] = weight;
    nodes_[toNode]->inEdges[fromNode] = weight;
}

void ActivationSpreadingNetwork::removeNode(const UUID& nodeId) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) return;
    
    // Remove all edges involving this node
    for (const auto& [targetId, weight] : it->second->outEdges) {
        if (nodes_.find(targetId) != nodes_.end()) {
            nodes_[targetId]->inEdges.erase(nodeId);
        }
    }
    
    for (const auto& [sourceId, weight] : it->second->inEdges) {
        if (nodes_.find(sourceId) != nodes_.end()) {
            nodes_[sourceId]->outEdges.erase(nodeId);
        }
    }
    
    nodes_.erase(it);
}

void ActivationSpreadingNetwork::removeEdge(const UUID& fromNode, const UUID& toNode) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    if (nodes_.find(fromNode) != nodes_.end()) {
        nodes_[fromNode]->outEdges.erase(toNode);
    }
    if (nodes_.find(toNode) != nodes_.end()) {
        nodes_[toNode]->inEdges.erase(fromNode);
    }
}

void ActivationSpreadingNetwork::spreadActivation(int iterations) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::unordered_map<UUID, double> newActivations;
        
        // Initialize with current activations
        for (const auto& [nodeId, node] : nodes_) {
            newActivations[nodeId] = node->activation;
        }
        
        // Spread activation
        for (const auto& [nodeId, node] : nodes_) {
            if (node->activation > activationThreshold_) {
                double totalOutWeight = 0.0;
                for (const auto& [targetId, weight] : node->outEdges) {
                    totalOutWeight += weight;
                }
                
                if (totalOutWeight > 0) {
                    double activationToSpread = node->activation * spreadingRate_;
                    
                    for (const auto& [targetId, weight] : node->outEdges) {
                        double proportionalActivation = activationToSpread * (weight / totalOutWeight);
                        newActivations[targetId] += proportionalActivation;
                    }
                    
                    // Decay source activation
                    newActivations[nodeId] *= (1.0 - spreadingRate_);
                }
            }
        }
        
        // Apply new activations with bounds checking
        for (auto& [nodeId, node] : nodes_) {
            node->activation = std::max(0.0, std::min(newActivations[nodeId], maxActivation_));
        }
    }
}

void ActivationSpreadingNetwork::injectActivation(const UUID& nodeId, double activation) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    if (nodes_.find(nodeId) == nodes_.end()) {
        auto node = std::make_unique<Node>();
        node->id = nodeId;
        node->activation = 0.0;
        nodes_[nodeId] = std::move(node);
    }
    
    nodes_[nodeId]->activation = std::max(0.0, std::min(
        nodes_[nodeId]->activation + activation, maxActivation_));
}

double ActivationSpreadingNetwork::getActivation(const UUID& nodeId) const {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    auto it = nodes_.find(nodeId);
    return it != nodes_.end() ? it->second->activation : 0.0;
}

std::vector<UUID> ActivationSpreadingNetwork::getTopActivatedNodes(size_t count) const {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    std::vector<std::pair<UUID, double>> nodeActivations;
    for (const auto& [nodeId, node] : nodes_) {
        nodeActivations.emplace_back(nodeId, node->activation);
    }
    
    std::sort(nodeActivations.begin(), nodeActivations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<UUID> result;
    for (size_t i = 0; i < std::min(count, nodeActivations.size()); ++i) {
        result.push_back(nodeActivations[i].first);
    }
    
    return result;
}

std::vector<UUID> ActivationSpreadingNetwork::getNeighbors(const UUID& nodeId) const {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    std::vector<UUID> neighbors;
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        for (const auto& [targetId, weight] : it->second->outEdges) {
            neighbors.push_back(targetId);
        }
    }
    
    return neighbors;
}

double ActivationSpreadingNetwork::getEdgeWeight(const UUID& fromNode, const UUID& toNode) const {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    auto it = nodes_.find(fromNode);
    if (it != nodes_.end()) {
        auto edgeIt = it->second->outEdges.find(toNode);
        if (edgeIt != it->second->outEdges.end()) {
            return edgeIt->second;
        }
    }
    
    return 0.0;
}

void ActivationSpreadingNetwork::decayActivations(double decayRate) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    for (auto& [nodeId, node] : nodes_) {
        node->activation *= decayRate;
        if (node->activation < activationThreshold_) {
            node->activation = 0.0;
        }
    }
}

void ActivationSpreadingNetwork::normalizeActivations() {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    double maxCurrentActivation = 0.0;
    for (const auto& [nodeId, node] : nodes_) {
        maxCurrentActivation = std::max(maxCurrentActivation, node->activation);
    }
    
    if (maxCurrentActivation > 0) {
        double scaleFactor = maxActivation_ / maxCurrentActivation;
        for (auto& [nodeId, node] : nodes_) {
            node->activation *= scaleFactor;
        }
    }
}

void ActivationSpreadingNetwork::clear() {
    std::lock_guard<std::mutex> lock(networkMutex_);
    nodes_.clear();
}

// AttentionAllocator Implementation
AttentionAllocator::AttentionAllocator(double initialBudget) {
    budget_ = std::make_unique<AttentionBudget>(initialBudget);
    spreadingNetwork_ = std::make_unique<ActivationSpreadingNetwork>();
}

void AttentionAllocator::updateAttentionValue(const UUID& elementId, const AttentionValue& value) {
    std::lock_guard<std::mutex> lock(attentionMutex_);
    
    attentionValues_[elementId] = value;
    
    // Update spreading network if enabled
    if (activationSpreadingEnabled_) {
        spreadingNetwork_->addNode(elementId, value.activation);
    }
}

AttentionValue AttentionAllocator::getAttentionValue(const UUID& elementId) const {
    std::lock_guard<std::mutex> lock(attentionMutex_);
    
    auto it = attentionValues_.find(elementId);
    return it != attentionValues_.end() ? it->second : AttentionValue{};
}

bool AttentionAllocator::hasAttentionValue(const UUID& elementId) const {
    std::lock_guard<std::mutex> lock(attentionMutex_);
    return attentionValues_.find(elementId) != attentionValues_.end();
}

std::vector<UUID> AttentionAllocator::allocateAttention(const std::vector<UUID>& candidates, size_t maxAllocations) {
    std::lock_guard<std::mutex> lock(attentionMutex_);
    
    auto priorityItems = prioritizeElements(candidates);
    
    std::vector<UUID> allocated;
    double availableBudget = budget_->getAvailableBudget();
    
    for (const auto& item : priorityItems) {
        if (allocated.size() >= maxAllocations) break;
        
        double requiredBudget = item.priority * 10.0; // Scale priority to budget units
        if (requiredBudget <= availableBudget) {
            if (budget_->allocateAttention(item.elementId, requiredBudget)) {
                allocated.push_back(item.elementId);
                availableBudget -= requiredBudget;
            }
        }
    }
    
    return allocated;
}

std::vector<AttentionPriorityItem> AttentionAllocator::prioritizeElements(const std::vector<UUID>& elements) {
    std::vector<AttentionPriorityItem> items;
    
    for (const auto& elementId : elements) {
        AttentionPriorityItem item;
        item.elementId = elementId;
        
        auto it = attentionValues_.find(elementId);
        if (it != attentionValues_.end()) {
            item.attentionValue = it->second;
            item.priority = it->second.getCompositeScore();
        } else {
            item.priority = 0.0;
        }
        
        items.push_back(item);
    }
    
    // Sort by priority (highest first)
    std::sort(items.begin(), items.end(), 
              [](const AttentionPriorityItem& a, const AttentionPriorityItem& b) {
                  return a.priority > b.priority;
              });
    
    return items;
}

std::vector<std::shared_ptr<Memory>> AttentionAllocator::prioritizeMemories(const std::vector<std::shared_ptr<Memory>>& memories) {
    std::vector<std::pair<std::shared_ptr<Memory>, double>> memoryPriorities;
    
    for (const auto& memory : memories) {
        double priority = 0.0;
        
        auto it = attentionValues_.find(memory->getId());
        if (it != attentionValues_.end()) {
            priority = it->second.getCompositeScore();
        } else {
            // Calculate priority based on memory properties
            AttentionValue value;
            value.importance = calculateImportance(memory->getContent(), {});
            value.urgency = calculateUrgency(memory->getCreatedAt(), {});
            value.novelty = calculateNovelty(memory->getContent(), {});
            priority = value.getCompositeScore();
        }
        
        memoryPriorities.emplace_back(memory, priority);
    }
    
    // Sort by priority (highest first)
    std::sort(memoryPriorities.begin(), memoryPriorities.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    std::vector<std::shared_ptr<Memory>> result;
    for (const auto& [key, val] : memoryPriorities) {
        result.push_back(key);
    }
    
    return result;
}

void AttentionAllocator::updateMemoryAttention(std::shared_ptr<Memory> memory, const AttentionValue& value) {
    updateAttentionValue(memory->getId(), value);
}

double AttentionAllocator::calculateNovelty(const std::string& content, const std::vector<std::string>& context) {
    (void)context; // Suppress unused warning
    std::lock_guard<std::mutex> lock(noveltyMutex_);
    
    auto features = extractFeatures(content);
    double noveltyScore = 0.0;
    
    for (const auto& feature : features) {
        auto it = noveltyModel_.find(feature);
        if (it == noveltyModel_.end()) {
            noveltyScore += 1.0; // Completely novel feature
            noveltyModel_[feature] = 1.0;
        } else {
            double frequency = it->second;
            noveltyScore += std::exp(-frequency / 10.0); // Decreasing novelty with frequency
            noveltyModel_[feature] = frequency + 1.0;
        }
    }
    
    return std::min(1.0, noveltyScore / features.size());
}

void AttentionAllocator::updateNoveltyModel(const std::string& content) {
    std::lock_guard<std::mutex> lock(noveltyMutex_);
    
    auto features = extractFeatures(content);
    for (const auto& feature : features) {
        noveltyModel_[feature] += 1.0;
    }
}

void AttentionAllocator::spreadActivation(int iterations) {
    if (activationSpreadingEnabled_) {
        spreadingNetwork_->spreadActivation(iterations);
        
        // Update attention values with new activations
        std::lock_guard<std::mutex> lock(attentionMutex_);
        for (auto& [elementId, value] : attentionValues_) {
            value.activation = spreadingNetwork_->getActivation(elementId);
        }
    }
}

void AttentionAllocator::addAttentionLink(const UUID& fromElement, const UUID& toElement, double weight) {
    if (activationSpreadingEnabled_) {
        spreadingNetwork_->addEdge(fromElement, toElement, weight);
    }
}

void AttentionAllocator::decayAttentionValues(double decayRate) {
    std::lock_guard<std::mutex> lock(attentionMutex_);
    
    for (auto& [elementId, value] : attentionValues_) {
        value.decayUrgency(urgencyDecayRate_ * decayRate);
        value.decayNovelty(noveltyDecayRate_ * decayRate);
        value.decayActivation(activationDecayRate_ * decayRate);
    }
    
    if (activationSpreadingEnabled_) {
        spreadingNetwork_->decayActivations(decayRate);
    }
}

void AttentionAllocator::periodicMaintenance() {
    decayAttentionValues();
    budget_->redistributeBudget();
    
    if (activationSpreadingEnabled_) {
        spreadingNetwork_->normalizeActivations();
    }
}

AttentionAllocator::AttentionStatistics AttentionAllocator::getStatistics() const {
    std::lock_guard<std::mutex> lock(attentionMutex_);
    
    AttentionStatistics stats;
    stats.totalElements = attentionValues_.size();
    stats.budgetUtilization = budget_->getAllocatedBudget() / budget_->getTotalBudget();
    
    if (attentionValues_.empty()) {
        stats.averageAttention = 0.0;
        stats.maxAttention = 0.0;
        stats.minAttention = 0.0;
        stats.activeElements = 0;
        return stats;
    }
    
    double totalAttention = 0.0;
    double maxAttention = 0.0;
    double minAttention = 1.0;
    size_t activeElements = 0;
    
    for (const auto& [elementId, value] : attentionValues_) {
        double compositeScore = value.getCompositeScore();
        totalAttention += compositeScore;
        maxAttention = std::max(maxAttention, compositeScore);
        minAttention = std::min(minAttention, compositeScore);
        
        if (compositeScore > 0.01) { // Active threshold
            activeElements++;
        }
    }
    
    stats.averageAttention = totalAttention / attentionValues_.size();
    stats.maxAttention = maxAttention;
    stats.minAttention = minAttention;
    stats.activeElements = activeElements;
    
    return stats;
}

std::vector<UUID> AttentionAllocator::getTopAttentionElements(size_t count) const {
    std::lock_guard<std::mutex> lock(attentionMutex_);
    
    std::vector<std::pair<UUID, double>> elementScores;
    for (const auto& [elementId, value] : attentionValues_) {
        elementScores.emplace_back(elementId, value.getCompositeScore());
    }
    
    std::sort(elementScores.begin(), elementScores.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    std::vector<UUID> result;
    for (size_t i = 0; i < std::min(count, elementScores.size()); ++i) {
        result.push_back(elementScores[i].first);
    }
    
    return result;
}

void AttentionAllocator::setDecayRates(double urgencyDecay, double noveltyDecay, double activationDecay) {
    urgencyDecayRate_ = urgencyDecay;
    noveltyDecayRate_ = noveltyDecay;
    activationDecayRate_ = activationDecay;
}

void AttentionAllocator::setBudgetSize(double newBudget) {
    budget_->adjustTotalBudget(newBudget);
}

void AttentionAllocator::setSpreadingParameters(double spreadingRate, double threshold) {
    // Store parameters for std::future use
    (void)spreadingRate; // Suppress unused warning
    (void)threshold;     // Suppress unused warning
    // Implementation depends on ActivationSpreadingNetwork having setters
    // For now, we'll store these and use them when creating new networks
}

// Helper methods implementation
double AttentionAllocator::calculateImportance(const std::string& content, const std::vector<std::string>& context) {
    (void)context; // Suppress unused warning for now
    
    // Simple importance calculation based on content length and keywords
    double importance = 0.0;
    
    // Length factor
    importance += std::min(1.0, content.length() / 1000.0) * 0.3;
    
    // Keyword matching (simple approach)
    std::vector<std::string> importantKeywords = {"urgent", "important", "critical", "priority", "deadline"};
    std::transform(importantKeywords.begin(), importantKeywords.end(), importantKeywords.begin(),
                   [](std::string& s) { std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); }); return s; });
    
    std::string lowerContent = content;
    std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    
    for (const auto& keyword : importantKeywords) {
        if (lowerContent.find(keyword) != std::string::npos) {
            importance += 0.2;
        }
    }
    
    return std::min(1.0, importance);
}

double AttentionAllocator::calculateUrgency(const Timestamp& timestamp, const std::vector<std::string>& tags) {
    // Calculate urgency based on recency and tags
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::hours>(now - timestamp).count();
    
    // Recent items are more urgent
    double urgency = std::exp(-age / 24.0); // Decay over days
    
    // Tag-based urgency boost
    for (const auto& tag : tags) {
        std::string lowerTag = tag;
        std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        
        if (lowerTag.find("urgent") != std::string::npos || 
            lowerTag.find("immediate") != std::string::npos ||
            lowerTag.find("asap") != std::string::npos) {
            urgency += 0.5;
        }
    }
    
    return std::min(1.0, urgency);
}

void AttentionAllocator::normalizeAttentionValues() {
    // Normalize attention values to maintain coherent scaling
    if (attentionValues_.empty()) return;
    
    double maxComposite = 0.0;
    for (const auto& [elementId, value] : attentionValues_) {
        maxComposite = std::max(maxComposite, value.getCompositeScore());
    }
    
    if (maxComposite > 1.0) {
        for (auto& [elementId, value] : attentionValues_) {
            value.importance /= maxComposite;
            value.urgency /= maxComposite;
            value.novelty /= maxComposite;
            value.activation /= maxComposite;
        }
    }
}

std::vector<std::string> AttentionAllocator::extractFeatures(const std::string& content) {
    // Simple feature extraction - split by spaces and punctuation
    std::vector<std::string> features;
    std::stringstream ss(content);
    std::string word;
    
    while (ss >> word) {
        // Remove punctuation and convert to lowercase
        std::string cleanWord;
        for (char c : word) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                cleanWord += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
        }
        
        if (!cleanWord.empty() && cleanWord.length() > 2) {
            features.push_back(cleanWord);
        }
    }
    
    return features;
}

// AttentionAwareMemoryManager Implementation
AttentionAwareMemoryManager::AttentionAwareMemoryManager() {
    attentionAllocator_ = std::make_shared<AttentionAllocator>();
}

std::vector<std::shared_ptr<Memory>> AttentionAwareMemoryManager::getMemoriesWithAttention(
    const MemorySearchParams& params, 
    size_t maxResults) {
    
    if (!memoryManager_) {
        return {};
    }
    
    // Get memories using standard search
    auto memories = memoryManager_->getMemories(params);
    
    // Prioritize by attention
    auto prioritizedMemories = attentionAllocator_->prioritizeMemories(memories);
    
    // Return top results
    if (prioritizedMemories.size() > maxResults) {
        prioritizedMemories.resize(maxResults);
    }
    
    return prioritizedMemories;
}

std::vector<std::shared_ptr<Memory>> AttentionAwareMemoryManager::searchMemoriesByAttention(
    const MemorySearchByEmbeddingParams& params,
    double minAttentionScore) {
    
    if (!memoryManager_) {
        return {};
    }
    
    // Get memories using embedding search
    auto memories = memoryManager_->searchMemories(params);
    
    // Filter by attention score
    std::vector<std::shared_ptr<Memory>> filteredMemories;
    for (const auto& memory : memories) {
        auto attentionValue = attentionAllocator_->getAttentionValue(memory->getId());
        if (attentionValue.getCompositeScore() >= minAttentionScore) {
            filteredMemories.push_back(memory);
        }
    }
    
    // Prioritize by attention
    return attentionAllocator_->prioritizeMemories(filteredMemories);
}

void AttentionAwareMemoryManager::updateMemoryAttention(const UUID& memoryId, const AttentionValue& value) {
    std::lock_guard<std::mutex> lock(memoryAttentionMutex_);
    
    memoryAttentionMap_[memoryId] = value;
    attentionAllocator_->updateAttentionValue(memoryId, value);
}

void AttentionAwareMemoryManager::refreshMemoryAttention(const UUID& memoryId) {
    if (!memoryManager_) return;
    
    auto memory = memoryManager_->getMemoryById(memoryId);
    if (!memory) return;
    
    // Calculate new attention values
    AttentionValue value;
    value.importance = calculateMemoryImportance(memory);
    value.urgency = calculateMemoryUrgency(memory);
    value.novelty = calculateMemoryNovelty(memory);
    value.activation = 0.1; // Default activation
    
    updateMemoryAttention(memoryId, value);
}

void AttentionAwareMemoryManager::consolidateMemories() {
    // Real consolidation: merge near-duplicate high-attention memories within
    // each room. Two memories are considered duplicates when their content is
    // an exact match OR they share more than 80% of unique tokens AND both
    // exceed a minimum composite attention score. The surviving memory keeps
    // the higher composite-score's metadata and accumulates the activation of
    // the merged memory before the duplicate is forgotten.
    if (!memoryManager_) return;

    constexpr double kMinScoreForConsolidation = 0.25;
    constexpr double kTokenOverlapThreshold = 0.80;

    // Snapshot of attention map (avoid holding lock across memory mutations)
    std::vector<std::pair<UUID, AttentionValue>> snapshot;
    {
        std::lock_guard<std::mutex> lock(memoryAttentionMutex_);
        snapshot.reserve(memoryAttentionMap_.size());
        for (const auto& kv : memoryAttentionMap_) {
            if (kv.second.getCompositeScore() >= kMinScoreForConsolidation) {
                snapshot.push_back(kv);
            }
        }
    }

    // Tokenizer used for overlap measurement (whitespace based, lowercased).
    auto tokenize = [](const std::string& s) {
        std::vector<std::string> tokens;
        std::string cur;
        for (char ch : s) {
            if (std::isspace(static_cast<unsigned char>(ch)) ||
                std::ispunct(static_cast<unsigned char>(ch))) {
                if (!cur.empty()) { tokens.push_back(cur); cur.clear(); }
            } else {
                cur.push_back(static_cast<char>(std::tolower(
                    static_cast<unsigned char>(ch))));
            }
        }
        if (!cur.empty()) tokens.push_back(cur);
        // Deduplicate (set semantics for Jaccard).
        std::sort(tokens.begin(), tokens.end());
        tokens.erase(std::unique(tokens.begin(), tokens.end()), tokens.end());
        return tokens;
    };

    auto jaccard = [](const std::vector<std::string>& a,
                      const std::vector<std::string>& b) {
        if (a.empty() && b.empty()) return 1.0;
        size_t intersection = 0;
        size_t i = 0, j = 0;
        while (i < a.size() && j < b.size()) {
            if (a[i] == b[j]) { ++intersection; ++i; ++j; }
            else if (a[i] < b[j]) ++i; else ++j;
        }
        size_t unionSize = a.size() + b.size() - intersection;
        return unionSize == 0 ? 0.0 : static_cast<double>(intersection) / unionSize;
    };

    std::vector<UUID> toForget;

    for (size_t i = 0; i < snapshot.size(); ++i) {
        auto memA = memoryManager_->getMemoryById(snapshot[i].first);
        if (!memA) continue;
        auto tokensA = tokenize(memA->getContent());
        for (size_t j = i + 1; j < snapshot.size(); ++j) {
            auto memB = memoryManager_->getMemoryById(snapshot[j].first);
            if (!memB) continue;
            // Only consolidate within the same room (semantic locality)
            if (memA->getRoomId() != memB->getRoomId()) continue;
            auto tokensB = tokenize(memB->getContent());
            const bool exact = memA->getContent() == memB->getContent();
            const double overlap = exact ? 1.0 : jaccard(tokensA, tokensB);
            if (overlap < kTokenOverlapThreshold) continue;

            // Choose the keeper as the higher composite score memory.
            const auto& vA = snapshot[i].second;
            const auto& vB = snapshot[j].second;
            const bool keepA = vA.getCompositeScore() >= vB.getCompositeScore();
            const UUID& keeperId = keepA ? snapshot[i].first : snapshot[j].first;
            const UUID& dropId   = keepA ? snapshot[j].first : snapshot[i].first;

            // Sum activations onto the keeper to preserve cumulative salience.
            AttentionValue merged = keepA ? vA : vB;
            merged.activation = std::min(
                1.0, vA.activation + vB.activation);
            merged.urgency = std::max(vA.urgency, vB.urgency);
            merged.novelty = std::max(vA.novelty, vB.novelty);
            updateMemoryAttention(keeperId, merged);
            toForget.push_back(dropId);
        }
    }

    // Remove the consolidated duplicates.
    for (const auto& id : toForget) {
        memoryManager_->deleteMemory(id);
        std::lock_guard<std::mutex> lock(memoryAttentionMutex_);
        memoryAttentionMap_.erase(id);
    }
}

void AttentionAwareMemoryManager::forgetLowAttentionMemories(double threshold) {
    if (!memoryManager_) return;
    
    std::vector<UUID> memoriesToForget;
    
    {
        std::lock_guard<std::mutex> lock(memoryAttentionMutex_);
        for (const auto& [memoryId, attentionValue] : memoryAttentionMap_) {
            if (attentionValue.getCompositeScore() < threshold) {
                memoriesToForget.push_back(memoryId);
            }
        }
    }
    
    // Remove low attention memories
    for (const auto& memoryId : memoriesToForget) {
        memoryManager_->deleteMemory(memoryId);
        std::lock_guard<std::mutex> lock(memoryAttentionMutex_);
        memoryAttentionMap_.erase(memoryId);
    }
}

void AttentionAwareMemoryManager::setMemoryManager(std::shared_ptr<AgentMemoryManager> memoryManager) {
    memoryManager_ = memoryManager;
}

std::shared_ptr<AgentMemoryManager> AttentionAwareMemoryManager::getMemoryManager() const {
    return memoryManager_;
}

std::shared_ptr<AttentionAllocator> AttentionAwareMemoryManager::getAttentionAllocator() const {
    return attentionAllocator_;
}

// Helper methods
void AttentionAwareMemoryManager::updateMemoryLinks(std::shared_ptr<Memory> memory) {
    // Build attention links between this memory and other memories that share
    // hypergraph nodes/edges or that live in the same room. Link weight is the
    // proportion of shared hypergraph node IDs (Dice coefficient). This wires
    // the activation-spreading network so spreadActivation() can propagate
    // salience along semantically related memories.
    if (!memory || !memoryManager_ || !attentionAllocator_) return;

    const auto& targetNodes = memory->getHypergraphNodes();
    const auto& targetEdges = memory->getHypergraphEdges();
    if (targetNodes.empty() && targetEdges.empty()) return;

    // Snapshot the IDs we have attention values for so we don't hold the
    // attention mutex across additional memory lookups.
    std::vector<UUID> candidateIds;
    {
        std::lock_guard<std::mutex> lock(memoryAttentionMutex_);
        candidateIds.reserve(memoryAttentionMap_.size());
        for (const auto& kv : memoryAttentionMap_) {
            if (kv.first != memory->getId()) candidateIds.push_back(kv.first);
        }
    }

    auto sharedCount = [](const std::vector<UUID>& a,
                          const std::vector<UUID>& b) -> size_t {
        size_t shared = 0;
        for (const auto& x : a) {
            if (std::find(b.begin(), b.end(), x) != b.end()) ++shared;
        }
        return shared;
    };

    for (const auto& otherId : candidateIds) {
        auto other = memoryManager_->getMemoryById(otherId);
        if (!other) continue;

        const size_t nodesShared = sharedCount(targetNodes,
                                               other->getHypergraphNodes());
        const size_t edgesShared = sharedCount(targetEdges,
                                               other->getHypergraphEdges());
        const size_t totalShared = nodesShared + edgesShared;
        const size_t totalUnique = targetNodes.size() + targetEdges.size() +
                                   other->getHypergraphNodes().size() +
                                   other->getHypergraphEdges().size();
        if (totalShared == 0 || totalUnique == 0) continue;

        // Dice similarity in [0,1].
        const double weight = (2.0 * static_cast<double>(totalShared)) /
                              static_cast<double>(totalUnique);
        if (weight <= 0.0) continue;
        attentionAllocator_->addAttentionLink(memory->getId(), otherId, weight);
        attentionAllocator_->addAttentionLink(otherId, memory->getId(), weight);
    }
}

double AttentionAwareMemoryManager::calculateMemoryImportance(std::shared_ptr<Memory> memory) {
    // Calculate importance based on memory content and metadata
    double importance = 0.0;
    
    // Content-based importance
    importance += std::min(1.0, memory->getContent().length() / 1000.0) * 0.3;
    
    // Embedding-based importance (if available)
    if (memory->getEmbedding()) {
        importance += 0.2; // Bonus for having embedding
    }
    
    // Uniqueness factor
    if (memory->isUnique()) {
        importance += 0.3;
    }
    
    return std::min(1.0, importance);
}

double AttentionAwareMemoryManager::calculateMemoryUrgency(std::shared_ptr<Memory> memory) {
    // Calculate urgency based on memory age and other factors
    auto now = std::chrono::system_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::hours>(now - memory->getCreatedAt()).count();
    
    // Recent memories are more urgent
    return std::exp(-age / 24.0); // Decay over days
}

double AttentionAwareMemoryManager::calculateMemoryNovelty(std::shared_ptr<Memory> memory) {
    // Use the attention allocator's novelty calculation
    return attentionAllocator_->calculateNovelty(memory->getContent(), {});
}

// Global attention-aware memory manager instance
AttentionAwareMemoryManager& getGlobalAttentionAwareMemoryManager() {
    static AttentionAwareMemoryManager instance;
    return instance;
}

// Convenience functions for attention-aware memory operations
namespace attention {
    std::vector<std::shared_ptr<Memory>> getTopMemories(size_t count) {
        MemorySearchParams params;
        params.count = static_cast<int>(count);
        return getGlobalAttentionAwareMemoryManager().getMemoriesWithAttention(params, count);
    }
    
    std::vector<std::shared_ptr<Memory>> searchWithAttention(
        const MemorySearchParams& params,
        double minAttentionScore) {
        // Convert to embedding search params (simplified)
        MemorySearchByEmbeddingParams embeddingParams;
        embeddingParams.tableName = params.tableName;
        embeddingParams.roomId = params.roomId;
        embeddingParams.entityId = params.entityId;
        embeddingParams.count = params.count;
        embeddingParams.matchThreshold = minAttentionScore;
        
        return getGlobalAttentionAwareMemoryManager().searchMemoriesByAttention(embeddingParams, minAttentionScore);
    }
    
    void updateAttention(const UUID& elementId, const AttentionValue& value) {
        getGlobalAttentionAwareMemoryManager().getAttentionAllocator()->updateAttentionValue(elementId, value);
    }
    
    AttentionValue getAttention(const UUID& elementId) {
        return getGlobalAttentionAwareMemoryManager().getAttentionAllocator()->getAttentionValue(elementId);
    }
    
    std::vector<UUID> allocateResources(const std::vector<UUID>& candidates, size_t maxAllocations) {
        return getGlobalAttentionAwareMemoryManager().getAttentionAllocator()->allocateAttention(candidates, maxAllocations);
    }
    
    void performMaintenance() {
        getGlobalAttentionAwareMemoryManager().getAttentionAllocator()->periodicMaintenance();
    }
    
    void decayAttentionValues(double decayRate) {
        getGlobalAttentionAwareMemoryManager().getAttentionAllocator()->decayAttentionValues(decayRate);
    }
}

} // namespace elizaos