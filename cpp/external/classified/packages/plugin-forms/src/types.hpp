#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Possible types for form fields
 */
using FormFieldType = std::variant<, std::string>;

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
    std::optional<std::variant<std::string, double, bool>> value;
    std::optional<std::string> error;
};

/**
 * Represents a step in a multi-step form
 */
struct FormStep {
    std::string id;
    std::string name;
    std::vector<FormField> fields;
    std::optional<bool> completed;
};

/**
 * Possible statuses for a form
 */
using FormStatus = std::string;

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
};

/**
 * Template for creating forms
 */
struct FormTemplate {
    std::string name;
    std::optional<std::string> description;
    std::vector<FormStep> steps;
};

/**
 * Result of a form update operation
 */
struct FormUpdateResult {
    bool success;
    std::optional<Form> form;
    std::optional<std::vector<std::string>> updatedFields;
    std::optional<bool> stepCompleted;
    std::optional<bool> formCompleted;
    std::optional<std::string> currentStep;
    std::optional<std::string> message;
};

// Extend the core service types with forms service
  struct ServiceTypeRegistry {
    std::string FORMS;
};

// Export service type constant

} // namespace elizaos
