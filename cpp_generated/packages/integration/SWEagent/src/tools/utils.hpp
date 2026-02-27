#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_TOOLS_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_TOOLS_UTILS_H
#include "core.h"
#include "./commands.h"

string guardMultilineInput(string action, std::function<any(string)> matchFct);

boolean shouldQuote(any value, std::shared_ptr<Command> command);

string getSignature(std::shared_ptr<Command> cmd);

string generateCommandDocs(array<std::shared_ptr<Command>> commands, array<string> subroutineTypes, Record<string, any> _kwargs);

string formatCommand(std::shared_ptr<Command> cmd);

#endif
