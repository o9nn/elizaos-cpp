#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS_TRADER_ACTIONS_ACT_REG_START_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS_TRADER_ACTIONS_ACT_REG_START_H
#include "core.hpp"
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;
#include "nodemailer.hpp"
// Using alias removed (invalid transpilation)
#include "../utils.h"
#include "../constants.h"

extern any transporter;
any generateRandomString(any length);

any extractEmails(any text);

void sendVerifyEmail(any address, any regCode);

extern std::shared_ptr<Action> userRegistration;
#endif
