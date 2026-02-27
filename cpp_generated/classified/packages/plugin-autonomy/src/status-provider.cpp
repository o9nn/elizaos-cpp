#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autonomy/src/status-provider.h"

std::shared_ptr<Provider> autonomyStatusProvider = object{
    object::pair{std::string("name"), std::string("AUTONOMY_STATUS")}, 
    object::pair{std::string("description"), std::string("Provides current autonomy status for agent awareness in conversations")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto autonomyService = runtime->getService(std::string("autonomy"));
        if (!autonomyService) {
            return object{
                object::pair{std::string("text"), string_empty}
            };
        }
        auto autonomousRoomId = (as<any>(autonomyService))["getAutonomousRoomId"]();
        if (AND((autonomousRoomId), (message->roomId == autonomousRoomId))) {
            return object{
                object::pair{std::string("text"), string_empty}
            };
        }
        auto autonomyEnabled = runtime->getSetting(std::string("AUTONOMY_ENABLED"));
        auto serviceRunning = OR(((as<any>(autonomyService))["isLoopRunning"]()), (false));
        auto interval = OR(((as<any>(autonomyService))["getLoopInterval"]()), (30000));
        string status;
        string statusIcon;
        if (serviceRunning) {
            status = std::string("running autonomously");
            statusIcon = std::string("🤖");
        } else if (autonomyEnabled) {
            status = std::string("autonomy enabled but not running");
            statusIcon = std::string("⏸️");
        } else {
            status = std::string("autonomy disabled");
            statusIcon = std::string("🔕");
        }
        auto intervalText = Math->round(interval / 1000);
        auto intervalUnit = (intervalText < 60) ? any(std::string("seconds")) : any(string_empty + Math->round(intervalText / 60) + std::string(" minutes"));
        return object{
            object::pair{std::string("text"), std::string("[AUTONOMY_STATUS]\
Current status: ") + statusIcon + std::string(" ") + status + std::string("\
Thinking interval: ") + (intervalText < 60) ? string_empty + intervalText + std::string(" seconds") : intervalUnit + std::string("\
[/AUTONOMY_STATUS]")}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("autonomyEnabled"), !!autonomyEnabled}, 
                object::pair{std::string("serviceRunning"), std::string("serviceRunning")}, 
                object::pair{std::string("interval"), std::string("interval")}, 
                object::pair{std::string("intervalSeconds"), intervalText}, 
                object::pair{std::string("status"), (serviceRunning) ? any(std::string("running")) : any((autonomyEnabled) ? std::string("enabled") : std::string("disabled"))}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
