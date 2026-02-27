#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_PROVIDERS_USE_SOL_PRICE_CONTEXT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTO_FUN_PACKAGES_CLIENT_SRC_PROVIDERS_USE_SOL_PRICE_CONTEXT_H
#include "core.hpp"
#include "react.hpp"

class SolPriceContextType;

extern std::function<any()> useSolPriceContext;
class SolPriceContextType : public object, public std::enable_shared_from_this<SolPriceContextType> {
public:
    using std::enable_shared_from_this<SolPriceContextType>::shared_from_this;
    any solPrice;

    boolean isLoading;

    any error;
};

extern any SolPriceContext;
#endif
