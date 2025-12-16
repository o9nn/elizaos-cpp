#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ProfileRepoNoticeProps {
    string | undefined; // User login name for display userLogin;
    boolean; // True if the page is initially loading data pageLoading;
    () => void; // Function to call when the create button is clicked onCreateRepo;
};


} // namespace elizaos
