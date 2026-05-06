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
                object::pair{std::string("id"), uuid()}, 
                object::pair{std::string("version"), 0}, 
                object::pair{std::string("name"), entity->blueprint->name}, 
                object::pair{std::string("image"), entity->blueprint->image}, 
                object::pair{std::string("author"), entity->blueprint->author}, 
                object::pair{std::string("url"), entity->blueprint->url}, 
                object::pair{std::string("desc"), entity->blueprint->desc}, 
                object::pair{std::string("model"), entity->blueprint->model}, 
                object::pair{std::string("script"), entity->blueprint->script}, 
                object::pair{std::string("props"), cloneDeep(entity->blueprint->props)}, 
                object::pair{std::string("preload"), entity->blueprint->preload}, 
                object::pair{std::string("public"), entity->blueprint->public}, 
                object::pair{std::string("locked"), entity->blueprint->locked}, 
                object::pair{std::string("frozen"), entity->blueprint->frozen}, 
                object::pair{std::string("unique"), entity->blueprint->unique}, 
                object::pair{std::string("disabled"), entity->blueprint->disabled}
            };
            world->blueprints->add(blueprint, true);
            blueprintId = blueprint["id"];
        }
        auto data = object{
            object::pair{std::string("id"), uuid()}, 
            object::pair{std::string("type"), std::string("app")}, 
            object::pair{std::string("blueprint"), blueprintId}, 
            object::pair{std::string("position"), entity->root->position->toArray()}, 
            object::pair{std::string("quaternion"), entity->root->quaternion->toArray()}, 
            object::pair{std::string("scale"), entity->root->scale->toArray()}, 
            object::pair{std::string("mover"), nullptr}, 
            object::pair{std::string("uploader"), nullptr}, 
            object::pair{std::string("pinned"), false}, 
            object::pair{std::string("state"), object{}}
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
    file = std::make_shared<File>(array<std::shared_ptr<Blob>>{ blob }, url->split(std::string("/"))->pop(), object{
        object::pair{std::string("type"), resp->headers->get(std::string("content-type"))}
    });
    if (!file) return std::shared_ptr<Promise<void>>();
    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
    {
        return setTimeout(resolve, 100);
    }
    ); });
    auto maxSize = world->network->maxUploadSize * 1024 * 1024;
    if (file["size"] > maxSize) {
        console->error(std::string("File too large. Maximum size is ") + (maxSize / (1024 * 1024)) + std::string("MB"));
        return std::shared_ptr<Promise<void>>();
    }
    auto validVec3 = [=](auto v) mutable
    {
        return AND((AND((Array->isArray(v)), (v["length"] == 3))), (v["every"]([=](auto n) mutable
        {
            return type_of(n) == std::string("number");
        }
        )));
    };
    auto validQuat = [=](auto q) mutable
    {
        return AND((AND((Array->isArray(q)), (q["length"] == 4))), (q["every"]([=](auto n) mutable
        {
            return type_of(n) == std::string("number");
        }
        )));
    };
    position = (validVec3(position)) ? any(position) : any(array<double>{ 0, 0, 0 });
    quaternion = (validQuat(quaternion)) ? any(quaternion) : any(array<double>{ 0, 0, 0, 1 });
    auto controls = world->controls;
    if (controls) {
        std::async([=]() { controls->goto(const_(position)[0], const_(position)[2]); });
    }
    auto transform = object{
        object::pair{std::string("position"), std::string("position")}, 
        object::pair{std::string("quaternion"), std::string("quaternion")}
    };
    auto ext = file["name"]["split"](std::string("."))["pop"]()["toLowerCase"]();
    if (ext == std::string("hyp")) {
        this->addApp(file, transform);
    }
    if (OR((ext == std::string("glb")), (ext == std::string("vrm")))) {
        this->addModel(file, transform);
    }
}

