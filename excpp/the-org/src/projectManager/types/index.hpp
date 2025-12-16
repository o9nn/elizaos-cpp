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
    { workHours;
    string; // HH:MM format start;
    string; // HH:MM format end;
    string; // e.g., "America/Los_Angeles" timeZone;
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
    std::optional<UUID[]; // Project IDs the team member is assigned to> projects;
    string; // ISO date format dateAdded;
    std::optional<string; // ISO date format> lastCheckIn;
};

/**
 * Interface representing a task within a project
 */
struct Task {
    UUID id;
    UUID projectId;
    std::string title;
    std::string description;
    std::optional<UUID; // Team member ID> assignedTo;
    TaskStatus status;
    TaskPriority priority;
    std::optional<string; // ISO date format> dueDate;
    string; // ISO date format createdAt;
    string; // ISO date format updatedAt;
    std::optional<UUID[]; // IDs of tasks that this task depends on> dependencies;
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
    string; // ISO date format dueDate;
    UUID[]; // Task IDs associated with this milestone tasks;
    bool completed;
};

/**
 * Interface representing a project
 */
struct Project {
    UUID id;
    std::string name;
    std::string description;
    UUID[]; // Team member IDs teamMembers;
    string; // ISO date format startDate;
    std::optional<string; // ISO date format> targetEndDate;
    std::optional<string; // ISO date format> actualEndDate;
    std::optional<std::string> client;
    'PLANNING' | 'ACTIVE' | 'ON_HOLD' | 'COMPLETED' | 'CANCELLED' status;
    string; // ISO date format createdAt;
    string; // ISO date format updatedAt;
};

/**
 * Interface representing daily updates from team members
 */
struct DailyUpdate {
    UUID id;
    UUID teamMemberId;
    UUID projectId;
    string; // ISO date format date;
    string; // Paragraph summary of work done summary;
    std::optional<UUID[]; // Task IDs completed> tasksCompleted;
    std::optional<UUID[]; // Task IDs in progress> tasksInProgress;
    std::optional<string[]; // Description of any blockers> blockers;
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
    string; // ISO date format date;
    double tasksCompleted;
    double tasksInProgress;
    double tasksBlocked;
    double tasksNotStarted;
    double totalTasks;
    double completionPercentage;
    { teamMemberStats;
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
    string; // ISO date format generatedAt;
    std::string summary;
    { progressMetrics;
    double completionPercentage;
    double tasksCompletedSinceLastReport;
    'ON_TRACK' | 'AT_RISK' | 'DELAYED' onTrackStatus;
    std::vector<TeamMemberSummary> teamMemberSummaries;
    std::optional<{> milestoneUpdates;
    UUID milestoneId;
    std::string title;
    'NOT_STARTED' | 'IN_PROGRESS' | 'COMPLETED' status;
    string; // ISO date format dueDate;
    double completionPercentage;
};

/**
 * Interface for check-in status tracking
 */
struct CheckInStatus {
    UUID teamMemberId;
    UUID projectId;
    std::optional<string; // ISO date format> lastCheckInDate;
    string; // ISO date format nextCheckInDue;
    double remindersSent;
    'PENDING' | 'COMPLETED' | 'MISSED' status;
};


} // namespace elizaos
