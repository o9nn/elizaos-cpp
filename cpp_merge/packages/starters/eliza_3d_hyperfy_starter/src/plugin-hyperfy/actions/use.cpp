#include "use.hpp"

string useItemTemplate = std:("\
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
"""json\
{\
  "entityId": "<string>" // or null if none\
}\
"""\
");
std::shared_ptr<Action> hyperfyUseItemAction = object{
    object::pair{std:("name"), std:("HYPERFY_USE_ITEM")}, 
    object::pair{std:("similes"), array<string>{ std:("INTERACT_WITH_ITEM"), std:("USE_NEARBY_OBJECT"), std:("PICK_UP_ITEM") }}, 
    object::pair{std:("description"), std:("Walks to and interacts with a nearby usable item (like picking it up); use when a player asks you to use or grab something.")}, 
    object::pair{std:("validate"), [=](auto runtime) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        return AND((AND((AND((!!service), (service->isConnected()))), (!!world->controls))), (!!world->actions));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state, auto options, auto callback) mutable
    {
        auto service = runtime->getService<std::shared_ptr<HyperfyService>>(HyperfyService::serviceType);
        auto world = service->getWorld();
        auto controls = as<std::shared_ptr<AgentControls>>(world->controls);
        auto actions = as<any>(world->actions);
        if (OR((OR((!service), (!world))), (!actions))) {
            logger->error(std:("Hyperfy service, world, or actions not found for HYPERFY_USE_ITEM action."));
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("Error: Cannot use item. Agent action system unavailable.")}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        auto targetEntityId = options["entityId"];
        if (!targetEntityId) {
            logger->info(std:("[USE ITEM] No entityId provided, attempting LLM extraction..."));
            try
            {
                auto useState = std::async([=]() { runtime->composeState(message, array<string>{ std:("HYPERFY_WORLD_STATE"), std:("RECENT_MESSAGES") }, true); });
                auto prompt = composePromptFromState(object{
                    object::pair{std:("state"), useState}, 
                    object::pair{std:("template"), useItemTemplate}
                });
                auto response = std::async([=]() { runtime->useModel(ModelType->OBJECT_SMALL, object{
                    object::pair{std:("prompt"), std:("prompt")}
                }); });
                if (AND((response->entityId), (type_of(response->entityId) == std:("string")))) {
                    targetEntityId = response->entityId;
                    logger->info(std:("[USE ITEM] Extracted entity ID: ") + targetEntityId + string_empty);
                } else {
                    logger->warn(std:("[USE ITEM] No valid entityId extracted."));
                }
            }
            catch (const any& err)
            {
                logger->error(std:("[USE ITEM] Extraction failed:"), err);
            }
        }
        if (!targetEntityId) {
            logger->warn(std:("[USE ITEM] No suitable item found to use based on the context."));
            return std::shared_ptr<Promise<void>>();
        }
        auto entity = world->entities->items->get(targetEntityId);
        auto targetPosition = entity->root->position;
        if (!targetPosition) {
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("Could not locate entity ") + targetEntityId + std:(".")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("error"), std:("entity_not_found")}
                }}
            }); });
            return std::shared_ptr<Promise<void>>();
        }
        std::async([=]() { controls->goto(targetPosition->x, targetPosition->z); });
        logger->info(std:("[USE ITEM] Attempting to use item with entity ID: ") + targetEntityId + string_empty);
        actions->performAction(targetEntityId);
        std::async([=]() { callback(object{
            object::pair{std:("text"), std:("Using item: ") + targetEntityId + string_empty}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_USE_ITEM") }}, 
            object::pair{std:("source"), std:("hyperfy")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("targetEntityId"), std:("targetEntityId")}, 
                object::pair{std:("status"), std:("triggered")}
            }}
        }); });
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Pick up the book.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Using item: book123")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_USE_ITEM") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Interact with the glowing orb.")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Using item: orb888")}, 
            object::pair{std:("actions"), array<string>{ std:("HYPERFY_USE_ITEM") }}, 
            object::pair{std:("source"), std:("hyperfy")}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("{{name1}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Do we need to pick something up?")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{name2}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("No suitable item found to use based on the context.")}
        }}
    } } }}
};

void Main(void)
{
}

MAIN
