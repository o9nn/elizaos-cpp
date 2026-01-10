#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_CTX-EMBEDDINGS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-KNOWLEDGE_SRC_CTX-EMBEDDINGS_H
#include "core.h"

extern double DEFAULT_CHUNK_TOKEN_SIZE;
extern double DEFAULT_CHUNK_OVERLAP_TOKENS;
extern double DEFAULT_CHARS_PER_TOKEN;
extern object CONTEXT_TARGETS;
extern string SYSTEM_PROMPT;
extern object SYSTEM_PROMPTS;
extern string CONTEXTUAL_CHUNK_ENRICHMENT_PROMPT_TEMPLATE;
extern string CACHED_CHUNK_PROMPT_TEMPLATE;
extern string CACHED_CODE_CHUNK_PROMPT_TEMPLATE;
extern string CACHED_MATH_PDF_PROMPT_TEMPLATE;
extern string CACHED_TECHNICAL_PROMPT_TEMPLATE;
extern string MATH_PDF_PROMPT_TEMPLATE;
extern string CODE_PROMPT_TEMPLATE;
extern string TECHNICAL_PROMPT_TEMPLATE;
string getContextualizationPrompt(string docContent, string chunkContent, double minTokens = CONTEXT_TARGETS["DEFAULT"]["MIN_TOKENS"], double maxTokens = CONTEXT_TARGETS["DEFAULT"]["MAX_TOKENS"], string promptTemplate = CONTEXTUAL_CHUNK_ENRICHMENT_PROMPT_TEMPLATE);

object getCachingContextualizationPrompt(string chunkContent, string contentType = undefined, double minTokens = CONTEXT_TARGETS["DEFAULT"]["MIN_TOKENS"], double maxTokens = CONTEXT_TARGETS["DEFAULT"]["MAX_TOKENS"]);

string getPromptForMimeType(string mimeType, string docContent, string chunkContent);

object getCachingPromptForMimeType(string mimeType, string chunkContent);

boolean containsMathematicalContent(string content);

boolean isTechnicalDocumentation(string content);

string getChunkWithContext(string chunkContent, string generatedContext);

#endif
