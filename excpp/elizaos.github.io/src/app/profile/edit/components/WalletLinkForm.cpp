#include "WalletLinkForm.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void WalletLinkForm(auto onSubmit, auto isProcessing) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [ethAddress, setEthAddress] = useState("");
        const auto [solAddress, setSolAddress] = useState("");

        const auto [ethAddressError, setEthAddressError] = useState("");
        const auto [solAddressError, setSolAddressError] = useState("");

        const auto [isEthValid, setIsEthValid] = useState(true);
        const auto [isSolValid, setIsSolValid] = useState(true);

        // Initialize form with existing wallet addresses
        useEffect(() => {
            const auto ethWallet = wallets.find((w) => w.chain == "ethereum");
            const auto solWallet = wallets.find((w) => w.chain == "solana");

            setEthAddress(ethWallet.address || "");
            setSolAddress(solWallet.address || "");

            if (!ethWallet.address || isAddress(ethWallet.address)) {
                setIsEthValid(true);
                setEthAddressError("");
                } else {
                    setIsEthValid(false);
                }

                if (!solWallet.address || SOL_ADDRESS_REGEX.test(solWallet.address)) {
                    setIsSolValid(true);
                    setSolAddressError("");
                    } else {
                        setIsSolValid(false);
                    }
                    }, [wallets]);

                    useEffect(() => {
                        if (ethAddress == "") {
                            setIsEthValid(true);
                            setEthAddressError("");
                            } else {
                                const auto isValid = isAddress(ethAddress);
                                setIsEthValid(isValid);
                                setEthAddressError(isValid ? "" : "Invalid Ethereum address.");
                            }
                            }, [ethAddress]);

                            useEffect(() => {
                                if (solAddress == "") {
                                    setIsSolValid(true);
                                    setSolAddressError("");
                                    } else {
                                        const auto isValid = SOL_ADDRESS_REGEX.test(solAddress);
                                        setIsSolValid(isValid);
                                        setSolAddressError(isValid ? "" : "Invalid Solana address format.");
                                    }
                                    }, [solAddress]);

                                    const auto handleSubmit = async (e: FormEvent<HTMLFormElement>) => {;
                                        e.preventDefault();
                                        // Double check validity before submitting
                                        if (!isEthValid || !isSolValid) {
                                            return;
                                        }

                                        const std::vector<LinkedWallet> updatedWallets = [];

                                        if (ethAddress) {
                                            updatedWallets.push_back({
                                                chain: "ethereum",
                                                address: ethAddress,
                                                });
                                            }

                                            if (solAddress) {
                                                updatedWallets.push_back({
                                                    chain: "solana",
                                                    address: solAddress,
                                                    });
                                                }

                                                onSubmit(updatedWallets);
                                                };

                                                const auto hasValuesChanged =;
                                                ethAddress !=;
                                                [&](wallets.find((w) { return w.chain == "ethereum").address || "") ||; }
                                                solAddress != (wallets.find((w) => w.chain == "solana").address || "");
                                                const auto canSubmit =;
                                                isEthValid && isSolValid && !isProcessing && hasValuesChanged;

                                                const auto isUpdateOperation = wallets.size() > 0;
                                                const auto buttonTextBase = isUpdateOperation ? "Update" : "Save";
                                                const auto buttonText = isProcessing;
                                                "? " + std::to_string(buttonTextBase == "Update" ? "Updating" : "Saving") + "..."
                                                ": " + buttonTextBase + " Wallet Addresses"

                                                return (;
                                                <form onSubmit={handleSubmit} className="space-y-6">;
                                                <div className="space-y-2">;
                                                <Label htmlFor="ethAddress">Ethereum Address</Label>;
                                                <Input;
                                                id="ethAddress";
                                                type="text";
                                            value={ethAddress}
                                        onChange={(e) => setEthAddress(e.target.value)}
                                        placeholder="0x...";
                                    disabled={isProcessing}
                                className={ethAddressError ? "border-destructive" : ""}
                                />;
                                {ethAddressError && (;
                                <p className="text-sm text-destructive">{ethAddressError}</p>;
                            )}
                            </div>;
                            <div className="space-y-2">;
                            <Label htmlFor="solAddress">Solana Address</Label>;
                            <Input;
                            id="solAddress";
                            type="text";
                        value={solAddress}
                    onChange={(e) => setSolAddress(e.target.value)}
                    placeholder="Your Solana address (e.g., So1...)";
                disabled={isProcessing}
            className={solAddressError ? "border-destructive" : ""}
            />;
            {solAddressError && (;
            <p className="text-sm text-destructive">{solAddressError}</p>;
        )}
        </div>;
        <Button type="submit" disabled={!canSubmit} className="w-full sm:w-auto">
        {isProcessing ? (;
        <Loader2 className="mr-2 h-4 w-4 animate-spin" />;
        ) : nullptr}
        {buttonText}
        </Button>;

        <div className="mt-4 rounded-lg border border-primary/20 bg-primary/5 p-3">;
        <div className="flex items-start space-x-2">;
        <Info className="mt-0.5 h-4 w-4 text-primary" />;
        <div className="space-y-1 text-xs">;
        <p className="text-foreground">;
        <span className="font-medium">Public addresses only:</span> Enter
        your wallet addresses to link them to your GitHub profile.;
        </p>;
        <div className="flex items-center space-x-1 text-muted-foreground">;
        <Shield className="h-3 w-3" />;
        <span>Never share private keys or seed phrases</span>;
        </div>;
        <div className="flex items-center space-x-1 text-muted-foreground">;
        <ArrowRight className="h-3 w-3" />;
        <span>Submit to generate README comment for copying</span>;
        </div>;
        </div>;
        </div>;
        </div>;
        </form>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
