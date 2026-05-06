#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types/base.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ApiError {
public:
    ApiError(const std::string& public code, const std::string& message, std::optional<std::string> public details, std::optional<double> public status);
};

    void if(auto this.baseUrl);
    void if(auto options.params);
    void if(auto options.body instanceof FormData);

      // Handle empty responses (204 No Content)
    void catch(auto error);
    void if(auto !response.ok || !data.success);
    void catch(auto error);


} // namespace elizaos
