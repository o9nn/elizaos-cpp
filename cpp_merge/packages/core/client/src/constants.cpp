#include "constants.hpp"

string USER_NAME = std:("user");
string CHAT_SOURCE = std:("client_chat");
string GROUP_CHAT_SOURCE = std:("client_group_chat");
double AVATAR_IMAGE_MAX_SIZE = 300;
object FIELD_REQUIREMENTS = object{
    object::pair{std:("name"), FIELD_REQUIREMENT_TYPE::REQUIRED}, 
    object::pair{std:("username"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std:("system"), FIELD_REQUIREMENT_TYPE::REQUIRED}, 
    object::pair{std:("settings.voice.model"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std:("bio"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std:("topics"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std:("adjectives"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std:("style.all"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std:("style.chat"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std:("style.post"), FIELD_REQUIREMENT_TYPE::OPTIONAL}
};

void Main(void)
{
}

MAIN