void BuildManager::addApp(any file, any transform)
{
    auto service = this->getService();
    shared world = service->getWorld();
    auto info = std::async([=]() { importApp(file); });
    auto blueprint = object{
        object::pair{std::string("id"), uuid()}, 
        object::pair{std::string("version"), 0}, 
        object::pair{std::string("name"), info->blueprint->name}, 
        object::pair{std::string("image"), info->blueprint->image}, 
        object::pair{std::string("author"), info->blueprint->author}, 
        object::pair{std::string("url"), info->blueprint->url}, 
        object::pair{std::string("desc"), info->blueprint->desc}, 
        object::pair{std::string("model"), info->blueprint->model}, 
        object::pair{std::string("script"), info->blueprint->script}, 
        object::pair{std::string("props"), info->blueprint->props}, 
        object::pair{std::string("preload"), info->blueprint->preload}, 
        object::pair{std::string("public"), info->blueprint->public}, 
        object::pair{std::string("locked"), info->blueprint->locked}, 
        object::pair{std::string("frozen"), info->blueprint->frozen}, 
        object::pair{std::string("unique"), info->blueprint->unique}, 
        object::pair{std::string("disabled"), info->blueprint->disabled}
    };
    world->blueprints->add(blueprint, true);
    auto data = object{
        object::pair{std::string("id"), uuid()}, 
        object::pair{std::string("type"), std::string("app")}, 
        object::pair{std::string("blueprint"), blueprint["id"]}, 
        object::pair{std::string("position"), transform["position"]}, 
        object::pair{std::string("quaternion"), transform["quaternion"]}, 
        object::pair{std::string("scale"), array<double>{ 1, 1, 1 }}, 
        object::pair{std::string("mover"), nullptr}, 
        object::pair{std::string("uploader"), world->network->id}, 
        object::pair{std::string("pinned"), false}, 
        object::pair{std::string("state"), object{}}
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
        console->error(std::string("failed to upload .hyp assets"));
        console->error(err);
        app->destroy();
    }
}

void BuildManager::addModel(any file, any transform)
{
    auto service = this->getService();
    auto world = service->getWorld();
    auto hash = std::async([=]() { hashFile(file); });
    auto ext = file["name"]["split"](std::string("."))["pop"]()["toLowerCase"]();
    auto filename = string_empty + hash + std::string(".") + ext + string_empty;
    auto baseUrl = world->assetsUrl->replace((new RegExp(std::string("\/"))), string_empty);
    auto url = string_empty + baseUrl + std::string("/") + filename + string_empty;
    auto uploadPromise = world->network->upload(file);
    auto timeoutPromise = std::make_shared<Promise>([=](auto _resolve, auto reject) mutable
    {
        return setTimeout([=]() mutable
        {
            return reject(std::make_shared<Error>(std::string("Upload timed out")));
        }
        , 30000);
    }
    );
    std::async([=]() { Promise->race(array<any>{ uploadPromise, timeoutPromise }); });
    auto blueprint = object{
        object::pair{std::string("id"), uuid()}, 
        object::pair{std::string("version"), 0}, 
        object::pair{std::string("name"), const_(file["name"]["split"](std::string(".")))[0]}, 
        object::pair{std::string("image"), nullptr}, 
        object::pair{std::string("author"), nullptr}, 
        object::pair{std::string("url"), nullptr}, 
        object::pair{std::string("desc"), nullptr}, 
        object::pair{std::string("model"), url}, 
        object::pair{std::string("script"), nullptr}, 
        object::pair{std::string("props"), object{}}, 
        object::pair{std::string("preload"), false}, 
        object::pair{std::string("public"), false}, 
        object::pair{std::string("locked"), false}, 
        object::pair{std::string("unique"), false}, 
        object::pair{std::string("disabled"), false}
    };
    world->blueprints->add(blueprint, true);
    auto data = object{
        object::pair{std::string("id"), uuid()}, 
        object::pair{std::string("type"), std::string("app")}, 
        object::pair{std::string("blueprint"), blueprint["id"]}, 
        object::pair{std::string("position"), transform["position"]}, 
        object::pair{std::string("quaternion"), transform["quaternion"]}, 
        object::pair{std::string("scale"), array<double>{ 1, 1, 1 }}, 
        object::pair{std::string("mover"), nullptr}, 
        object::pair{std::string("uploader"), world->network->id}, 
        object::pair{std::string("pinned"), false}, 
        object::pair{std::string("state"), object{}}
    };
    auto app = world->entities->add(data, true);
    app->onUploaded();
}

void BuildManager::entityUpdate(any entity)
{
    auto service = this->getService();
    auto world = service->getWorld();
    world->network->send(std::string("entityModified"), object{
        object::pair{std::string("id"), entity["data"]["id"]}, 
        object::pair{std::string("position"), entity["root"]["position"]["toArray"]()}, 
        object::pair{std::string("quaternion"), entity["root"]["quaternion"]["toArray"]()}, 
        object::pair{std::string("scale"), entity["root"]["scale"]["toArray"]()}
    });
}

any BuildManager::getService()
{
    return this->runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
}

