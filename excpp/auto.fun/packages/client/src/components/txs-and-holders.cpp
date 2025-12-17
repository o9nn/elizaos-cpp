#include "txs-and-holders.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TransactionsAndHolders() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [mode, setMode] = useState<"transactions" | "holders">("transactions");

    return (;
    <div className="md:overflow-x-hidden xs:max-w-fit md:max-w-full">
    <div className="flex items-center justify-between py-3">;
    <div className="flex items-center">;
    <Button;
    size="small";
    variant={mode == "transactions" ? "primary" : "outline"}
    onClick={() => setMode("transactions")}
    >;
    Trades;
    </Button>;
    <Button;
    size="small";
    variant={mode == "holders" ? "primary" : "outline"}
    onClick={() => setMode("holders")}
    >;
    Holders;
    </Button>;
    </div>;
    </div>;
    {mode == "transactions" ? (;
    <SwapsTable token={token} />;
    ) : (
    <HoldersTable token={token} />;
    )}
    </div>;
    );

}

} // namespace elizaos
