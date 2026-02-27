#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/managers/build-manager.h"

BuildManager::BuildManager(std::shared_ptr<IAgentRuntime> runtime) {
    this->runtime = runtime;
}

void BuildManager::translate(any entityId, std::tuple<double, double, double> position)
{
    auto service = this->getService();
    auto world = service->getWorld();
    auto entity = world->entities->items->get(entityId);
    if (entity) {
        auto controls = world->controls;
        if (controls) {
            std::async([=]() { controls->goto(entity->root->position->x, entity->root->position->z); });
        }
        entity->root->position->fromArray(position);
        this->entityUpdate(entity);
    }
}

void BuildManager::rotate(any entityId, std::tuple<double, double, double, double> quaternion)
{
    auto service = this->getService();
    auto world = service->getWorld();
    auto entity = world->entities->items->get(entityId);
    if (entity) {
        auto controls = world->controls;
        if (controls) {
            std::async([=]() { controls->goto(entity->root->position->x, entity->root->position->z); });
        }
        entity->root->quaternion->fromArray(quaternion);
        this->entityUpdate(entity);
    }
}

void BuildManager::scale(any entityId, std::tuple<double, double, double> scale)
{
    auto service = this->getService();
    auto world = service->getWorld();
    auto entity = world->entities->items->get(entityId);
    if (entity) {
        auto controls = world->controls;
        if (controls) {
            std::async([=]() { controls->goto(entity->root->position->x, entity->root->position->z); });
        }
        entity->root->scale->fromArray(scale);
        this->entityUpdate(entity);
    }
}

void BuildManager::duplicate(any entityId)
{
    auto service = this->getService();
    auto world = service->getWorld();
    auto entity = world->entities->items->get(entityId);
    auto controls = world->controls;
    if (controls) {
        std::async([=]() { controls->goto(entity->root->position->x, entity->root->position->z); });
    }
    if (entity->isApp) {
        auto blueprintId = entity->data->blueprint;
        if (entity->blueprint->unique) {
            auto blueprint = object{
                object::pair{std:("id"), uuid()}, 
                object::pair{std:("version"), 0}, 
                object::pair{std:("name"), entity->blueprint->name}, 
                object::pair{std:("image"), entity->blueprint->image}, 
                object::pair{std:("author"), entity->blueprint->author}, 
                object::pair{std:("url"), entity->blueprint->url}, 
                object::pair{std:("desc"), entity->blueprint->desc}, 
                object::pair{std:("model"), entity->blueprint->model}, 
                object::pair{std:("script"), entity->blueprint->script}, 
                object::pair{std:("props"), cloneDeep(entity->blueprint->props)}, 
                object::pair{std:("preload"), entity->blueprint->preload}, 
                object::pair{std:("public"), entity->blueprint->public}, 
                object::pair{std:("locked"), entity->blueprint->locked}, 
                object::pair{std:("frozen"), entity->blueprint->frozen}, 
                object::pair{std:("unique"), entity->blueprint->unique}, 
                object::pair{std:("disabled"), entity->blueprint->disabled}
            };
            world->blueprints->add(blueprint, true);
            blueprintId = blueprint["id"];
        }
        auto data = object{
            object::pair{std:("id"), uuid()}, 
            object::pair{std:("type"), std:("app")}, 
            object::pair{std:("blueprint"), blueprintId}, 
            object::pair{std:("position"), entity->root->position->toArray()}, 
            object::pair{std:("quaternion"), entity->root->quaternion->toArray()}, 
            object::pair{std:("scale"), entity->root->scale->toArray()}, 
            object::pair{std:("mover"), nullptr}, 
            object::pair{std:("uploader"), nullptr}, 
            object::pair{std:("pinned"), false}, 
            object::pair{std:("state"), object{}}
        };
        world->entities->add(data, true);
    }
}

