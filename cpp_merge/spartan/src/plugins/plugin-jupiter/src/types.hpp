#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_JUPITER_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_JUPITER_SRC_TYPES_H
#include "core.hpp"

class JupiterQuoteParams;
class JupiterSwapParams;
class JupiterQuoteResponse;
class JupiterSwapResponse;

class JupiterQuoteParams : public object, public std::enable_shared_from_this<JupiterQuoteParams> {
public:
    using std::enable_shared_from_this<JupiterQuoteParams>::shared_from_this;
    string inputMint;

    string outputMint;

    double amount;

    double slippageBps;
};

class JupiterSwapParams : public object, public std::enable_shared_from_this<JupiterSwapParams> {
public:
    using std::enable_shared_from_this<JupiterSwapParams>::shared_from_this;
    any quoteResponse;

    string userPublicKey;

    double slippageBps;
};

class JupiterQuoteResponse : public object, public std::enable_shared_from_this<JupiterQuoteResponse> {
public:
    using std::enable_shared_from_this<JupiterQuoteResponse>::shared_from_this;
    string inputMint;

    string outputMint;

    string inAmount;

    string outAmount;

    string otherAmountThreshold;

    string swapMode;

    double slippageBps;

    string priceImpactPct;

    array<any> routePlan;

    double contextSlot;

    double timeTaken;
};

class JupiterSwapResponse : public object, public std::enable_shared_from_this<JupiterSwapResponse> {
public:
    using std::enable_shared_from_this<JupiterSwapResponse>::shared_from_this;
    string swapTransaction;

    double lastValidBlockHeight;

    double prioritizationFeeLamports;
};

#endif
