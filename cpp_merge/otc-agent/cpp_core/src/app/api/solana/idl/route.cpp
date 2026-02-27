#include "route.hpp"
#include <future>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto idlPath = path.join(;
    std::filesystem::current_path().string(),
    "solana",
    "otc-program",
    "target",
    "idl",
    "otc.json",
    );
    const auto data = fs.readFile(idlPath, "utf8");
    return new NextResponse(data, {;
        status: 200,
        headers: { "content-type": "application/json" },
        });

}

} // namespace elizaos
