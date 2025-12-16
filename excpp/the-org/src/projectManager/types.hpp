#include "elizaos/core.hpp"
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
 * Type definitions for Project Manager
 */

;

// Team member enums
using WeekDay = std::variant<, 'MONDAY', 'TUESDAY', 'WEDNESDAY', 'THURSDAY', 'FRIDAY', 'SATURDAY', 'SUNDAY'>;

using EmploymentStatus = std::variant<'FULL_TIME', 'PART_TIME', 'CONTRACTOR', 'INTERN'>;

using PlatformType = std::variant<'DISCORD', 'SLACK', 'TELEGRAM', 'EMAIL'>;

using Skill = std::string;

// Platform-specific contact info
struct PlatformContact {
    PlatformType platform;
    string; // username, email, etc. identifier;
};


// Team member availability
struct Availability {
    std::optional<std::vector<WeekDay>> workDays;
    string; // 24-hour format HH:MM workHoursStart;
    string; // 24-hour format HH:MM workHoursEnd;
    string; // e.g., 'America/New_York', 'UTC' timeZone;
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
using ProjectStatus = std::variant<'PLANNING', 'IN_PROGRESS', 'ON_HOLD', 'COMPLETED', 'CANCELLED'>;

using TaskStatus = std::variant<'TODO', 'IN_PROGRESS', 'BLOCKED', 'COMPLETED', 'CANCELLED'>;

using TaskPriority = std::variant<'LOW', 'MEDIUM', 'HIGH', 'URGENT'>;

// Task interface
struct Task {
    UUID id;
    std::string name;
    std::string description;
    TaskStatus status;
    TaskPriority priority;
    std::optional<UUID; // TeamMember ID> assignedTo;
    std::optional<UUID[]; // Task IDs> dependsOn;
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
    UUID[]; // Task IDs tasks;
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
    UUID[]; // TeamMember IDs teamMembers;
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
using UpdateType = std::variant<'CHECK_IN', 'STATUS_UPDATE', 'BLOCKER_REPORT'>;

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
    'team-member-update' type;
    UUID updateId;
    UUID teamMemberId;
    std::optional<std::string> teamMemberName;
    std::string serverName;
    std::string checkInType;
    std::string timestamp;
    std::optional<UUID> channelId;
    std::optional<std::string> serverId;
    string; // JSON stringified object containing all dynamic Q&A pairs answers;
};


struct CheckInSchedule {
    'team-member-checkin-schedule' type;
    std::string scheduleId;
    std::optional<string | undefined | null> teamMemberName;
    std::optional<std::string> teamMemberUserName;
    std::string checkInType;
    std::string channelId;
    'WEEKDAYS' | 'DAILY' | 'WEEKLY' | 'BIWEEKLY' | 'MONTHLY' | 'CUSTOM' frequency;
    string; // Time in "HH:mm" format checkInTime;
    std::string createdAt;
    std::optional<string; // Add source field to track where the message came from> source;
    std::optional<std::string> serverId;
};


} // namespace elizaos
