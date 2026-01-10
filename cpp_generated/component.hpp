#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_CYPRESS_SUPPORT_COMPONENT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLIENT_CYPRESS_SUPPORT_COMPONENT_H
#include "core.h"
#include "./commands.h"
#include "@cypress/react.h"
#include "react.h"
using React = _default;
#include "react-router-dom.h"
#include "@tanstack/react-query.h"
#include "@/index.css.h"
#include "@radix-ui/react-direction.h"
#include "@/components/ui/tooltip.h"
#include "buffer.h"
#include "process/browser.h"
using process = _default;

extern std::function<any()> createTestQueryClient;
any mountWithProviders(std::shared_ptr<React::ReactNode> component, object options = object{});

any mountWithRouter(std::shared_ptr<React::ReactNode> component, object options = object{});

any mountRadix(std::shared_ptr<React::ReactNode> component, object options = object{});

#endif
