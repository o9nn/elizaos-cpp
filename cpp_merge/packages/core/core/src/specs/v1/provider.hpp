#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_PROVIDER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_PROVIDER_H
#include "core.hpp"
#include "./types.h"
using ProviderFromTypes = Provider;
#include "./state.h"
#include "../v2.h"
using ProviderV2 = Provider;

typedef ProviderFromTypes Provider;


Provider fromV2Provider(std::shared_ptr<ProviderV2> providerV2);

std::shared_ptr<ProviderV2> toV2Provider(Provider provider);

#endif
