#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/index.h"

string HYPERFY_WS_URL = OR((process->env->WS_URL), (std::string("wss://chill.hyperfy.xyz/ws")));
any hyperfyPluginConfigSchema = z->object(object{
    object::pair{std::string("DEFAULT_HYPERFY_WS_URL"), z->string()->url()->optional()}
});
std::shared_ptr<Plugin> hyperfyPlugin = object{
    object::pair{std::string("name"), std::string("hyperfy")}, 
    object::pair{std::string("description"), std::string("Integrates ElizaOS agents with Hyperfy worlds")}, 
    object::pair{std::string("config"), object{
        object::pair{std::string("DEFAULT_HYPERFY_WS_URL"), HYPERFY_WS_URL}
    }}, 
    , 
    object::pair{std::string("services"), array<HyperfyService>{ HyperfyService }}, 
    object::pair{std::string("events"), hyperfyEvents}, 
    object::pair{std::string("actions"), array<any>{ hyperfyScenePerceptionAction, hyperfyGotoEntityAction, hyperfyUseItemAction, hyperfyUnuseItemAction, hyperfyStopMovingAction, hyperfyWalkRandomlyAction, hyperfyAmbientSpeechAction, hyperfyEditEntityAction, replyAction, ignoreAction }}, 
    object::pair{std::string("providers"), array<any>{ hyperfyProvider, hyperfyEmoteProvider, hyperfyActionsProvider, characterProvider }}
};

void Main(void)
{
}

MAIN
