#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_CTX-EMBEDDINGS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_CTX-EMBEDDINGS_H
#include "core.h"

extern double DEFAULT_CHUNK_TOKEN_SIZE;
extern double DEFAULT_CHUNK_OVERLAP_TOKENS;
extern double DEFAULT_CHARS_PER_TOKEN;
extern object CONTEXT_TARGETS;
extern std::string SYSTEM_PROMPT;
extern object SYSTEM_PROMPTS;
extern std::string CONTEXTUAL_CHUNK_ENRICHMENT_PROMPT_TEMPLATE;
extern std::string CACHED_CHUNK_PROMPT_TEMPLATE;
extern std::string CACHED_CODE_CHUNK_PROMPT_TEMPLATE;
extern std::string CACHED_MATH_PDF_PROMPT_TEMPLATE;
extern std::string CACHED_TECHNICAL_PROMPT_TEMPLATE;
extern std::string MATH_PDF_PROMPT_TEMPLATE;
extern std::string CODE_PROMPT_TEMPLATE;
extern std::string TECHNICAL_PROMPT_TEMPLATE;
std::string getContextualizationPrompt(std::string docContent, std::string chunkContent, double minTokens = CONTEXT_TARGETS["DEFAULT"]["MIN_TOKENS"], double maxTokens = CONTEXT_TARGETS["DEFAULT"]["MAX_TOKENS"], std::string promptTemplate = CONTEXTUAL_CHUNK_ENRICHMENT_PROMPT_TEMPLATE);

object getCachingContextualizationPrompt(std::string chunkContent, std::string contentType = undefined, double minTokens = CONTEXT_TARGETS["DEFAULT"]["MIN_TOKENS"], double maxTokens = CONTEXT_TARGETS["DEFAULT"]["MAX_TOKENS"]);

std::string getPromptForMimeType(std::string mimeType, std::string docContent, std::string chunkContent);

object getCachingPromptForMimeType(std::string mimeType, std::string chunkContent);

boolean containsMathematicalContent(std::string content);

boolean isTechnicalDocumentation(std::string content);

std::string getChunkWithContext(std::string chunkContent, std::string generatedContext);

#endif
