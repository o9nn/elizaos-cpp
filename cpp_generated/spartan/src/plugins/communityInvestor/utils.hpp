#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_UTILS_H
#include "core.h"
#include "fast-xml-parser.h"
#include "zod.h"
#include "zod-to-json-schema.h"
using zodToJsonSchema = _default;

any getZodJsonSchema(std::shared_ptr<ZodType<any>> schema);

any extractXMLFromResponse(string output, string tag);

any parseRecommendationsResponse(string xmlResponse);

any parseTokensResponse(string xmlResponse);

any parseConfirmationResponse(string xmlResponse);

any parseSignalResponse(string xmlResponse);

any parseTokenResponse(string xmlResponse);

#endif
