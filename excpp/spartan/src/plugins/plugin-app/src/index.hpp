#include "actions/act_menu.hpp"
#include "actions/act_reg_confirmemail.hpp"
#include "actions/act_reg_delete.hpp"
#include "actions/act_reg_query.hpp"
#include "actions/act_reg_start.hpp"
#include "actions/act_wallet_create.hpp"
#include "actions/act_wallet_list.hpp"
#include "actions/act_wallet_setstrategy.hpp"
#include "actions/devfix.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

import type { Plugin } from "@elizaos/core";

// actions
;
;
;
;
;
;
;
;
;

const appPlugin: Plugin = {
    name: "AppDev",
    description: "application development framework for ElizaOS",
    actions: [
      userRegistration, checkRegistrationCode, checkRegistration, deleteRegistration,
      servicesMenu, walletCreate, setStrategy, userMetawalletList, devFix
    ],
    evaluators: [],
    providers: [],
};

default appPlugin;

} // namespace elizaos
