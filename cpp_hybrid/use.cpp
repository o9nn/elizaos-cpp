#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/actions/use.h"

string useItemTemplate = std::string("\
# Task: Decide if the agent should interact with an entity (e.g. pick up or activate) based on recent context.\
# DO NOT assume the last message has a command. Look at overall context.\
# ONLY return entity IDs that exist in the Hyperfy World State.\
\
{{providers}}\
\
# Instructions:\
Decide if the agent should use/interact with a specific entity based on the conversation and world state.\
\
Response format:\
```json\
{\
  "entityId": "<string>" // or null if none\
}\
```\
");
std::shared_ptr<Action> hyperfyUseItemAction = object{
    object::pair{std::string("name"), std::string("HYPERFY_USE_ITEM")}, 
    object::pair{std::string("similes"), array<string>{ std::string("INTERACT_WITH_ITEM"), std::string("USE_NEARBY_OBJECT"), std::string("PICK_UP_ITEM") }}, 
    object::pair{std::string("description"), std::string("Walks to and interacts with a nearby usable item (like picking it up); use when a player asks you to use or grab something.")}, 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        return AND((AND((AND((!!service), (service->isConnected()))), (!!world->controls))), (!!world->actions));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state, auto options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        auto controls = as<std::shared_ptr<AgentControls>>(world->controls);
        auto actions = as<any>(world->actions);
        if (OR((OR((!service), (!world))), (!actions))) {
            logger->error(std::string("Hyperfy service, world, or actions not found for HYPERFY_USE_ITEM action."));
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("Error: Cannot use item. Agent action system unavailable.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        auto targetEntityId = options["entityId"];
        if (!targetEntityId) {
            logger->info(std::string("[USE ITEM] No entityId provided, attempting LLM extraction..."));
            try
            {
                auto useState = std::async([=]() { runtime->composeState(message, array<string>{ std::string("HYPERFY_WORLD_STATE"), std::string("RECENT_MESSAGES") }, true); });
                auto prompt = composePromptFromState(object{
                    object::pair{std::string("state"), useState}, 
                    object::pair{std::string("template"), useItemTemplate}
                });
                auto response = std::async([=]() { runtime->useModel(ModelType->OBJECT_SMALL, object{
                    object::pair{std::string("prompt"), std::string("prompt")}
                }); });
                if (AND((response->entityId), (type_of(response->entityId) == std::string("string")))) {
                    targetEntityId = response->entityId;
                    logger->info(std::string("[USE ITEM] Extracted entity ID: ") + targetEntityId + string_empty);
                } else {
                    logger->warn(std::string("[USE ITEM] No valid entityId extracted."));
                }
            }
            catch (const any& err)
            {
                logger->error(std::string("[USE ITEM] Extraction failed:"), err);
            }
        }
        if (!targetEntityId) {
            logger->warn(std::string("[USE ITEM] No suitable item found to use based on the context."));
            return std::shared_ptr<Promise<void>>();
        }
        auto entity = world->entities->items->get(targetEntityId);
        auto targetPosition = entity->root->position;
        if (!targetPosition) {
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("Could not locate entity ") + targetEntityId + std::string(".")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("error"), std::string("entity_not_found")}
                }}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        std::async([=]() { controls->goto(targetPosition->x, targetPosition->z); });
        logger->info(std::string("[USE ITEM] Attempting to use item with entity ID: ") + targetEntityId + string_empty);
        actions->performAction(targetEntityId);
        std::async([=]() { callback(object{
            object::pair{std::string("text"), std::string("Using item: ") + targetEntityId + string_empty}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_USE_ITEM") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("targetEntityId"), std::string("targetEntityId")}, 
                object::pair{std::string("status"), std::string("triggered")}
            }}
        }); });
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Pick up the book.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Using item: book123")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_USE_ITEM") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Interact with the glowing orb.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Using item: orb888")}, 
            object::pair{std::string("actions"), array<string>{ std::string("HYPERFY_USE_ITEM") }}, 
            object::pair{std::string("source"), std::string("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{name1}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Do we need to pick something up?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{name2}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("No suitable item found to use based on the context.")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
