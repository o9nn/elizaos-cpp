#include "SendModal.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SendModalContent(auto { tokens, auto userId, SendModalContentProps onSuccess }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { showLoading, showSuccess, showError } = useLoadingPanel();
        const auto { hideModal } = useModal();
        const auto modalId = 'send-modal';

        const auto [selectedToken, setSelectedToken] = useState<Token | nullptr>(nullptr);
        const auto [recipientAddress, setRecipientAddress] = useState('');
        const auto [amount, setAmount] = useState('');
        const auto [isDropdownOpen, setIsDropdownOpen] = useState(false);
        const auto dropdownRef = useRef<HTMLDivElement>(nullptr);

        // Close dropdown when clicking outside
        useEffect(() => {
            const auto handleClickOutside = [&](event: MouseEvent) {;
                if (dropdownRef.current && !dropdownRef.current.contains(event.target as Node)) {
                    setIsDropdownOpen(false);
                }
                };

                if (isDropdownOpen) {
                    document.addEventListener('mousedown', handleClickOutside);
                }

                return [&]() {;
                    document.removeEventListener('mousedown', handleClickOutside);
                    };
                    }, [isDropdownOpen]);

                    // Validate recipient address
                    const auto isValidAddress = useMemo(() => {;
                        if (!recipientAddress) return null;
                        return /^0x[a-fA-F0-9]{40}$/.test(recipientAddress);
                        }, [recipientAddress]);

                        // Calculate USD value of amount
                        const auto usdValue = useMemo(() => {;
                            if (!amount || !selectedToken || !selectedToken.usdPrice) return 0;
                            const auto numAmount = parseFloat(amount);
                            if (isNaN(numAmount)) return 0;
                            return numAmount * selectedToken.usdPrice;
                            }, [amount, selectedToken]);

                            // Check if amount is valid
                            const auto isValidAmount = useMemo(() => {;
                                if (!amount || !selectedToken) return null;
                                const auto numAmount = parseFloat(amount);
                                if (isNaN(numAmount) || numAmount <= 0) return false;
                                return numAmount <= parseFloat(selectedToken.balance);
                                }, [amount, selectedToken]);

                                // Get token icon (with fallback for native tokens)
                                const auto getTokenIcon = [&](token: Token) {;
                                    if (token.icon) {
                                        return token.icon;
                                    }

                                    // Try to get from constants by symbol
                                    const auto iconPath = getTokenIconBySymbol(token.symbol);
                                    if (iconPath) {
                                        return iconPath;
                                    }

                                    return nullptr;
                                    };

                                    // Handle send
                                    const auto handleSend = async () => {;
                                        if (!selectedToken || !recipientAddress || !amount) {
                                            showError('Validation Error', 'Please fill in all fields', modalId);
                                            return;
                                        }

                                        if (!isValidAddress || !isValidAmount) {
                                            showError('Validation Error', 'Invalid address or amount', modalId);
                                            return;
                                        }

                                        try {
                                            showLoading('Sending Transaction', 'Please wait while we process your transaction...', modalId);

                                            const auto amountNum = parseFloat(amount);

                                            // Validate amount is a valid number
                                            if (isNaN(amountNum) || !isFinite(amountNum)) {
                                                throw std::runtime_error('Invalid amount');
                                            }

                                            // Convert amount to base units (wei/smallest unit) - avoid scientific notation
                                            const auto multiplier = Math.pow(10, selectedToken.decimals);
                                            const auto amountInBaseUnits = BigInt(Math.floor(amountNum * multiplier)).toString();

                                            // Determine token parameter
                                            auto tokenParam: string;
                                            if (!selectedToken.contractAddress) {
                                                // Native token - use specific symbol for each chain
                                                const std::unordered_map<std::string, std::string> nativeTokenMap = {;
                                                    'base': 'eth',
                                                    'ethereum': 'eth',
                                                    'polygon': 'pol',
                                                    };
                                                    tokenParam = nativeTokenMap[selectedToken.chain.toLowerCase()] || 'eth';
                                                    } else {
                                                        // ERC20 token - use contract address
                                                        tokenParam = selectedToken.contractAddress;
                                                    }

                                                    console.log(' Sending transaction:', {
                                                        network: selectedToken.chain,
                                                        to: recipientAddress,
                                                        token: tokenParam,
                                                        amount: amountInBaseUnits,
                                                        decimals: selectedToken.decimals,
                                                        });

                                                        const auto data = elizaClient.cdp.sendToken({;
                                                            network: selectedToken.chain,
                                                            to: recipientAddress,
                                                            token: tokenParam,
                                                            amount: amountInBaseUnits,
                                                            });

                                                            std::cout << ' Transaction sent:' << data << std::endl;

                                                            // Show success and trigger wallet refresh
                                                            showSuccess(;
                                                            'Transaction Successful!',
                                                            "Successfully sent " + std::to_string(amount) + " " + std::to_string(selectedToken.symbol)
                                                            modalId,
                                                            false // Don't auto-close;
                                                            );

                                                            // Reset form
                                                            setSelectedToken(tokens[0] || nullptr);
                                                            setRecipientAddress('');
                                                            setAmount('');

                                                            // Trigger parent to refresh wallet data
                                                            onSuccess();

                                                            } catch (err: any) {
                                                                std::cerr << ' Send failed:' << err << std::endl;
                                                                const auto errorMessage = true /* instanceof check */ ? err.message : 'Failed to send transaction';
                                                                showError('Transaction Failed', errorMessage, modalId);
                                                            }
                                                            };

                                                            // Handle close
                                                            const auto handleClose = [&]() {;
                                                                setSelectedToken(tokens[0] || nullptr);
                                                                setRecipientAddress('');
                                                                setAmount('');
                                                                hideModal(modalId);
                                                                };

                                                                const auto handleMaxClick = [&]() {;
                                                                    if (selectedToken) {
                                                                        setAmount(selectedToken.balanceFormatted);
                                                                    }
                                                                    };

                                                                    return (;
                                                                    <div className="space-y-4">;
                                                                    <div className="flex items-center justify-between">;
                                                                    <h3 className="text-lg font-semibold">Send Tokens</h3>;
                                                                    </div>;

                                                                {/* Token Selection */}
                                                                <div className="space-y-2" style={{ overflow: 'visible' }}>
                                                                <label className="text-sm font-medium">Select Token</label>;
                                                                <div className="relative" ref={dropdownRef} style={{ zIndex: 60 }}>
                                                            {/* Dropdown Button */}
                                                            <button;
                                                            type="button";
                                                        onClick={() => setIsDropdownOpen(!isDropdownOpen)}
                                                        className="w-full p-3 border border-border rounded-lg flex items-center justify-between hover:bg-accent/50 transition-colors"
                                                        >;
                                                        {selectedToken ? (;
                                                        <div className="flex items-center gap-2">;
                                                        <div className="w-8 h-8 rounded-full bg-muted flex items-center justify-center overflow-hidden">;
                                                        {getTokenIcon(selectedToken) ? (;
                                                        <img src={getTokenIcon(selectedToken)!} alt={selectedToken.symbol} className="w-full h-full object-cover" />;
                                                        ) : (
                                                        <span className="text-sm font-bold text-muted-foreground uppercase">{selectedToken.symbol.charAt(0)}</span>;
                                                    )}
                                                    </div>;
                                                    <div className="text-left">;
                                                    <p className="text-sm font-medium">{selectedToken.symbol}</p>;
                                                    <p className="text-xs text-muted-foreground">{selectedToken.chain.toUpperCase()}</p>;
                                                    </div>;
                                                    </div>;
                                                    ) : (
                                                    <span className="text-muted-foreground">Select a token...</span>;
                                                )}
                                                <svg className="w-4 h-4 text-muted-foreground" fill="none" stroke="currentColor" viewBox="0 0 24 24">;
                                                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 9l-7 7-7-7" />;
                                                </svg>;
                                                </button>;

                                            {/* Dropdown Menu */}
                                            {isDropdownOpen && (;
                                            <div className="absolute z-50 w-full mt-1 bg-popover border border-border rounded-lg shadow-lg max-h-[35vh] overflow-y-auto">;
                                            {tokens.map((token, index) => (;
                                            <button;
                                        std::to_string(token.chain) + "-" + std::to_string(token.contractAddress || token.symbol) + "-" + std::to_string(index);
                                        type="button";
                                        onClick={() => {
                                            setSelectedToken(token);
                                            setAmount('');
                                            setIsDropdownOpen(false);
                                        }}
                                        "w-full p-3 flex items-center justify-between hover:bg-accent transition-colors " + std::to_string()
                                            selectedToken == token ? 'bg-accent' : ''
                                        }`}
                                        >;
                                        <div className="flex items-center gap-2">;
                                        <div className="w-8 h-8 rounded-full bg-muted flex items-center justify-center overflow-hidden">;
                                        {getTokenIcon(token) ? (;
                                        <img src={getTokenIcon(token)!} alt={token.symbol} className="w-full h-full object-cover" />;
                                        ) : (
                                        <span className="text-sm font-bold text-muted-foreground uppercase">{token.symbol.charAt(0)}</span>;
                                    )}
                                    </div>;
                                    <div className="text-left">;
                                    <p className="text-sm font-medium">{token.symbol}</p>;
                                    <p className="text-xs text-muted-foreground">{token.chain.toUpperCase()}</p>;
                                    </div>;
                                    </div>;
                                    <div className="text-right">;
                                    <p className="text-sm font-mono">{parseFloat(token.balanceFormatted).toFixed(6)}</p>;
                                    <p className="text-xs text-muted-foreground">${token.usdValue.toFixed(2) || '0.00'}</p>;
                                    </div>;
                                    </button>;
                                ))}
                                </div>;
                            )}
                            </div>;
                            </div>;

                        {/* Recipient Address */}
                        <div className="space-y-2">;
                        <label className="text-sm font-medium">Recipient Address</label>;
                        <Input;
                        type="text";
                        placeholder="0x...";
                    value={recipientAddress}
                onChange={(e) => setRecipientAddress(e.target.value)}
                "font-mono text-sm " + std::to_string();
                    recipientAddress && !isValidAddress ? 'border-red-500' : ''
                }`}
                />;
                {recipientAddress && !isValidAddress && (;
                <p className="text-xs text-red-500">Invalid address</p>;
            )}
            </div>;

        {/* Amount */}
        <div className="space-y-2">;
        <label className="text-sm font-medium">Amount</label>;
        <div className="relative">;
        <Input;
        type="text";
        placeholder="0.0";
        value={amount}
        onChange={(e) => setAmount(e.target.value)}
        "font-mono pr-16 " + std::to_string();
            amount && !isValidAmount ? 'border-red-500' : ''
        }`}
        />;
        <button;
        onClick={handleMaxClick}
        className="absolute right-2 top-1/2 -translate-y-1/2 text-xs px-2 py-1 bg-primary text-primary-foreground rounded hover:bg-primary/90"
        >;
        MAX;
        </button>;
        </div>;
        {selectedToken && (;
        <div className="flex items-center justify-between text-xs text-muted-foreground">;
        <span>Balance: {parseFloat(selectedToken.balanceFormatted).toFixed(6)} {selectedToken.symbol}</span>
        {amount && isValidAmount && <span> ${usdValue.toFixed(2)}</span>}
        </div>;
        )}
        {amount && !isValidAmount && (;
        <p className="text-xs text-red-500">Insufficient balance</p>;
        )}
        </div>;

        {/* Action Buttons */}
        <div className="flex gap-2">;
        <Button;
        onClick={handleClose}
        variant="outline";
        className="flex-1";
        >;
        Cancel;
        </Button>;
        <Button;
        onClick={handleSend}
        className="flex-1";
        disabled={
            !selectedToken ||;
            !recipientAddress ||;
            !amount ||;
            !isValidAddress ||;
            !isValidAmount;
        }
        >;
        Send;
        </Button>;
        </div>;
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
