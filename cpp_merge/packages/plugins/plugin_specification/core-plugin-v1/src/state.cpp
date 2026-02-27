#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/state.h"

State fromV2State(std::shared_ptr<StateV2> stateV2)
{
    auto state = utils::assign(object{
        , 
        object::pair{std::string("text"), stateV2->text}
    }, DEFAULT_STATE, stateV2->values, stateV2->data);
    for (auto& key : keys_(stateV2))
    {
        if (AND((AND((key != std::string("values")), (key != std::string("data")))), (key != std::string("text")))) {
            state[key] = const_(stateV2)[key];
        }
    }
    return state;
};


std::shared_ptr<StateV2> toV2State(State state)
{
    auto stateV2 = object{
        object::pair{std::string("values"), object{}}, 
        object::pair{std::string("data"), object{}}, 
        object::pair{std::string("text"), OR((state->text), (string_empty))}
    };
    for (auto& key : keys_(state))
    {
        if (key != std::string("text")) {
            stateV2[key] = const_(state)[key];
        }
    }
    return stateV2;
};


Partial<State> DEFAULT_STATE = object{
    object::pair{std::string("bio"), string_empty}, 
    object::pair{std::string("lore"), string_empty}, 
    object::pair{std::string("messageDirections"), string_empty}, 
    object::pair{std::string("postDirections"), string_empty}, 
    object::pair{std::string("actors"), string_empty}, 
    object::pair{std::string("recentMessages"), string_empty}, 
    object::pair{std::string("recentMessagesData"), array<any>()}
};

void Main(void)
{
}

MAIN
