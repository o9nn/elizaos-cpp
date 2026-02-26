#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_UTILS_XML-PARSER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_UTILS_XML-PARSER_H
#include "core.h"
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
