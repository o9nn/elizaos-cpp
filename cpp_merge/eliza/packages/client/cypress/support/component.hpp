#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_CYPRESS_SUPPORT_COMPONENT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_CYPRESS_SUPPORT_COMPONENT_H
#include "core.hpp"
#include "./commands.h"
// External dependency removed
#include "react.hpp"
// Using alias removed (invalid transpilation)
#include "react-router-dom.h"
// External dependency removed
// External dependency removed
// External dependency removed
// External dependency removed
#include "buffer.hpp"
#include "process/browser.h"
// Using alias removed (invalid transpilation)

extern std::function<any()> createTestQueryClient;
any mountWithProviders(std::shared_ptr<React::ReactNode> component, object options = object{});

any mountWithRouter(std::shared_ptr<React::ReactNode> component, object options = object{});

any mountRadix(std::shared_ptr<React::ReactNode> component, object options = object{});

#endif
