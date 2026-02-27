#include "index.hpp"

string HYPERFY_WS_URL = OR((process->env->WS_URL), (std:("wss://chill.hyperfy.xyz/ws")));
any hyperfyPluginConfigSchema = z->object(object{
    object::pair{std:("DEFAULT_HYPERFY_WS_URL"), z->string()->url()->optional()}
});
std::shared_ptr<Plugin> hyperfyPlugin = object{
    object::pair{std:("name"), std:("hyperfy")}, 
    object::pair{std:("description"), std:("Integrates ElizaOS agents with Hyperfy worlds")}, 
    object::pair{std:("config"), object{
        object::pair{std:("DEFAULT_HYPERFY_WS_URL"), HYPERFY_WS_URL}
    }}, 
    , 
    object::pair{std:("services"), array<HyperfyService>{ HyperfyService }}, 
    object::pair{std:("events"), hyperfyEvents}, 
    object::pair{std:("actions"), array<any>{ hyperfyScenePerceptionAction, hyperfyGotoEntityAction, hyperfyUseItemAction, hyperfyUnuseItemAction, hyperfyStopMovingAction, hyperfyWalkRandomlyAction, hyperfyAmbientSpeechAction, hyperfyEditEntityAction, replyAction, ignoreAction }}, 
    object::pair{std:("providers"), array<any>{ hyperfyProvider, hyperfyEmoteProvider, hyperfyActionsProvider, characterProvider }}
};

void Main(void)
{
}

MAIN
