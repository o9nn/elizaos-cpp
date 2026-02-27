#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/communityManager/plugins/communityManager/providers/timeout.h"

std::shared_ptr<Provider> timeoutUserProvider = object{
    object::pair{std:("name"), std:("TIMEOUT_USER_CONTEXT")}, 
    object::pair{std:("description"), std:("Prompt to decide if someone in the conversation should be timed out.")}, 
    object::pair{std:("get"), [=]() mutable
    {
        auto instruction = std:("Review the conversation carefully.\
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
            object::pair{std:("data"), object{}}, 
            object::pair{std:("values"), object{
                object::pair{std:("moderation"), addHeader(std:("# Timeout Decision Instructions"), instruction)}
            }}, 
            object::pair{std:("text"), addHeader(std:("# Timeout Decision Instructions"), instruction)}
        };
    }
    }
};

void Main(void)
{
}

MAIN
