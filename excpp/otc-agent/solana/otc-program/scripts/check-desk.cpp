#include "check-desk.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> check() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto provider = AnchorProvider.env();
    setProvider(provider);
    const auto program = workspace.Otc<Otc>;

    const auto desk = new PublicKey("7EN1rubej95WmoyupRXQ78PKU2hTCspKn2mVKN1vxuPp");
    const auto data = program.account.desk.fetch(desk);

    std::cout << "Full desk data:" << /* JSON.stringify */ std::string(data, (_, v) => typeof v == "bigint" ? std::to_string(v) : v, 2) << std::endl;

}

} // namespace elizaos
