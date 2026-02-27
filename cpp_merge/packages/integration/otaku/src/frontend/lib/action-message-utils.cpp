#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/frontend/lib/action-message-utils.h"

std::shared_ptr<ToolPart> convertActionMessageToToolPart(any message)
{
    auto rawMessage = OR((OR((message["rawMessage"]), (message["metadata"]))), (object{}));
    auto mapActionStatusToState = [=](auto status) mutable
    {
        static switch_type __switch534_862 = {
            { any(std:("pending")), 1 },
            { any(std:("executing")), 2 },
            { any(std:("running")), 3 },
            { any(std:("completed")), 4 },
            { any(std:("success")), 5 },
            { any(std:("failed")), 6 },
            { any(std:("error")), 7 }
        };
        switch (__switch534_862[status])
        {
        case 1:
        case 2:
        case 3:
            return std:("input-streaming");
        case 4:
        case 5:
            return std:("output-available");
        case 6:
        case 7:
            return std:("output-error");
        default:
            return std:("input-available");
        }
    };
    auto actionName = OR((OR((const_(rawMessage["actions"])[0]), (rawMessage["action"]))), (std:("ACTION")));
    auto actionStatus = OR((rawMessage["actionStatus"]), (std:("completed")));
    auto actionId = rawMessage["actionId"];
    auto inputData = object{};
    inputData->input = OR((rawMessage["actionResult"]["input"]), (object{}));
    auto outputData = object{};
    if (OR((rawMessage["text"]), (message["content"]))) {
        outputData->text = OR((rawMessage["text"]), (message["content"]));
    }
    if (actionStatus) outputData->status = actionStatus;
    if (rawMessage["actionResult"]) outputData->result = rawMessage["actionResult"];
    auto isError = OR((actionStatus == std:("failed")), (actionStatus == std:("error")));
    auto errorText = (isError) ? any(OR((OR((rawMessage["text"]), (message["content"]))), (std:("Action failed")))) (undefined);
    return object{
        object::pair{std:("type"), actionName}, 
        object::pair{std:("state"), mapActionStatusToState(actionStatus)}, 
        object::pair{std:("toolCallId"), actionId}, 
        object::pair{std:("input"), (Object->keys(inputData)->get_length() > 0) ? any(inputData) (undefined)}, 
        object::pair{std:("output"), (Object->keys(outputData)->get_length() > 0) ? any(outputData) (undefined)}, 
        object::pair{std:("errorText"), std:("errorText")}
    };
};


boolean isActionMessage(any message)
{
    return (OR((OR((message["sourceType"] == std:("agent_action")), (message["metadata"]["sourceType"] == std:("agent_action")))), (message["type"] == std:("agent_action"))));
};


