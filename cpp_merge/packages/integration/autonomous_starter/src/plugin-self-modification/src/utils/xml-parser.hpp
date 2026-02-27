#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SELF_MODIFICATION_SRC_UTILS_XML_PARSER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SELF_MODIFICATION_SRC_UTILS_XML_PARSER_H
#include "core.hpp"
#include "fast-xml-parser.h"
#include "../types.h"

extern any parser;
extern any builder;
string sanitizeXml(string xmlString);

string escapeXml(string unsafe);

any isValidOperationType(string type);

std::shared_ptr<CharacterDiff> parseCharacterDiff(string xmlString);

string buildCharacterDiffXml(std::shared_ptr<CharacterDiff> diff);

#endif
