#include "TxToast.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TxToast(const std::any& { signature }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto link = "https://solscan.io/tx/" + std::to_string(signature);

    const auto linkToExplorer = [&]() {;
        // open in new tab
        window.open(link, "_blank");
        };

        return (;
        <div className="w-full space-y-2">;
        <p>Transaction sent:</p>
        <button;
    onClick={linkToExplorer}
    className="flex justify-center w-full border-green-500 border rounded-lg py-2 items-center";
    >;
    <img src="/link.svg" alt="link" className="w-4 h-4 mr-2" />;
    <p className="text-sm text-white font-bold">View</p>;
    </button>;
    </div>;
    );

}

} // namespace elizaos
