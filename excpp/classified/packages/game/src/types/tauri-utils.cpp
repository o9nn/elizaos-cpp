#include "tauri-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<MemoryEntry> extractMemoriesFromResponse(unknown response) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!response || typeof response != 'object') {
        return [];
    }

    // If response is already an array, return it (assuming it's MemoryEntry[])
    if (Array.isArray(response)) {
        return response[];
    }

    const auto responseObj = response;

    // Check for memories in the direct property
    if (Array.isArray(responseObj.memories)) {
        return responseObj.memories;
    }

    // Check for memories in the data.memories property
    if (responseObj.data && Array.isArray(responseObj.data.memories)) {
        return responseObj.data.memories;
    }

    // Check for memories in the data property (direct array)
    if (responseObj.data && Array.isArray(responseObj.data)) {
        return responseObj.data[];
    }

    return [];

}

std::vector<LogEntry> extractLogsFromResponse(unknown response) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!response || typeof response != 'object') {
        return [];
    }

    const auto responseObj = response;

    // Check if it's a successful response with logs
    if (responseObj.success && responseObj.data && Array.isArray(responseObj.data.logs)) {
        return responseObj.data.logs;
    }

    return [];

}

response is TauriMemoryResponse & isSuccessfulTauriResponse(unknown response) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    success: true
}

std::vector<std::unordered_map<std::string, unknown>> convertToRecordArray(const std::vector<unknown>& data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return data.map((item) => {;
        if (typeof item == 'object' && item != null) {
            return item<string, unknown>;
        }
        return { value: item }
        });

}

} // namespace elizaos
