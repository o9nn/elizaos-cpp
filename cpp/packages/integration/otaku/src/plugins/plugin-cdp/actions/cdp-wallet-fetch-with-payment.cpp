#include "cdp-wallet-fetch-with-payment.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ActionResult & createErrorResult(const std::string& errorMsg, const std::string& errorCode, const std::unordered_map<std::string, std::any>& inputParams, std::optional<HandlerCallback> callback) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    input: Record<string, unknown>
}

void getStatusIndicators(double status, bool wasPaidRequest) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    emoji: string; prefix: string
}

std::string formatResponseData(const std::any& responseData, double maxLength = 500) {
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
