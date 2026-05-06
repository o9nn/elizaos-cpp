#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/services/taskService.h"

TaskService::TaskService(std::shared_ptr<IAgentRuntime> runtime_) : runtime(runtime_)  {
}

std::shared_ptr<Promise<array<std::shared_ptr<Task>>>> TaskService::getTasks(object params)
{
    try
    {
        auto tasks = std::async([=]() { this->runtime->getTasks(params); });
        return tasks->map([=](auto task) mutable
        {
            return this->normalizeTask(task);
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std::string("[TaskService] Error getting tasks:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<any>> TaskService::createTask(object params)
{
    try
    {
        auto taskId = std::async([=]() { this->runtime->createTask(as<any>(params)); });
        return taskId;
    }
    catch (const any& error)
    {
        logger->error(std::string("[TaskService] Error creating task:"), error);
        return nullptr;
    }
}

std::shared_ptr<Promise<boolean>> TaskService::updateTask(std::shared_ptr<UUID> taskId, object updates)
{
    try
    {
        std::async([=]() { this->runtime->updateTask(taskId, updates); });
        return true;
    }
    catch (const any& error)
    {
        logger->error(std::string("[TaskService] Error updating task:"), error);
        return false;
    }
}

std::shared_ptr<Task> TaskService::normalizeTask(std::shared_ptr<Task> task)
{
    if (task->tags) {
        task->tags = dbCompat->parseArray(as<any>(task->tags));
    }
    if (task->metadata) {
        task->metadata = OR((dbCompat->parseJson(as<any>(task->metadata))), (object{}));
    }
    if (task->metadata) {
        if (task->metadata->completedToday != undefined) {
            task->metadata->completedToday = dbCompat->parseBoolean(task->metadata->completedToday);
        }
        if (task->metadata->urgent != undefined) {
            task->metadata->urgent = dbCompat->parseBoolean(task->metadata->urgent);
        }
    }
    if (task->metadata) {
        if (task->metadata->dueDate) {
            auto parsedDate = dbCompat->parseDate(task->metadata->dueDate);
            task->metadata->dueDate = (parsedDate) ? any(parsedDate->toISOString()) : any(undefined);
        }
        if (task->metadata->completedAt) {
            auto parsedDate = dbCompat->parseDate(task->metadata->completedAt);
            task->metadata->completedAt = (parsedDate) ? any(parsedDate->toISOString()) : any(undefined);
        }
        if (task->metadata->lastCompletedAt) {
            auto parsedDate = dbCompat->parseDate(task->metadata->lastCompletedAt);
            task->metadata->lastCompletedAt = (parsedDate) ? any(parsedDate->getTime()) : any(undefined);
        }
        if (task->metadata->lastReminderSent) {
            auto parsedDate = dbCompat->parseDate(task->metadata->lastReminderSent);
            task->metadata->lastReminderSent = (parsedDate) ? any(parsedDate->toISOString()) : any(undefined);
        }
    }
    return task;
}

std::shared_ptr<Promise<array<std::shared_ptr<Task>>>> TaskService::searchTasksByName(string name, object params)
{
    auto allTasks = std::async([=]() { this->getTasks(params); });
    shared searchLower = name->toLowerCase();
    return allTasks->filter([=](auto task) mutable
    {
        return task->name->toLowerCase()->includes(searchLower);
    }
    );
}

std::shared_ptr<TaskService> createTaskService(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<TaskService>(runtime);
};


