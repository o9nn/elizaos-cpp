#include "elizaos/core.hpp"
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



class TradeDataProviderService extends Service {
  private isRunning = false;

  static serviceType = 'TRADER_DATAPROVIDER';
  capabilityDescription = 'The agent is able to get information about blockchains';

  // config (key/string)

  constructor(public runtime: IAgentRuntime) {
    super(runtime); // sets this.runtime
    this.registry = {};
    console.log('TRADER_DATAPROVIDER cstr');
    this.events = new Map();
  }

  // return DataProvider handle
    // add to registry

  // interested in trending updates
    // add to our local eventHandlers

  // should be a task and not here
    // get a list of positions (chains -> wallets -> positions)

  // should this be a task?
    // collect all
      // foreach provider
      // do they have this type of service
        // if so register handler with event
          //console.log('updateTrending - result', result)
    // process results

    // emit event

    // this doesn't go here, just temp hack

  // interested in price delta events

    // for each provider in register
      // do they have this type of service
        // if so register handler with event
        // add to our local eventHandlers

      // not cache, go fetch realtime
    // needs to include liquidity, 24h volume, suspicous atts

  /**
   * Start the scenario service with the given runtime.
   * @param {IAgentRuntime} runtime - The agent runtime
   * @returns {Promise<ScenarioService>} - The started scenario service
   */
  /**
   * Stops the Scenario service associated with the given runtime.
   *
   * @param {IAgentRuntime} runtime The runtime to stop the service for.
   * @throws {Error} When the Scenario service is not found.
   */

    // maybe we don't need to do this under the first registers
    // this is actually too soon
    //this.updateTrending()


} // namespace elizaos
