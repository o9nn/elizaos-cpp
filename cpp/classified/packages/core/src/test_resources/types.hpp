#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Interface representing a User.
 * @typedef {Object} User
 * @property {string} id - The user's ID.
 * @property {string} [email] - The user's email (optional).
 * @property {string} [phone] - The user's phone number (optional).
 * @property {string} [role] - The user's role (optional).
 */
/**
 * Represents a User with optional properties.
 * @typedef {Object} User
 * @property {string} id - The unique identifier for the user.
 * @property {string} [email] - The email address of the user (optional).
 * @property {string} [phone] - The phone number of the user (optional).
 * @property {string} [role] - The role of the user (optional).
 */
struct User {
    std::string id;
    std::optional<std::string> email;
    std::optional<std::string> phone;
    std::optional<std::string> role;
};


} // namespace elizaos
