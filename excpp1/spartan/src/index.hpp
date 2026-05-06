#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "init.hpp"
#include "plugins/communityInvestor.hpp"
#include "plugins/degenIntel.hpp"
#include "plugins/degenTrader.hpp"
#include "plugins/helius.hpp"
#include "plugins/plugin-app.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Read and convert to Base64

/**
 * Represents a character named Spartan who is a DeFi trading agent specializing in Solana-based trading and liquidity pool management.
 *
 * @typedef {Object} Character
 * @property {string} name - The name of the character
 * @property {string[]} plugins - List of plugins used by the character
 * @property {Object} secrets - Object containing secret keys for Discord application
 * @property {string} system - Description of the character's system and capabilities
 * @property {string[]} bio - Bio of the character highlighting its specialties and traits
 * @property {Object[]} messageExamples - Examples of messages exchanged by the character in chats
 * @property {Object} style - Object containing communication style guidelines for the character
 */

    //"hentai",
    //"Hentai",
      /*
      //"Use precise numbers and percentages",
      //"Professional and focused on task",
      //"No speculation or financial advice",
      //"Keep responses brief", // and data-focused
      //"Use market terminology correctly",
      //"Stay neutral about price movements",
      */
      /*
      */

    // this wasn't here in the initial Spartan

/**
 * @typedef {Object} OnboardingConfig
 * @property {Object} settings - Contains various settings for onboarding.
 * @property {Object} settings.POOL_SETTINGS - Default settings for new trading pools.
 * @property {string} settings.POOL_SETTINGS.name - Name of the setting.
 * @property {string} settings.POOL_SETTINGS.description - Description of the setting.
 * @property {string} settings.POOL_SETTINGS.usageDescription - Usage description of the setting.
 * @property {boolean} settings.POOL_SETTINGS.required - Indicates if the setting is required.
 * @property {boolean} settings.POOL_SETTINGS.public - Indicates if the setting is public.
 * @property {boolean} settings.POOL_SETTINGS.secret - Indicates if the setting is secret.
 * @property {Function} settings.POOL_SETTINGS.validation - Function to validate the setting value.
 * @property {Object} settings.DEX_PREFERENCES - Preferred DEXs and their priority order.
 * @property {string} settings.DEX_PREFERENCES.name - Name of the setting.
 * @property {string} settings.DEX_PREFERENCES.description - Description of the setting.
 * @property {string} settings.DEX_PREFERENCES.usageDescription - Usage description of the setting.
 * @property {boolean} settings.DEX_PREFERENCES.required - Indicates if the setting is required.
 * @property {boolean} settings.DEX_PREFERENCES.public - Indicates if the setting is public.
 * @property {boolean} settings.DEX_PREFERENCES.secret - Indicates if the setting is secret.
 * @property {Function} settings.DEX_PREFERENCES.validation - Function to validate the setting value.
 * @property {Object} settings.COPY_TRADE_SETTINGS - Settings for copy trading functionality.
 * @property {string} settings.COPY_TRADE_SETTINGS.name - Name of the setting.
 * @property {string} settings.COPY_TRADE_SETTINGS.description - Description of the setting.
 * @property {string} settings.COPY_TRADE_SETTINGS.usageDescription - Usage description of the setting.
 * @property {boolean} settings.COPY_TRADE_SETTINGS.required - Indicates if the setting is required.
 * @property {boolean} settings.COPY_TRADE_SETTINGS.public - Indicates if the setting is public.
 * @property {boolean} settings.COPY_TRADE_SETTINGS.secret - Indicates if the setting is secret.
 * @property {Object} settings.LP_SETTINGS - Default settings for LP management.
 * @property {string} settings.LP_SETTINGS.name - Name of the setting.
 * @property {string} settings.LP_SETTINGS.description - Description of the setting.
 * @property {string} settings.LP_SETTINGS.usageDescription - Usage description of the setting.
 * @property {boolean} settings.LP_SETTINGS.required - Indicates if the setting is required.
 * @property {boolean} settings.LP_SETTINGS.public - Indicates if the setting is public.
 * @property {boolean} settings.LP_SETTINGS.secret - Indicates if the setting is secret.
 * @property {Object} settings.RISK_LIMITS - Trading and risk management limits.
 * @property {string} settings.RISK_LIMITS.name - Name of the setting.
 * @property {string} settings.RISK_LIMITS.description - Description of the setting.
 * @property {string} settings.RISK_LIMITS.usageDescription - Usage description of the setting.
 * @property {boolean} settings.RISK_LIMITS.required - Indicates if the setting is required.
 * @property {boolean} settings.RISK_LIMITS.public - Indicates if the setting is public.
 * @property {boolean} settings.RISK_LIMITS.secret - Indicates if the setting is secret.
 */
    // disable these settings for now
    // these are more specific than Spartan, more like specific plugin config
    /*
    */


} // namespace elizaos
