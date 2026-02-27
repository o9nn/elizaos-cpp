#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/providers/emote.h"

std::shared_ptr<Provider> hyperfyEmoteProvider = object{
    object::pair{std:("name"), std:("HYPERFY_EMOTE_LIST")}, 
    object::pair{std:("description"), std:("Lists all available emotes and their descriptions")}, 
    object::pair{std:("get"), [=](auto _runtime, auto _message) mutable
    {
        auto animationListText = EMOTES_LIST->map([=](auto e) mutable
        {
            return std:("- **") + e["name"] + std:("**: ") + e["description"] + string_empty;
        }
        )->join(std:("\
"));
        auto animationText = std:("## Available Animations\
") + animationListText + string_empty;
        return object{
            object::pair{std:("data"), object{
                object::pair{std:("emotes"), EMOTES_LIST}
            }}, 
            object::pair{std:("values"), object{
                object::pair{std:("hyperfyAnimations"), animationText}
            }}, 
            object::pair{std:("text"), animationText}
        };
    }
    }
};

void Main(void)
{
}

MAIN
