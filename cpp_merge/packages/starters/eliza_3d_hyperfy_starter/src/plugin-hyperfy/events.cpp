#include "events.hpp"

object hyperfyEvents = object{
    object::pair{hyperfyEventType::MESSAGE_RECEIVED, array<std::function<std::shared_ptr<Promise<void>>(any)>>{ [=](auto payload) mutable
    {
        std::async([=]() { messageReceivedHandler(object{
            object::pair{std:("runtime"), payload->runtime}, 
            object::pair{std:("message"), payload->message}, 
            object::pair{std:("callback"), payload->callback}, 
            object::pair{std:("onComplete"), payload->onComplete}
        }); });
    }
     }}, 
    object::pair{hyperfyEventType::VOICE_MESSAGE_RECEIVED, array<std::function<std::shared_ptr<Promise<void>>(any)>>{ [=](auto payload) mutable
    {
        std::async([=]() { messageReceivedHandler(object{
            object::pair{std:("runtime"), payload->runtime}, 
            object::pair{std:("message"), payload->message}, 
            object::pair{std:("callback"), payload->callback}, 
            object::pair{std:("onComplete"), payload->onComplete}
        }); });
    }
     }}, 
    object::pair{std:("CONTROL_MESSAGE"), array<any>()}
};

void Main(void)
{
}

MAIN
