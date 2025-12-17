#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".constants.hpp"
#include ".utils.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Create an SMTP transporter

void generateRandomString(auto length);

void extractEmails(auto text);

std::future<void> sendVerifyEmail(auto address, auto regCode);

// handle no EMAIL_TYPE to EMAIL_TYPE transition
/*
*/

    // if not a discord/telegram message, we can ignore it

    // using the service to get this/components might be good way

    // ok we need to change a state on this author

    // get room and it's components?
    // doesn't have components

      // initialize

    // using the service to get this/components might be good way

        // any overlap?

        // set this entities email
      // update spartanData
      std::future<void> updateSpartanData(auto agentEntityId, auto spartanData);
      // if we need to update it
      // no email provided

      // we can make a component for the state of this form

      // do we have an email component already
        // if so we should confirm

        // set wizard state
        // set form state
        // yes/no
        // set form state
/*
*/
} // namespace elizaos