void BuildManager::delete(any entityId)
{
    auto service = this->getService();
    auto world = service->getWorld();
    auto entity = world->entities->items->get(entityId);
    if (AND((entity->isApp), (!entity->data->pinned))) {
        auto controls = world->controls;
        if (controls) {
            std::async([=]() { controls->goto(entity->root->position->x, entity->root->position->z); });
        }
        entity->destroy(true);
        this->entityUpdate(entity);
    }
}

void BuildManager::importEntity(string url, any position, any quaternion)
{
    auto service = this->getService();
    auto world = service->getWorld();
    auto resolvedUrlurl = std::async([=]() { resolveUrl(url, world); });
    any file;
    auto resp = std::async([=]() { fetch(resolvedUrlurl); });
    auto blob = std::async([=]() { resp->blob(); });
    file = std::make_shared<File>(array<std::shared_ptr<Blob>>{ blob }, url->split(std:("/"))->pop(), object{
        object::pair{std:("type"), resp->headers->get(std:("content-type"))}
    });
    if (!file) return std::shared_ptr<Promise<void>>();
    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
    {
        return setTimeout(resolve, 100);
    }
    ); });
    auto maxSize = world->network->maxUploadSize * 1024 * 1024;
    if (file["size"] > maxSize) {
        console->error(std:("File too large. Maximum size is ") + (maxSize / (1024 * 1024)) + std:("MB"));
        return std::shared_ptr<Promise<void>>();
    }
    auto validVec3 = [=](auto v) mutable
    {
        return AND((AND((Array->isArray(v)), (v["length"] == 3))), (v["every"]([=](auto n) mutable
        {
            return type_of(n) == std:("number");
        }
        )));
    };
    auto validQuat = [=](auto q) mutable
    {
        return AND((AND((Array->isArray(q)), (q["length"] == 4))), (q["every"]([=](auto n) mutable
        {
            return type_of(n) == std:("number");
        }
        )));
    };
    position = (validVec3(position)) ? any(position) (array<double>{ 0, 0, 0 });
    quaternion = (validQuat(quaternion)) ? any(quaternion) (array<double>{ 0, 0, 0, 1 });
    auto controls = world->controls;
    if (controls) {
        std::async([=]() { controls->goto(const_(position)[0], const_(position)[2]); });
    }
    auto transform = object{
        object::pair{std:("position"), std:("position")}, 
        object::pair{std:("quaternion"), std:("quaternion")}
    };
    auto ext = file["name"]["split"](std:("."))["pop"]()["toLowerCase"]();
    if (ext == std:("hyp")) {
        this->addApp(file, transform);
    }
    if (OR((ext == std:("glb")), (ext == std:("vrm")))) {
        this->addModel(file, transform);
    }
}

void BuildManager::addApp(any file, any transform)
{
    auto service = this->getService();
    shared world = service->getWorld();
    auto info = std::async([=]() { importApp(file); });
    auto blueprint = object{
        object::pair{std:("id"), uuid()}, 
        object::pair{std:("version"), 0}, 
        object::pair{std:("name"), info->blueprint->name}, 
        object::pair{std:("image"), info->blueprint->image}, 
        object::pair{std:("author"), info->blueprint->author}, 
        object::pair{std:("url"), info->blueprint->url}, 
        object::pair{std:("desc"), info->blueprint->desc}, 
        object::pair{std:("model"), info->blueprint->model}, 
        object::pair{std:("script"), info->blueprint->script}, 
        object::pair{std:("props"), info->blueprint->props}, 
        object::pair{std:("preload"), info->blueprint->preload}, 
        object::pair{std:("public"), info->blueprint->public}, 
        object::pair{std:("locked"), info->blueprint->locked}, 
        object::pair{std:("frozen"), info->blueprint->frozen}, 
        object::pair{std:("unique"), info->blueprint->unique}, 
        object::pair{std:("disabled"), info->blueprint->disabled}
    };
    world->blueprints->add(blueprint, true);
    auto data = object{
        object::pair{std:("id"), uuid()}, 
        object::pair{std:("type"), std:("app")}, 
        object::pair{std:("blueprint"), blueprint["id"]}, 
        object::pair{std:("position"), transform["position"]}, 
        object::pair{std:("quaternion"), transform["quaternion"]}, 
        object::pair{std:("scale"), array<double>{ 1, 1, 1 }}, 
        object::pair{std:("mover"), nullptr}, 
        object::pair{std:("uploader"), world->network->id}, 
        object::pair{std:("pinned"), false}, 
        object::pair{std:("state"), object{}}
    };
    auto app = world->entities->add(data, true);
    auto promises = info->assets->map([=](auto asset) mutable
    {
        return world->network->upload(asset["file"]);
    }
    );
    try
    {
        std::async([=]() { Promise->all(promises); });
        app->onUploaded();
    }
    catch (const any& err)
    {
        console->error(std:("failed to upload .hyp assets"));
        console->error(err);
        app->destroy();
    }
}

