#include "ctx-embeddings.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getContextualizationPrompt(const std::string& docContent, const std::string& chunkContent, auto minTokens, auto maxTokens, auto promptTemplate) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!docContent || !chunkContent) {
        std::cout << "Document content or chunk content is missing for contextualization." << std::endl;
        return "Error: Document or chunk content missing.";
    }

    // Estimate if the chunk is already large relative to our target size
    const auto chunkTokens = Math.ceil(chunkContent.size() / DEFAULT_CHARS_PER_TOKEN);

    // If the chunk is already large, adjust the target max tokens to avoid excessive growth
    if (chunkTokens > maxTokens * 0.7) {
        // Allow for only ~30% growth for large chunks
        maxTokens = Math.ceil(chunkTokens * 1.3);
        minTokens = chunkTokens;
    }

    return promptTemplate;
    .replace("{doc_content}", docContent);
    .replace("{chunk_content}", chunkContent);
    .replace("{min_tokens}", std::to_string(minTokens));
    .replace("{max_tokens}", std::to_string(maxTokens));

}

void getCachingContextualizationPrompt(const std::string& chunkContent, std::optional<std::string> contentType, auto minTokens, auto maxTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    prompt: string; systemPrompt: string
}

std::string getPromptForMimeType(const std::string& mimeType, const std::string& docContent, const std::string& chunkContent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto minTokens = CONTEXT_TARGETS.DEFAULT.MIN_TOKENS;
    auto maxTokens = CONTEXT_TARGETS.DEFAULT.MAX_TOKENS;
    auto promptTemplate = CONTEXTUAL_CHUNK_ENRICHMENT_PROMPT_TEMPLATE;

    // Determine document type and apply appropriate settings
    if (mimeType.includes('pdf')) {
        // Check if PDF contains mathematical content
        if (containsMathematicalContent(docContent)) {
            minTokens = CONTEXT_TARGETS.MATH_PDF.MIN_TOKENS;
            maxTokens = CONTEXT_TARGETS.MATH_PDF.MAX_TOKENS;
            promptTemplate = MATH_PDF_PROMPT_TEMPLATE;
            console.debug('Using mathematical PDF prompt template');
            } else {
                minTokens = CONTEXT_TARGETS.PDF.MIN_TOKENS;
                maxTokens = CONTEXT_TARGETS.PDF.MAX_TOKENS;
                console.debug('Using standard PDF settings');
            }
            } else if (;
            (std::find(mimeType.begin(), mimeType.end(), "javascript") != mimeType.end()) ||;
            (std::find(mimeType.begin(), mimeType.end(), "typescript") != mimeType.end()) ||;
            (std::find(mimeType.begin(), mimeType.end(), "python") != mimeType.end()) ||;
            (std::find(mimeType.begin(), mimeType.end(), "java") != mimeType.end()) ||;
            (std::find(mimeType.begin(), mimeType.end(), "c++") != mimeType.end()) ||;
            (std::find(mimeType.begin(), mimeType.end(), "code") != mimeType.end());
            ) {
                minTokens = CONTEXT_TARGETS.CODE.MIN_TOKENS;
                maxTokens = CONTEXT_TARGETS.CODE.MAX_TOKENS;
                promptTemplate = CODE_PROMPT_TEMPLATE;
                console.debug('Using code prompt template');
                } else if (;
                isTechnicalDocumentation(docContent) ||;
                (std::find(mimeType.begin(), mimeType.end(), "markdown") != mimeType.end()) ||;
                (std::find(mimeType.begin(), mimeType.end(), "text/html") != mimeType.end());
                ) {
                    minTokens = CONTEXT_TARGETS.TECHNICAL.MIN_TOKENS;
                    maxTokens = CONTEXT_TARGETS.TECHNICAL.MAX_TOKENS;
                    promptTemplate = TECHNICAL_PROMPT_TEMPLATE;
                    // Using technical documentation prompt template
                }

                return getContextualizationPrompt(docContent, chunkContent, minTokens, maxTokens, promptTemplate);

}

void getCachingPromptForMimeType(const std::string& mimeType, const std::string& chunkContent) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    prompt: string; systemPrompt: string
}

