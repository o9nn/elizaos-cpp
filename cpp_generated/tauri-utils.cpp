#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/src/types/tauri-utils.h"

array<std::shared_ptr<MemoryEntry>> extractMemoriesFromResponse(any response)
{
    if (OR((!response), (type_of(response) != std::string("object")))) {
        return array<any>();
    }
    if (Array->isArray(response)) {
        return as<array<std::shared_ptr<MemoryEntry>>>(response);
    }
    auto responseObj = as<std::shared_ptr<TauriMemoryResponse>>(response);
    if (Array->isArray(responseObj->memories)) {
        return responseObj->memories;
    }
    if (AND((responseObj->data), (Array->isArray(responseObj->data["memories"])))) {
        return responseObj->data["memories"];
    }
    if (AND((responseObj->data), (Array->isArray(responseObj->data)))) {
        return as<array<std::shared_ptr<MemoryEntry>>>(responseObj->data);
    }
    return array<any>();
};


array<std::shared_ptr<LogEntry>> extractLogsFromResponse(any response)
{
    if (OR((!response), (type_of(response) != std::string("object")))) {
        return array<any>();
    }
    auto responseObj = as<std::shared_ptr<TauriMemoryResponse>>(response);
    if (AND((AND((responseObj->success), (responseObj->data))), (Array->isArray(responseObj->data["logs"])))) {
        return responseObj->data["logs"];
    }
    return array<any>();
};


any isSuccessfulTauriResponse(any response)
{
    return (AND((AND((AND((response != nullptr), (type_of(response) == std::string("object")))), (in(std::string("success"), response)))), ((as<std::shared_ptr<TauriMemoryResponse>>(response))->success == true)));
};


array<Record<string, any>> convertToRecordArray(array<any> data)
{
    return data->map([=](auto item) mutable
    {
        if (AND((type_of(item) == std::string("object")), (item != nullptr))) {
            return as<Record<string, any>>(item);
        }
        return object{
            object::pair{std::string("value"), item}
        };
    }
    );
};


