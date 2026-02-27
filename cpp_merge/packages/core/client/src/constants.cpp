#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/client/src/constants.h"

string USER_NAME = std::string("user");
string CHAT_SOURCE = std::string("client_chat");
string GROUP_CHAT_SOURCE = std::string("client_group_chat");
double AVATAR_IMAGE_MAX_SIZE = 300;
object FIELD_REQUIREMENTS = object{
    object::pair{std::string("name"), FIELD_REQUIREMENT_TYPE::REQUIRED}, 
    object::pair{std::string("username"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std::string("system"), FIELD_REQUIREMENT_TYPE::REQUIRED}, 
    object::pair{std::string("settings.voice.model"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std::string("bio"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std::string("topics"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std::string("adjectives"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std::string("style.all"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std::string("style.chat"), FIELD_REQUIREMENT_TYPE::OPTIONAL}, 
    object::pair{std::string("style.post"), FIELD_REQUIREMENT_TYPE::OPTIONAL}
};

void Main(void)
{
}

MAIN
