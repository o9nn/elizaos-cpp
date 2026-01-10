#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/events.h"

object hyperfyEvents = object{
    object::pair{hyperfyEventType::MESSAGE_RECEIVED, array<std::function<std::shared_ptr<Promise<void>>(any)>>{ [=](auto payload) mutable
    {
        std::async([=]() { messageReceivedHandler(object{
            object::pair{std::string("runtime"), payload->runtime}, 
            object::pair{std::string("message"), payload->message}, 
            object::pair{std::string("callback"), payload->callback}, 
            object::pair{std::string("onComplete"), payload->onComplete}
        }); });
    }
     }}, 
    object::pair{hyperfyEventType::VOICE_MESSAGE_RECEIVED, array<std::function<std::shared_ptr<Promise<void>>(any)>>{ [=](auto payload) mutable
    {
        std::async([=]() { messageReceivedHandler(object{
            object::pair{std::string("runtime"), payload->runtime}, 
            object::pair{std::string("message"), payload->message}, 
            object::pair{std::string("callback"), payload->callback}, 
            object::pair{std::string("onComplete"), payload->onComplete}
        }); });
    }
     }}, 
    object::pair{std::string("CONTROL_MESSAGE"), array<any>()}
};

void Main(void)
{
}

MAIN
