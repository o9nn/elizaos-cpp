#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_TYPES_INDEX_H
#include "core.h"
#include "@elizaos/core.h"

typedef any WeekDay;

typedef any EmploymentStatus;

typedef any ReportType;

typedef any TaskStatus;

typedef any TaskPriority;

typedef any PlatformType;

class Availability;
class PlatformContact;
class TeamMember;
class Task;
class Milestone;
class Project;
class DailyUpdate;
class TeamMemberSummary;
class ProjectProgress;
class Report;
class CheckInStatus;

class Availability : public object, public std::enable_shared_from_this<Availability> {
public:
    using std::enable_shared_from_this<Availability>::shared_from_this;
    array<WeekDay> workDays;

    object workHours;

    string timeZone;

    double hoursPerWeek;

    EmploymentStatus employmentStatus;
};

class PlatformContact : public object, public std::enable_shared_from_this<PlatformContact> {
public:
    using std::enable_shared_from_this<PlatformContact>::shared_from_this;
    PlatformType platform;

    string username;

    boolean isPreferred;
};

class TeamMember : public object, public std::enable_shared_from_this<TeamMember> {
public:
    using std::enable_shared_from_this<TeamMember>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    std::shared_ptr<Availability> availability;

    array<std::shared_ptr<PlatformContact>> contacts;

    array<string> skills;

    array<std::shared_ptr<UUID>> projects;

    string dateAdded;

    string lastCheckIn;
};

class Task : public object, public std::enable_shared_from_this<Task> {
public:
    using std::enable_shared_from_this<Task>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> projectId;

    string title;

    string description;

    std::shared_ptr<UUID> assignedTo;

    TaskStatus status;

    TaskPriority priority;

    string dueDate;

    string createdAt;

    string updatedAt;

    array<std::shared_ptr<UUID>> dependencies;

    double estimatedHours;
};

class Milestone : public object, public std::enable_shared_from_this<Milestone> {
public:
    using std::enable_shared_from_this<Milestone>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> projectId;

    string title;

    string description;

    string dueDate;

    array<std::shared_ptr<UUID>> tasks;

    boolean completed;
};

class Project : public object, public std::enable_shared_from_this<Project> {
public:
    using std::enable_shared_from_this<Project>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string description;

    array<std::shared_ptr<UUID>> teamMembers;

    string startDate;

    string targetEndDate;

    string actualEndDate;

    string client;

    any status;

    string createdAt;

    string updatedAt;
};

class DailyUpdate : public object, public std::enable_shared_from_this<DailyUpdate> {
public:
    using std::enable_shared_from_this<DailyUpdate>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> teamMemberId;

    std::shared_ptr<UUID> projectId;

    string date;

    string summary;

    array<std::shared_ptr<UUID>> tasksCompleted;

    array<std::shared_ptr<UUID>> tasksInProgress;

    array<string> blockers;

    double hoursWorked;
};

class TeamMemberSummary : public object, public std::enable_shared_from_this<TeamMemberSummary> {
public:
    using std::enable_shared_from_this<TeamMemberSummary>::shared_from_this;
    std::shared_ptr<UUID> teamMemberId;

    string name;

    string summary;

    double tasksCompleted;

    double tasksInProgress;

    boolean hasBlockers;
};

class ProjectProgress : public object, public std::enable_shared_from_this<ProjectProgress> {
public:
    using std::enable_shared_from_this<ProjectProgress>::shared_from_this;
    std::shared_ptr<UUID> projectId;

    string date;

    double tasksCompleted;

    double tasksInProgress;

    double tasksBlocked;

    double tasksNotStarted;

    double totalTasks;

    double completionPercentage;

    array<object> teamMemberStats;
};

class Report : public object, public std::enable_shared_from_this<Report> {
public:
    using std::enable_shared_from_this<Report>::shared_from_this;
    std::shared_ptr<UUID> id;

    ReportType type;

    std::shared_ptr<UUID> projectId;

    string generatedAt;

    string summary;

    object progressMetrics;

    array<std::shared_ptr<TeamMemberSummary>> teamMemberSummaries;

    array<object> milestoneUpdates;
};

class CheckInStatus : public object, public std::enable_shared_from_this<CheckInStatus> {
public:
    using std::enable_shared_from_this<CheckInStatus>::shared_from_this;
    std::shared_ptr<UUID> teamMemberId;

    std::shared_ptr<UUID> projectId;

    string lastCheckInDate;

    string nextCheckInDue;

    double remindersSent;

    any status;
};

#endif
