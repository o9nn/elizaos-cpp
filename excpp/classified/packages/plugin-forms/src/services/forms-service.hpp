#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

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
    std::future<FormsService> start(IAgentRuntime runtime);
    std::future<void> initialize(IAgentRuntime _runtime);
    void registerDefaultTemplates();
    std::future<void> forcePersist();
    std::future<bool> waitForTables(auto maxAttempts = 10, auto delayMs = 1000);
    bool isPersistenceAvailable();
    std::future<Form> createForm(const std::variant<std::string, std::optional<Form>>& templateOrForm, std::optional<Record<string> metadata, auto unknown>);
    std::future<FormUpdateResult> updateForm(UUID formId, Memory message);
    Promise< extractFormValues(const std::string& text, const std::vector<FormField>& fields);
    ' trim();
    void if(auto jsonMatch);
    void for(auto const field of fields);
    void if(auto !result.success);
    void catch(auto parseError);
    void catch(auto error);
    std::future<std::vector<Form>> listForms(std::optional<FormStatus> status);
    std::variant<Promise<Form, null>> getForm(UUID formId);
    std::future<bool> cancelForm(UUID formId);
    void registerTemplate(FormTemplate template);
    std::vector<FormTemplate> getTemplates();
    std::future<double> cleanup(number = 24 * 60 * 60 * 1000 olderThanMs);
    std::future<void> stop();
    std::future<void> checkDatabaseTables();
    std::future<void> recheckTables();
    std::future<void> persistFormsBatch();
    std::future<void> persistFormsIndividual();
    std::future<void> persistForms();
    std::future<void> restorePersistedForms();
    (Omit<Form, 'steps'> & validateAndSanitizeFormData(const std::any& formRow);
    void if(auto !result.success);
    std::optional<Omit<FormField, 'value'>> validateAndSanitizeFieldData(const std::any& fieldRow);
    std::variant<std::string, double, bool> parseFieldValue(const std::string& value, const std::string& type, boolean = false isSecret);
     validateFieldValue(const std::any& value, FormField field);
    void switch(auto field.type);
    std::future<void> cleanupOldForms();
};


} // namespace elizaos
