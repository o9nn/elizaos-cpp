#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_SRC_TYPES_TAURI-UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_SRC_TYPES_TAURI-UTILS_H
#include "core.h"
#include "./shared.h"

array<std::shared_ptr<MemoryEntry>> extractMemoriesFromResponse(std::any response);

array<std::shared_ptr<LogEntry>> extractLogsFromResponse(std::any response);

std::any isSuccessfulTauriResponse(std::any response);

template <typename T>
array<T> extractDataFromResponse(std::any response, std::any dataKey = undefined);

array<Record<std::string, any>> convertToRecordArray(array<any> data);

template <typename T>
array<T> extractDataFromResponse(std::any response, std::any dataKey)
{
    if (OR((!isSuccessfulTauriResponse(response)), (!response->data))) {
        return array<any>();
    }
    if (dataKey) {
        auto data = const_(response->data)[dataKey];
        return (Array->isArray(data)) ? (as<array<T>>(data)) : array<any>();
    }
    if (Array->isArray(response->data)) {
        return as<array<T>>(response->data);
    }
    return array<any>();
};


#endif
