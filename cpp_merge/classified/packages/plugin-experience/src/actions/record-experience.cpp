#include "record-experience.h"

std::shared_ptr<Action> recordExperienceAction = object{
    object::pair{std:("name"), std:("RECORD_EXPERIENCE")}, 
    object::pair{std:("description"), std:("Manually record a learning experience")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("User")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Remember that installing dependencies is required for Python scripts")}
        }}
    }, object{
        object::pair{std:("name"), std:("Agent")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll record that experience. Learning: Need to install dependencies before running Python scripts.")}, 
            object::pair{std:("action"), std:("RECORD_EXPERIENCE")}
        }}
    } } }}, 
    , 
};

void Main(void)
{
}

MAIN
