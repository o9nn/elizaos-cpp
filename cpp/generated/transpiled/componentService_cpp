#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-todo/services/componentService.h"

ComponentService::ComponentService(std::shared_ptr<IAgentRuntime> runtime_) : runtime(runtime_)  {
}

std::shared_ptr<Promise<any>> ComponentService::getComponent(std::shared_ptr<UUID> entityId, string componentType)
{
    try
    {
        auto component = std::async([=]() { this->runtime->getComponent(dbCompat->formatUuid(entityId), componentType); });
        if (!component) return nullptr;
        return this->normalizeComponent(component);
    }
    catch (const any& error)
    {
        logger->error(std::string("[ComponentService] Error getting component:"), error);
        return nullptr;
    }
}

std::shared_ptr<Promise<boolean>> ComponentService::createComponent(object params)
{
    try
    {
        auto result = std::async([=]() { this->runtime->createComponent(as<any>(params)); });
        return !!result;
    }
    catch (const any& error)
    {
        logger->error(std::string("[ComponentService] Error creating component:"), error);
        return false;
    }
}

std::shared_ptr<Promise<boolean>> ComponentService::updateComponent(object params)
{
    try
    {
        std::async([=]() { this->runtime->updateComponent(as<any>(params)); });
        return true;
    }
    catch (const any& error)
    {
        logger->error(std::string("[ComponentService] Error updating component:"), error);
        return false;
    }
}

std::shared_ptr<Component> ComponentService::normalizeComponent(std::shared_ptr<Component> component)
{
    if (component->data) {
        component->data = OR((dbCompat->parseJson(as<any>(component->data))), (object{}));
    }
    if (AND((component->data), (type_of(component->data) == std::string("object")))) {
        this->normalizeBooleans(component->data);
    }
    if (component->id) {
        component->id = as<std::shared_ptr<UUID>>(component->id->toLowerCase());
    }
    if (component->entityId) {
        component->entityId = as<std::shared_ptr<UUID>>(component->entityId->toLowerCase());
    }
    if (component->agentId) {
        component->agentId = as<std::shared_ptr<UUID>>(component->agentId->toLowerCase());
    }
    if (component->roomId) {
        component->roomId = as<std::shared_ptr<UUID>>(component->roomId->toLowerCase());
    }
    if (component->worldId) {
        component->worldId = as<std::shared_ptr<UUID>>(component->worldId->toLowerCase());
    }
    if (component->sourceEntityId) {
        component->sourceEntityId = as<std::shared_ptr<UUID>>(component->sourceEntityId->toLowerCase());
    }
    return component;
}

void ComponentService::normalizeBooleans(any obj)
{
    if (OR((!obj), (type_of(obj) != std::string("object")))) return;
    for (auto& key : keys_(obj))
    {
        if (obj["hasOwnProperty"](key)) {
            auto value = const_(obj)[key];
            if (AND((type_of(value) == std::string("number")), ((OR((value == 0), (value == 1)))))) {
                obj[key] = dbCompat->parseBoolean(value);
            } else if (type_of(value) == std::string("object")) {
                this->normalizeBooleans(value);
            }
        }
    }
}

std::shared_ptr<ComponentService> createComponentService(std::shared_ptr<IAgentRuntime> runtime)
{
    return std::make_shared<ComponentService>(runtime);
};


