#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define the days of the week for availability
using WeekDay = std::variant<, 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday'>;

// Define employment status types
using EmploymentStatus = std::variant<'FULL_TIME', 'PART_TIME', 'FREELANCE', 'NONE'>;

// Define report types
using ReportType = std::variant<'DAILY', 'WEEKLY', 'MONTHLY'>;

// Define task status types
using TaskStatus = std::variant<'NOT_STARTED', 'IN_PROGRESS', 'BLOCKED', 'COMPLETED'>;

// Define task priority levels
using TaskPriority = std::variant<'LOW', 'MEDIUM', 'HIGH', 'URGENT'>;

// Define platform types for multi-platform support
using PlatformType = std::variant<'DISCORD', 'TELEGRAM', 'SLACK', 'EMAIL'>;

/**
 * Interface for team member availability
 */
struct Availability {
    std::vector<WeekDay> workDays;
    std::string start;
    std::string end;
    std::string timeZone;
    double hoursPerWeek;
    EmploymentStatus employmentStatus;
};

/**
 * Interface for platform-specific contact information
 */
struct PlatformContact {
    PlatformType platform;
    std::string username;
    bool isPreferred;
};

/**
 * Interface representing a team member
 */
struct TeamMember {
    UUID id;
    std::string name;
    Availability availability;
    std::vector<PlatformContact> contacts;
    std::optional<std::vector<std::string>> skills;
    std::optional<std::vector<UUID>> projects;
    std::string dateAdded;
    std::optional<std::string> lastCheckIn;
};

/**
 * Interface representing a task within a project
 */
struct Task {
    UUID id;
    UUID projectId;
    std::string title;
    std::string description;
    std::optional<UUID> assignedTo;
    TaskStatus status;
    TaskPriority priority;
    std::optional<std::string> dueDate;
    std::string createdAt;
    std::string updatedAt;
    std::optional<std::vector<UUID>> dependencies;
    std::optional<double> estimatedHours;
};

/**
 * Interface representing a project milestone
 */
struct Milestone {
    UUID id;
    UUID projectId;
    std::string title;
    std::string description;
    std::string dueDate;
    std::vector<UUID> tasks;
    bool completed;
};

/**
 * Interface representing a project
 */
struct Project {
    UUID id;
    std::string name;
    std::string description;
    std::vector<UUID> teamMembers;
    std::string startDate;
    std::optional<std::string> targetEndDate;
    std::optional<std::string> actualEndDate;
    std::optional<std::string> client;
    std::variant<'PLANNING', 'ACTIVE', 'ON_HOLD', 'COMPLETED', 'CANCELLED'> status;
    std::string createdAt;
    std::string updatedAt;
};

/**
 * Interface representing daily updates from team members
 */
struct DailyUpdate {
    UUID id;
    UUID teamMemberId;
    UUID projectId;
    std::string date;
    std::string summary;
    std::optional<std::vector<UUID>> tasksCompleted;
    std::optional<std::vector<UUID>> tasksInProgress;
    std::optional<std::vector<std::string>> blockers;
    std::optional<double> hoursWorked;
};

/**
 * Interface for team member summaries in reports
 */
struct TeamMemberSummary {
    UUID teamMemberId;
    std::string name;
    std::string summary;
    double tasksCompleted;
    double tasksInProgress;
    bool hasBlockers;
};

/**
 * Interface representing a project progress metrics
 */
struct ProjectProgress {
    UUID projectId;
    std::string date;
    double tasksCompleted;
    double tasksInProgress;
    double tasksBlocked;
    double tasksNotStarted;
    double totalTasks;
    double completionPercentage;
    UUID teamMemberId;
    double tasksCompleted;
    double tasksInProgress;
    bool hasUpdate;
};

/**
 * Interface representing a generated report
 */
struct Report {
    UUID id;
    ReportType type;
    UUID projectId;
    std::string generatedAt;
    std::string summary;
    double completionPercentage;
    double tasksCompletedSinceLastReport;
    std::variant<'ON_TRACK', 'AT_RISK', 'DELAYED'> onTrackStatus;
    std::vector<TeamMemberSummary> teamMemberSummaries;
    UUID milestoneId;
    std::string title;
    std::variant<'NOT_STARTED', 'IN_PROGRESS', 'COMPLETED'> status;
    std::string dueDate;
    double completionPercentage;
};

/**
 * Interface for check-in status tracking
 */
struct CheckInStatus {
    UUID teamMemberId;
    UUID projectId;
    std::optional<std::string> lastCheckInDate;
    std::string nextCheckInDue;
    double remindersSent;
    std::variant<'PENDING', 'COMPLETED', 'MISSED'> status;
};


} // namespace elizaos