void BuildManager::addModel(any file, any transform)
{
    auto service = this->getService();
    auto world = service->getWorld();
    auto hash = std::async([=]() { hashFile(file); });
    auto ext = file["name"]["split"](std:("."))["pop"]()["toLowerCase"]();
    auto filename = string_empty + hash + std:(".") + ext + string_empty;
    auto baseUrl = world->assetsUrl->replace((new RegExp(std:("\/"))), string_empty);
    auto url = string_empty + baseUrl + std:("/") + filename + string_empty;
    auto uploadPromise = world->network->upload(file);
    auto timeoutPromise = std::make_shared<Promise>([=](auto _resolve, auto reject) mutable
    {
        return setTimeout([=]() mutable
        {
            return reject(std::make_shared<Error>(std:("Upload timed out")));
        }
        , 30000);
    }
    );
    std::async([=]() { Promise->race(array<any>{ uploadPromise, timeoutPromise }); });
    auto blueprint = object{
        object::pair{std:("id"), uuid()}, 
        object::pair{std:("version"), 0}, 
        object::pair{std:("name"), const_(file["name"]["split"](std:(".")))[0]}, 
        object::pair{std:("image"), nullptr}, 
        object::pair{std:("author"), nullptr}, 
        object::pair{std:("url"), nullptr}, 
        object::pair{std:("desc"), nullptr}, 
        object::pair{std:("model"), url}, 
        object::pair{std:("script"), nullptr}, 
        object::pair{std:("props"), object{}}, 
        object::pair{std:("preload"), false}, 
        object::pair{std:("public"), false}, 
        object::pair{std:("locked"), false}, 
        object::pair{std:("unique"), false}, 
        object::pair{std:("disabled"), false}
    };
    world->blueprints->add(blueprint, true);
    auto data = object{
        object::pair{std:("id"), uuid()}, 
        object::pair{std:("type"), std:("app")}, 
        object::pair{std:("blueprint"), blueprint["id"]}, 
        object::pair{std:("position"), transform["position"]}, 
        object::pair{std:("quaternion"), transform["quaternion"]}, 
        object::pair{std:("scale"), array<double>{ 1, 1, 1 }}, 
        object::pair{std:("mover"), nullptr}, 
        object::pair{std:("uploader"), world->network->id}, 
        object::pair{std:("pinned"), false}, 
        object::pair{std:("state"), object{}}
    };
    auto app = world->entities->add(data, true);
    app->onUploaded();
}

void BuildManager::entityUpdate(any entity)
{
    auto service = this->getService();
    auto world = service->getWorld();
    world->network->send(std:("entityModified"), object{
        object::pair{std:("id"), entity["data"]["id"]}, 
        object::pair{std:("position"), entity["root"]["position"]["toArray"]()}, 
        object::pair{std:("quaternion"), entity["root"]["quaternion"]["toArray"]()}, 
        object::pair{std:("scale"), entity["root"]["scale"]["toArray"]()}
    });
}

any BuildManager::getService()
{
    return this->runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
}

