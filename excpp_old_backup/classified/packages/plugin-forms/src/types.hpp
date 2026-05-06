#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Possible types for form fields
 */
using FormFieldType = std::variant<, 'text', 'number', 'email', 'tel', 'url', 'textarea', 'choice', 'checkbox', 'date', 'time', 'datetime'>;

/**
 * Represents a single field in a form
 */
struct FormField {
    std::string id;
    std::string label;
    FormFieldType type;
    std::optional<std::string> description;
    std::optional<std::string> criteria;
    std::optional<bool> optional;
    std::optional<bool> secret;
    std::optional<string | number | boolean> value;
    std::optional<std::string> error;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};

/**
 * Represents a step in a multi-step form
 */
struct FormStep {
    std::string id;
    std::string name;
    std::vector<FormField> fields;
    std::optional<bool> completed;
    std::optional<(form: Form, stepId: string) => Promise<void>> onComplete;
};

/**
 * Possible statuses for a form
 */
using FormStatus = std::variant<'active', 'completed', 'cancelled'>;

/**
 * Represents a complete form instance
 */
struct Form {
    UUID id;
    std::string name;
    std::optional<std::string> description;
    std::vector<FormStep> steps;
    double currentStepIndex;
    FormStatus status;
    double createdAt;
    double updatedAt;
    std::optional<double> completedAt;
    UUID agentId;
    std::optional<(form: Form) => Promise<void>> onComplete;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};

/**
 * Template for creating forms
 */
struct FormTemplate {
    std::string name;
    std::optional<std::string> description;
    std::vector<FormStep> steps;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};

/**
 * Result of a form update operation
 */
struct FormUpdateResult {
    bool success;
    std::optional<Form> form;
    std::optional<std::vector<std::string>> updatedFields;
    std::optional<std::vector<{ fieldId: string; message: string }>> errors;

// Extend the core service types with forms service
  struct ServiceTypeRegistry {
    'FORMS' FORMS;
};

// Export service type constant

} // namespace elizaos
