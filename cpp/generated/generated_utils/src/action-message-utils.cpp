#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/frontend/lib/action-message-utils.h"

std::shared_ptr<ToolPart> convertActionMessageToToolPart(any message)
{
    auto rawMessage = OR((OR((message["rawMessage"]), (message["metadata"]))), (object{}));
    auto mapActionStatusToState = [=](auto status) mutable
    {
        static switch_type __switch534_862 = {
            { any(std::string("pending")), 1 },
            { any(std::string("executing")), 2 },
            { any(std::string("running")), 3 },
            { any(std::string("completed")), 4 },
            { any(std::string("success")), 5 },
            { any(std::string("failed")), 6 },
            { any(std::string("error")), 7 }
        };
        switch (__switch534_862[status])
        {
        case 1:
        case 2:
        case 3:
            return std::string("input-streaming");
        case 4:
        case 5:
            return std::string("output-available");
        case 6:
        case 7:
            return std::string("output-error");
        default:
            return std::string("input-available");
        }
    };
    auto actionName = OR((OR((const_(rawMessage["actions"])[0]), (rawMessage["action"]))), (std::string("ACTION")));
    auto actionStatus = OR((rawMessage["actionStatus"]), (std::string("completed")));
    auto actionId = rawMessage["actionId"];
    auto inputData = object{};
    inputData->input = OR((rawMessage["actionResult"]["input"]), (object{}));
    auto outputData = object{};
    if (OR((rawMessage["text"]), (message["content"]))) {
        outputData->text = OR((rawMessage["text"]), (message["content"]));
    }
    if (actionStatus) outputData->status = actionStatus;
    if (rawMessage["actionResult"]) outputData->result = rawMessage["actionResult"];
    auto isError = OR((actionStatus == std::string("failed")), (actionStatus == std::string("error")));
    auto errorText = (isError) ? any(OR((OR((rawMessage["text"]), (message["content"]))), (std::string("Action failed")))) : any(undefined);
    return object{
        object::pair{std::string("type"), actionName}, 
        object::pair{std::string("state"), mapActionStatusToState(actionStatus)}, 
        object::pair{std::string("toolCallId"), actionId}, 
        object::pair{std::string("input"), (Object->keys(inputData)->get_length() > 0) ? any(inputData) : any(undefined)}, 
        object::pair{std::string("output"), (Object->keys(outputData)->get_length() > 0) ? any(outputData) : any(undefined)}, 
        object::pair{std::string("errorText"), std::string("errorText")}
    };
};


boolean isActionMessage(any message)
{
    return (OR((OR((message["sourceType"] == std::string("agent_action")), (message["metadata"]["sourceType"] == std::string("agent_action")))), (message["type"] == std::string("agent_action"))));
};


