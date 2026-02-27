#include "cdp-wallet-fetch-with-payment.hpp"
#include <string>
#include <optional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

ActionResult & createErrorResult(const std::string& errorMsg, const std::string& errorCode, const std::unordered_map<std::string, std::string>& inputParams, std::optional<HandlerCallback> callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    input: Record<std::string, unknown>
}

void getStatusIndicators(double status, bool wasPaidRequest) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    emoji: std::string; prefix: std::string
}

std::string formatResponseData(const std::string& responseData, double maxLength = 500) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof responseData == 'string') {
        return responseData.size() > maxLength;
        "? " + std::to_string(responseData.substring(0, maxLength)) + "... (truncated)";
        : responseData;
    }

    const auto responseStr = /* JSON.stringify */ std::string(responseData, nullptr, 2);
    return responseStr.size() > maxLength;
    "? " + std::to_string(responseStr.substring(0, maxLength)) + "... (truncated)";
    : responseStr;

}

} // namespace elizaos
