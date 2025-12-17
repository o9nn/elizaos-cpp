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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Type definitions for Project Manager
 */

// Team member enums
using WeekDay = std::variant<, std::string>;

using EmploymentStatus = std::string;

using PlatformType = std::string;

using Skill = std::string;

// Platform-specific contact info
struct PlatformContact {
    PlatformType platform;
    std::string identifier;
};

// Team member availability
struct Availability {
    std::optional<std::vector<WeekDay>> workDays;
    std::string workHoursStart;
    std::string workHoursEnd;
    std::string timeZone;
};

// Team member interface
struct TeamMember {
    UUID id;
    std::string name;
    Availability availability;
    EmploymentStatus employmentStatus;
    double hoursPerWeek;
    std::optional<std::vector<Skill>> skills;
    std::vector<PlatformContact> contacts;
    std::string createdAt;
    std::string updatedAt;
};

// Project status types
using ProjectStatus = std::string;

using TaskStatus = std::string;

using TaskPriority = std::string;

// Task interface
struct Task {
    UUID id;
    std::string name;
    std::string description;
    TaskStatus status;
    TaskPriority priority;
    std::optional<UUID> assignedTo;
    std::optional<std::vector<UUID>> dependsOn;
    std::optional<double> estimatedHours;
    std::optional<double> actualHours;
    std::string createdAt;
    std::string updatedAt;
    std::optional<std::string> dueDate;
};

// Milestone interface
struct Milestone {
    UUID id;
    std::string name;
    std::string description;
    std::vector<UUID> tasks;
    std::string dueDate;
    std::optional<std::string> completedAt;
};

// Progress metrics
struct ProjectProgress {
    std::string date;
    double completedTasks;
    double totalTasks;
    double completedMilestones;
    double totalMilestones;
    double blockedTasks;
};

// Project interface
struct Project {
    UUID id;
    std::string name;
    std::string description;
    ProjectStatus status;
    std::vector<UUID> teamMembers;
    std::vector<Task> tasks;
    std::vector<Milestone> milestones;
    std::vector<ProjectProgress> progress;
    std::string startDate;
    std::string targetEndDate;
    std::optional<std::string> actualEndDate;
    std::string createdAt;
    std::string updatedAt;
};

// Daily update types
using UpdateType = std::string;

// Daily update interface
struct DailyUpdate {
    std::optional<UUID> id;
    UUID teamMemberId;
    std::optional<UUID> projectId;
    UpdateType updateType;
    std::optional<std::vector<std::string>> completedItems;
    std::optional<std::vector<std::string>> inProgressItems;
    std::optional<std::vector<std::string>> blockers;
    std::optional<std::string> notes;
    std::string timestamp;
};

struct TeamMemberUpdate {
    std::string type;
    UUID updateId;
    UUID teamMemberId;
    std::optional<std::string> teamMemberName;
    std::string serverName;
    std::string checkInType;
    std::string timestamp;
    std::optional<UUID> channelId;
    std::optional<std::string> serverId;
    std::string answers;
};

struct CheckInSchedule {
    std::string type;
    std::string scheduleId;
    std::optional<std::string> teamMemberName;
    std::optional<std::string> teamMemberUserName;
    std::string checkInType;
    std::string channelId;
    std::string frequency;
    std::string checkInTime;
    std::string createdAt;
    std::optional<std::string> source;
    std::optional<std::string> serverId;
};


} // namespace elizaos
