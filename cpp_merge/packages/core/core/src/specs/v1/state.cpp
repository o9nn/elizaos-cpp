#include "state.hpp"

State fromV2State(std::shared_ptr<StateV2> stateV2)
{
    auto state = utils::assign(object{
        , 
        object::pair{std:("text"), stateV2->text}
    }, DEFAULT_STATE, stateV2->values, stateV2->data);
    for (auto& key : keys_(stateV2))
    {
        if (AND((AND((key != std:("values")), (key != std:("data")))), (key != std:("text")))) {
            state[key] = const_(stateV2)[key];
        }
    }
    return state;
};


std::shared_ptr<StateV2> toV2State(State state)
{
    auto stateV2 = object{
        object::pair{std:("values"), object{}}, 
        object::pair{std:("data"), object{}}, 
        object::pair{std:("text"), OR((state->text), (string_empty))}
    };
    for (auto& key : keys_(state))
    {
        if (key != std:("text")) {
            stateV2[key] = const_(state)[key];
        }
    }
    return stateV2;
};


Partial<State> DEFAULT_STATE = object{
    object::pair{std:("bio"), string_empty}, 
    object::pair{std:("lore"), string_empty}, 
    object::pair{std:("messageDirections"), string_empty}, 
    object::pair{std:("postDirections"), string_empty}, 
    object::pair{std:("actors"), string_empty}, 
    object::pair{std:("recentMessages"), string_empty}, 
    object::pair{std:("recentMessagesData"), array<any>()}
};

void Main(void)
{
}

MAIN
