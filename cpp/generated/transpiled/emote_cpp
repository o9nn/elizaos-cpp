#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/providers/emote.h"

std::shared_ptr<Provider> hyperfyEmoteProvider = object{
    object::pair{std::string("name"), std::string("HYPERFY_EMOTE_LIST")}, 
    object::pair{std::string("description"), std::string("Lists all available emotes and their descriptions")}, 
    object::pair{std::string("get"), [=](auto _runtime, auto _message) mutable
    {
        auto animationListText = EMOTES_LIST->map([=](auto e) mutable
        {
            return std::string("- **") + e["name"] + std::string("**: ") + e["description"] + string_empty;
        }
        )->join(std::string("\
"));
        auto animationText = std::string("## Available Animations\
") + animationListText + string_empty;
        return object{
            object::pair{std::string("data"), object{
                object::pair{std::string("emotes"), EMOTES_LIST}
            }}, 
            object::pair{std::string("values"), object{
                object::pair{std::string("hyperfyAnimations"), animationText}
            }}, 
            object::pair{std::string("text"), animationText}
        };
    }
    }
};

void Main(void)
{
}

MAIN
