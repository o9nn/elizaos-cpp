#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Zod schemas for validation

// Raw schema for database rows (before decryption)

// Schema for validating decrypted field values

/**
 * FormsService manages form lifecycle and state.
 * It provides methods to create, update, list, and cancel forms.
 */
class FormsService {
public:
    FormsService(IAgentRuntime runtime);
    static std::future<FormsService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime _runtime);
    void registerDefaultTemplates();
    std::future<void> forcePersist();
    std::future<bool> waitForTables(auto maxAttempts, auto delayMs);
    bool isPersistenceAvailable();
    std::future<Form> createForm(const std::variant<std::string, std::optional<Form>>& templateOrForm, std::optional<std::unordered_map<std::string, std::any>> metadata);
    std::future<FormUpdateResult> updateForm(UUID formId, Memory message);
    Promise< extractFormValues(const std::string& text, const std::vector<FormField>& fields);
    std::string trim();
    std::future<std::vector<Form>> listForms(std::optional<FormStatus> status);
    std::variant<Promise<Form, null>> getForm(UUID formId);
    std::future<bool> cancelForm(UUID formId);
    void registerTemplate(FormTemplate template);
    std::vector<FormTemplate> getTemplates();
    std::future<double> cleanup(double olderThanMs = 24 * 60 * 60 * 1000);
    std::future<void> stop();
    std::future<void> checkDatabaseTables();
    std::future<void> recheckTables();
    std::future<void> persistFormsBatch();
    std::future<void> persistFormsIndividual();
    std::future<void> persistForms();
    std::future<void> restorePersistedForms();
    std::variant<std::string, double, bool> parseFieldValue(const std::string& value, const std::string& type, bool isSecret = false);
     validateFieldValue(const std::any& value, FormField field);
    std::future<void> cleanupOldForms();
};


} // namespace elizaos
