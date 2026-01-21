#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/communityManager/plugins/communityManager/providers/timeout.h"

std::shared_ptr<Provider> timeoutUserProvider = object{
    object::pair{std::string("name"), std::string("TIMEOUT_USER_CONTEXT")}, 
    object::pair{std::string("description"), std::string("Prompt to decide if someone in the conversation should be timed out.")}, 
    object::pair{std::string("get"), [=]() mutable
    {
        auto instruction = std::string("Review the conversation carefully.\
\
If any user is:\
- Spreading FUD\
- Being toxic or disrespectful\
- Using violent, hateful, or abusive language\
- Clearly violating community guidelines\
\
Then you **must** select the TIMEOUT_USER action and return the username of the person who should be timed out.\
\
only return the action if:\
- You are confident someone broke the rules, **and**\
- That person has **not already been punished**.\
");
        return object{
            object::pair{std::string("data"), object{}}, 
            object::pair{std::string("values"), object{
                object::pair{std::string("moderation"), addHeader(std::string("# Timeout Decision Instructions"), instruction)}
            }}, 
            object::pair{std::string("text"), addHeader(std::string("# Timeout Decision Instructions"), instruction)}
        };
    }
    }
};

void Main(void)
{
}

MAIN
