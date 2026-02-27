#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_TYPES_H
#include "core.hpp"
// External dependency removed

typedef any WeekDay;

typedef any EmploymentStatus;

typedef any PlatformType;

typedef string Skill;

typedef any ProjectStatus;

typedef any TaskStatus;

typedef any TaskPriority;

typedef any UpdateType;

class PlatformContact;
class Availability;
class TeamMember;
class Task;
class Milestone;
class ProjectProgress;
class Project;
class DailyUpdate;
class TeamMemberUpdate;
class CheckInSchedule;

class PlatformContact : public object, public std::enable_shared_from_this<PlatformContact> {
public:
    using std::enable_shared_from_this<PlatformContact>::shared_from_this;
    PlatformType platform;

    string identifier;
};

class Availability : public object, public std::enable_shared_from_this<Availability> {
public:
    using std::enable_shared_from_this<Availability>::shared_from_this;
    array<WeekDay> workDays;

    string workHoursStart;

    string workHoursEnd;

    string timeZone;
};

class TeamMember : public object, public std::enable_shared_from_this<TeamMember> {
public:
    using std::enable_shared_from_this<TeamMember>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    std::shared_ptr<Availability> availability;

    EmploymentStatus employmentStatus;

    double hoursPerWeek;

    array<Skill> skills;

    array<std::shared_ptr<PlatformContact>> contacts;

    string createdAt;

    string updatedAt;
};

class Task : public object, public std::enable_shared_from_this<Task> {
public:
    using std::enable_shared_from_this<Task>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string description;

    TaskStatus status;

    TaskPriority priority;

    std::shared_ptr<UUID> assignedTo;

    array<std::shared_ptr<UUID>> dependsOn;

    double estimatedHours;

    double actualHours;

    string createdAt;

    string updatedAt;

    string dueDate;
};

class Milestone : public object, public std::enable_shared_from_this<Milestone> {
public:
    using std::enable_shared_from_this<Milestone>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string description;

    array<std::shared_ptr<UUID>> tasks;

    string dueDate;

    string completedAt;
};

class ProjectProgress : public object, public std::enable_shared_from_this<ProjectProgress> {
public:
    using std::enable_shared_from_this<ProjectProgress>::shared_from_this;
    string date;

    double completedTasks;

    double totalTasks;

    double completedMilestones;

    double totalMilestones;

    double blockedTasks;
};

class Project : public object, public std::enable_shared_from_this<Project> {
public:
    using std::enable_shared_from_this<Project>::shared_from_this;
    std::shared_ptr<UUID> id;

    string name;

    string description;

    ProjectStatus status;

    array<std::shared_ptr<UUID>> teamMembers;

    array<std::shared_ptr<Task>> tasks;

    array<std::shared_ptr<Milestone>> milestones;

    array<std::shared_ptr<ProjectProgress>> progress;

    string startDate;

    string targetEndDate;

    string actualEndDate;

    string createdAt;

    string updatedAt;
};

class DailyUpdate : public object, public std::enable_shared_from_this<DailyUpdate> {
public:
    using std::enable_shared_from_this<DailyUpdate>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> teamMemberId;

    std::shared_ptr<UUID> projectId;

    UpdateType updateType;

    array<string> completedItems;

    array<string> inProgressItems;

    array<string> blockers;

    string notes;

    string timestamp;
};

class TeamMemberUpdate : public object, public std::enable_shared_from_this<TeamMemberUpdate> {
public:
    using std::enable_shared_from_this<TeamMemberUpdate>::shared_from_this;
    string type;

    std::shared_ptr<UUID> updateId;

    std::shared_ptr<UUID> teamMemberId;

    string teamMemberName;

    string serverName;

    string checkInType;

    string timestamp;

    std::shared_ptr<UUID> channelId;

    string serverId;

    string answers;
};

class CheckInSchedule : public object, public std::enable_shared_from_this<CheckInSchedule> {
public:
    using std::enable_shared_from_this<CheckInSchedule>::shared_from_this;
    string type;

    string scheduleId;

    any teamMemberName;

    string teamMemberUserName;

    string checkInType;

    string channelId;

    any frequency;

    string checkInTime;

    string createdAt;

    string source;

    string serverId;
};

#endif
