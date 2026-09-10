#pragma once

#include <memory>
#include <string>

#include "http_client.hpp"

namespace elizaos {

enum class KoboldEndpointPolicy { LoopbackOrPrivateOnly, AllowRemote };

class KoboldCppProvider {
public:
    static constexpr const char* PROVIDER_NAME = "koboldcpp";
    static constexpr const char* DEFAULT_ENDPOINT = "http://localhost:5001";

    static std::string generate(
        const std::string& prompt, const std::string& system_prompt,
        const std::string& endpoint = DEFAULT_ENDPOINT, int max_tokens = 500,
        double temperature = 0.7,
        std::shared_ptr<const HttpTransport> transport = {},
        KoboldEndpointPolicy endpoint_policy = KoboldEndpointPolicy::LoopbackOrPrivateOnly,
        HttpRequestOptions request_options = {});

    static std::string generate_native(
        const std::string& prompt, const std::string& endpoint = DEFAULT_ENDPOINT,
        int max_length = 500, double temperature = 0.7, double rep_pen = 1.1,
        int top_k = 40, double top_p = 0.9,
        std::shared_ptr<const HttpTransport> transport = {},
        KoboldEndpointPolicy endpoint_policy = KoboldEndpointPolicy::LoopbackOrPrivateOnly,
        HttpRequestOptions request_options = {});

    static bool is_available(
        const std::string& endpoint = DEFAULT_ENDPOINT,
        std::shared_ptr<const HttpTransport> transport = {},
        KoboldEndpointPolicy endpoint_policy = KoboldEndpointPolicy::LoopbackOrPrivateOnly,
        HttpRequestOptions request_options = {});
};

} // namespace elizaos
