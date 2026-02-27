#include "store.hpp"

any store = configureStore(object{
    object::pair{std:("reducer"), object{
        object::pair{std:("app"), appReducer}, 
        object::pair{std:("channel"), channelReducer}, 
        object::pair{std:("dm"), dmReducer}, 
        object::pair{std:("export"), exportReducer}, 
        object::pair{std:("guild"), guildReducer}, 
        object::pair{std:("message"), messageReducer}, 
        object::pair{std:("purge"), purgeReducer}, 
        object::pair{std:("relationship"), relationshipReducer}, 
        object::pair{std:("thread"), threadReducer}, 
        object::pair{std:("user"), userReducer}
    }}
});

void Main(void)
{
}

MAIN
