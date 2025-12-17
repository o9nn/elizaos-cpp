#include "add-approver.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> addApprover() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto provider = AnchorProvider.env();
    setProvider(provider);
    const auto program = workspace.Otc<Otc>;

    const auto ownerData = /* JSON.parse */ fs.readFileSync("./id.json", "utf8");
    const auto owner = Keypair.fromSecretKey(Uint8Array.from(ownerData));
    const auto desk = new PublicKey("7EN1rubej95WmoyupRXQ78PKU2hTCspKn2mVKN1vxuPp");

    std::cout << "Adding owner..." << std::endl;
    std::cout << "  Owner:" << owner.std::to_string(publicKey) << std::endl;
    std::cout << "  Desk:" << std::to_string(desk) << std::endl;

    const auto tx = program.methods;
    .setApprover(owner.publicKey, true);
    .accountsPartial({
        desk,
        owner: owner.publicKey,
        });
        .signers([owner]);
        .rpc();

        std::cout << "✅ Owner added << tx:" << tx << std::endl;

}

} // namespace elizaos
