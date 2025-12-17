#include "verify-ready.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> verify() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto provider = AnchorProvider.env();
    setProvider(provider);
    const auto program = workspace.Otc<Otc>;

    const auto desk = new PublicKey("7EN1rubej95WmoyupRXQ78PKU2hTCspKn2mVKN1vxuPp");
    const auto data = program.account.desk.fetch(desk);

    const auto now = Math.floor(Date.now() / 1000);
    const auto pricesAge = now - parseInt(data.pricesUpdatedAt.toString(), 16);
    const auto maxAge = parseInt(data.maxPriceAgeSecs.toString(), 16);
    const auto tokenPrice = parseInt(data.tokenUsdPrice8d.toString(), 16) / 1e8;
    const auto solPrice = parseInt(data.solUsdPrice8d.toString(), 16) / 1e8;
    const auto tokenBalance = parseInt(data.tokenDeposited.toString(), 16);
    const auto reserved = parseInt(data.tokenReserved.toString(), 16);
    const auto available = tokenBalance - reserved;

    std::cout << "📊 LOCALNET READINESS CHECK" << std::endl;
    std::cout << "==================\n" << std::endl;

    std::cout << "✅ Prices:" << std::endl;
    std::cout << "   Token: $" + std::to_string(tokenPrice.toFixed(2)) << std::endl;
    std::cout << "   SOL: $" + std::to_string(solPrice.toFixed(2)) << std::endl;
    std::cout << "   Age: " + std::to_string(pricesAge) + "s (max: " + std::to_string(maxAge) + "s)" << std::endl;
    std::cout << "   Status: " + std::to_string(pricesAge < maxAge ? "FRESH ✅" : "STALE ❌") + "\n" << std::endl;

    std::cout << "✅ Inventory:" << std::endl;
    std::cout << "   Available: " + std::to_string(available) + " tokens" << std::endl;
    std::cout << "   Reserved: " + std::to_string(reserved) + " tokens" << std::endl;
    std::cout << "   Status: " + std::to_string(available > 0 ? "READY ✅" : "EMPTY ❌") + "\n" << std::endl;

    std::cout << "✅ Approvers:" << std::endl;
    std::cout << "   Count: " + std::to_string(data.approvers.length) << std::endl;
    std::cout << "   List: " + std::to_string(data.approvers.map((a: PublicKey) => a.toString().slice(0, 8)).join(", ")) + "\n" << std::endl;

    std::cout << "✅ Configuration:" << std::endl;
    std::cout << "   Paused: " + std::to_string(data.paused ? "YES ❌" : "NO ✅") << std::endl;
    std::cout << "   Next Offer ID: " + std::to_string(parseInt(data.nextOfferId.toString(), 16)) + "\n" << std::endl;

    if (pricesAge < maxAge && available > 0 && !data.paused && data.approvers.length > 0) {
        std::cout << "🎉 LOCALNET READY FOR TRADING" << std::endl;
        } else {
            std::cout << "⚠️  LOCALNET NOT READY - check items above" << std::endl;
            process.exit(1);
        }

}

} // namespace elizaos