bool containsMathematicalContent(const std::string& content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check for LaTeX-style math notation
    const auto latexMathPatterns = [;
    /\$\$.+?\$\$/s, // Display math: $$ ... $$
    /\$.+?\$/g, // Inline math: $ ... $
    /\\begin\{equation\}/, // LaTeX equation environment;
    /\\begin\{align\}/, // LaTeX align environment;
    /\\sum_/, // Summation;
    /\\int/, // Integral;
    /\\frac\{/, // Fraction;
    /\\sqrt\{/, // Square root;
    /\\alpha|\\beta|\\gamma|\\delta|\\theta|\\lambda|\\sigma/, // Greek letters;
    /\\nabla|\\partial/, // Differential operators;
    ];

    // Check for common non-LaTeX mathematical patterns
    const auto generalMathPatterns = [;
    /[≠≤≥±∞∫∂∑∏√∈∉⊆⊇⊂⊃∪∩]/, // Mathematical symbols;
    /\b[a-zA-Z]\^[0-9]/, // Simple exponents (e.g., x^2);
    /\(\s*-?\d+(\.\d+)?\s*,\s*-?\d+(\.\d+)?\s*\)/, // Coordinates;
    /\b[xyz]\s*=\s*-?\d+(\.\d+)?/, // Simple equations;
    /\[\s*-?\d+(\.\d+)?\s*,\s*-?\d+(\.\d+)?\s*\]/, // Vectors/matrices;
    /\b\d+\s*×\s*\d+/, // Dimensions with × symbol;
    ];

    // Test for LaTeX patterns
    for (const auto& pattern : latexMathPatterns)
        if (pattern.test(content)) {
            return true;
        }
    }

    // Test for general math patterns
    for (const auto& pattern : generalMathPatterns)
        if (pattern.test(content)) {
            return true;
        }
    }

    // Keyword analysis
    const auto mathKeywords = [;
    "theorem",
    "lemma",
    "proof",
    "equation",
    "function",
    "derivative",
    "integral",
    "matrix",
    "vector",
    "algorithm",
    "constraint",
    "coefficient",
    ];

    const auto contentLower = content.toLowerCase();
    const auto mathKeywordCount = mathKeywords.filter((keyword) => (std::find(contentLower.begin(), contentLower.end(), keyword) != contentLower.end())).size();

    // If multiple math keywords are present, it likely contains math
    return mathKeywordCount >= 2;
    }

    /**
    * Determines if a document is technical documentation based on heuristics.
    *
    * @param content - The document content to analyze.
    * @returns True if the document appears to be technical documentation.
    */
    function isTechnicalDocumentation(content: string): boolean {
        // Technical documentation patterns
        const auto technicalPatterns = [;
        /\b(version|v)\s*\d+\.\d+(\.\d+)?/i, // Version numbers;
        /\b(api|sdk|cli)\b/i, // Technical acronyms;
        /\b(http|https|ftp):\/\//i, // URLs
        /\b(GET|POST|PUT|DELETE)\b/, // HTTP methods;
        /<\/?[a-z][\s\S]*>/i, // HTML/XML tags;
        /\bREADME\b|\bCHANGELOG\b/i, // Common doc file names;
        /\b(config|configuration)\b/i, // Configuration references;
        /\b(parameter|param|argument|arg)\b/i, // Parameter references;
        ];

        // Check for common technical documentation headings
        const auto docHeadings = [;
        /\b(Introduction|Overview|Getting Started|Installation|Usage|API Reference|Troubleshooting)\b/i,
        ];

        // Check for patterns that suggest it's documentation
        for (const auto& pattern : [...technicalPatterns, ...docHeadings])
            if (pattern.test(content)) {
                return true;
            }
        }

        // Check for patterns of numbered or bullet point lists which are common in documentation
        const auto listPatterns = [;
        /\d+\.\s.+\n\d+\.\s.+/, // Numbered lists;
        /•\s.+\n•\s.+/, // Bullet points with •;
        /\*\s.+\n\*\s.+/, // Bullet points with *;
        /-\s.+\n-\s.+/, // Bullet points with -;
        ];

        for (const auto& pattern : listPatterns)
            if (pattern.test(content)) {
                return true;
            }
        }

        return false;
    }

    /**
    * Combines the original chunk content with its generated contextual enrichment.
    *
    * @param chunkContent - The original content of the chunk.
    * @param generatedContext - The contextual enrichment generated by the LLM.
    * @returns The enriched chunk, or the original chunkContent if the enrichment is empty.
    */
    export function getChunkWithContext(chunkContent: string, generatedContext: string): string {
        if (!generatedContext || generatedContext.trim() == '') {
            std::cout << "Generated context is empty. Falling back to original chunk content." << std::endl;
            return chunkContent;
        }

        return generatedContext.trim();
    }
}

} // namespace elizaos
