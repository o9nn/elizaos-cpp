#include ".classes/guild.hpp"
#include ".classes/user.hpp"
#include "install-params-object.hpp"
#include "team-object.hpp"
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

// https://discord.com/developers/docs/resources/application#application-object
;
;
;
;

using ApplicationObject = {
  id: Snowflake;
  name: string;
  icon: string | Maybe;
  description: string;
  rpc_origins?: string[];
  bot_public: boolean;
  bot_require_code_grant: boolean;
  bot?: User;
  terms_of_service_url?: string;
  privacy_policy_url?: string;
  owner?: User;
  summary: string;
  verify_key: string;
  team: TeamObject | Maybe;
  guild_id?: Snowflake;
  guild?: Guild;
  primary_sku_id?: Snowflake;
  slug?: string;
  cover_image?: string;
  flags?: number;
  approximate_guild_count?: number;
  redirect_uris?: string[];
  interactions_endpoint_url?: string;
  role_connections_verification_url?: string;
  tags?: string[];
  install_params?: InstallParamsObject;
  custom_install_url?: string;
};

} // namespace elizaos
