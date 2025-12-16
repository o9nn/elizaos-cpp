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

;

/**
 * Schema for AI summary configuration
 */
const AISummaryConfigSchema = z.object({
  enabled: z.boolean().default(false),
  defaultModel: z.string(),
  models: z.object({
    day: z.string(),
    week: z.string(),
    month: z.string(),
  }),
  apiKey: z.string(),
  endpoint: z.string().default("https://openrouter.ai/api/v1/chat/completions"),
  temperature: z.number().default(0.1),
  max_tokens: z.number().default(200),
  projectContext: z.string().default("An open source project on GitHub."),
});

// Inferred type from the schema
using AISummaryConfig = z.infer<typeof AISummaryConfigSchema>;

} // namespace elizaos
