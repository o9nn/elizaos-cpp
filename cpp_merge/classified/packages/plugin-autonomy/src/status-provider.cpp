#include "status-provider.h"

std::shared_ptr<Provider> autonomyStatusProvider = object{
    object::pair{std:("name"), std:("AUTONOMY_STATUS")}, 
    object::pair{std:("description"), std:("Provides current autonomy status for agent awareness in conversations")}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto autonomyService = runtime->getService(std:("autonomy"));
        if (!autonomyService) {
            return object{
                object::pair{std:("text"), string_empty}
            };
        }
        auto autonomousRoomId = (as<any>(autonomyService))["getAutonomousRoomId"]();
        if (AND((autonomousRoomId), (message->roomId == autonomousRoomId))) {
            return object{
                object::pair{std:("text"), string_empty}
            };
        }
        auto autonomyEnabled = runtime->getSetting(std:("AUTONOMY_ENABLED"));
        auto serviceRunning = OR(((as<any>(autonomyService))["isLoopRunning"]()), (false));
        auto interval = OR(((as<any>(autonomyService))["getLoopInterval"]()), (30000));
        string status;
        string statusIcon;
        if (serviceRunning) {
            status = std:("running autonomously");
            statusIcon = std:("🤖");
        } else if (autonomyEnabled) {
            status = std:("autonomy enabled but not running");
            statusIcon = std:("⏸️");
        } else {
            status = std:("autonomy disabled");
            statusIcon = std:("🔕");
        }
        auto intervalText = Math->round(interval / 1000);
        auto intervalUnit = (intervalText < 60) ? any(std:("seconds")) (string_empty + Math->round(intervalText / 60) + std:(" minutes"));
        return object{
            object::pair{std:("text"), std:("[AUTONOMY_STATUS]\
Current status: ") + statusIcon + std:(" ") + status + std:("\
Thinking interval: ") + (intervalText < 60) ? string_empty + intervalText + std:(" seconds") : intervalUnit + std:("\
[/AUTONOMY_STATUS]")}, 
            object::pair{std:("data"), object{
                object::pair{std:("autonomyEnabled"), !!autonomyEnabled}, 
                object::pair{std:("serviceRunning"), std:("serviceRunning")}, 
                object::pair{std:("interval"), std:("interval")}, 
                object::pair{std:("intervalSeconds"), intervalText}, 
                object::pair{std:("status"), (serviceRunning) ? any(std:("running")) ((autonomyEnabled) ? std:("enabled") : std:("disabled"))}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
