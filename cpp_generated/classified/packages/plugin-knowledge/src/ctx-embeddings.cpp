#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-knowledge/src/ctx-embeddings.h"

string getContextualizationPrompt(string docContent, string chunkContent, double minTokens, double maxTokens, string promptTemplate)
{
    if (OR((!docContent), (!chunkContent))) {
        console->warn(std::string("Document content or chunk content is missing for contextualization."));
        return std::string("Error: Document or chunk content missing.");
    }
    auto chunkTokens = Math->ceil(chunkContent->get_length() / DEFAULT_CHARS_PER_TOKEN);
    if (chunkTokens > maxTokens * 0.7) {
        maxTokens = Math->ceil(chunkTokens * 1.3);
        minTokens = chunkTokens;
    }
    return promptTemplate->replace(std::string("{doc_content}"), docContent)->replace(std::string("{chunk_content}"), chunkContent)->replace(std::string("{min_tokens}"), minTokens->toString())->replace(std::string("{max_tokens}"), maxTokens->toString());
};


object getCachingContextualizationPrompt(string chunkContent, string contentType, double minTokens, double maxTokens)
{
    if (!chunkContent) {
        console->warn(std::string("Chunk content is missing for contextualization."));
        return object{
            object::pair{std::string("prompt"), std::string("Error: Chunk content missing.")}, 
            object::pair{std::string("systemPrompt"), SYSTEM_PROMPTS["DEFAULT"]}
        };
    }
    auto chunkTokens = Math->ceil(chunkContent->get_length() / DEFAULT_CHARS_PER_TOKEN);
    if (chunkTokens > maxTokens * 0.7) {
        maxTokens = Math->ceil(chunkTokens * 1.3);
        minTokens = chunkTokens;
    }
    auto promptTemplate = CACHED_CHUNK_PROMPT_TEMPLATE;
    auto systemPrompt = SYSTEM_PROMPTS["DEFAULT"];
    if (contentType) {
        if (OR((OR((OR((OR((OR((contentType->includes(std::string("javascript"))), (contentType->includes(std::string("typescript"))))), (contentType->includes(std::string("python"))))), (contentType->includes(std::string("java"))))), (contentType->includes(std::string("c++"))))), (contentType->includes(std::string("code"))))) {
            promptTemplate = CACHED_CODE_CHUNK_PROMPT_TEMPLATE;
            systemPrompt = SYSTEM_PROMPTS["CODE"];
        } else if (contentType->includes(std::string("pdf"))) {
            if (containsMathematicalContent(chunkContent)) {
                promptTemplate = CACHED_MATH_PDF_PROMPT_TEMPLATE;
                systemPrompt = SYSTEM_PROMPTS["MATH_PDF"];
            } else {
                systemPrompt = SYSTEM_PROMPTS["PDF"];
            }
        } else if (OR((OR((contentType->includes(std::string("markdown"))), (contentType->includes(std::string("text/html"))))), (isTechnicalDocumentation(chunkContent)))) {
            promptTemplate = CACHED_TECHNICAL_PROMPT_TEMPLATE;
            systemPrompt = SYSTEM_PROMPTS["TECHNICAL"];
        }
    }
    auto formattedPrompt = promptTemplate->replace(std::string("{chunk_content}"), chunkContent)->replace(std::string("{min_tokens}"), minTokens->toString())->replace(std::string("{max_tokens}"), maxTokens->toString());
    return object{
        object::pair{std::string("prompt"), formattedPrompt}, 
        object::pair{std::string("systemPrompt"), std::string("systemPrompt")}
    };
};


string getPromptForMimeType(string mimeType, string docContent, string chunkContent)
{
    auto minTokens = CONTEXT_TARGETS["DEFAULT"]["MIN_TOKENS"];
    auto maxTokens = CONTEXT_TARGETS["DEFAULT"]["MAX_TOKENS"];
    auto promptTemplate = CONTEXTUAL_CHUNK_ENRICHMENT_PROMPT_TEMPLATE;
    if (mimeType->includes(std::string("pdf"))) {
        if (containsMathematicalContent(docContent)) {
            minTokens = CONTEXT_TARGETS["MATH_PDF"]["MIN_TOKENS"];
            maxTokens = CONTEXT_TARGETS["MATH_PDF"]["MAX_TOKENS"];
            promptTemplate = MATH_PDF_PROMPT_TEMPLATE;
            console->debug(std::string("Using mathematical PDF prompt template"));
        } else {
            minTokens = CONTEXT_TARGETS["PDF"]["MIN_TOKENS"];
            maxTokens = CONTEXT_TARGETS["PDF"]["MAX_TOKENS"];
            console->debug(std::string("Using standard PDF settings"));
        }
    } else if (OR((OR((OR((OR((OR((mimeType->includes(std::string("javascript"))), (mimeType->includes(std::string("typescript"))))), (mimeType->includes(std::string("python"))))), (mimeType->includes(std::string("java"))))), (mimeType->includes(std::string("c++"))))), (mimeType->includes(std::string("code"))))) {
        minTokens = CONTEXT_TARGETS["CODE"]["MIN_TOKENS"];
        maxTokens = CONTEXT_TARGETS["CODE"]["MAX_TOKENS"];
        promptTemplate = CODE_PROMPT_TEMPLATE;
        console->debug(std::string("Using code prompt template"));
    } else if (OR((OR((isTechnicalDocumentation(docContent)), (mimeType->includes(std::string("markdown"))))), (mimeType->includes(std::string("text/html"))))) {
        minTokens = CONTEXT_TARGETS["TECHNICAL"]["MIN_TOKENS"];
        maxTokens = CONTEXT_TARGETS["TECHNICAL"]["MAX_TOKENS"];
        promptTemplate = TECHNICAL_PROMPT_TEMPLATE;
    }
    return getContextualizationPrompt(docContent, chunkContent, minTokens, maxTokens, promptTemplate);
};


object getCachingPromptForMimeType(string mimeType, string chunkContent)
{
    auto minTokens = CONTEXT_TARGETS["DEFAULT"]["MIN_TOKENS"];
    auto maxTokens = CONTEXT_TARGETS["DEFAULT"]["MAX_TOKENS"];
    if (mimeType->includes(std::string("pdf"))) {
        if (containsMathematicalContent(chunkContent)) {
            minTokens = CONTEXT_TARGETS["MATH_PDF"]["MIN_TOKENS"];
            maxTokens = CONTEXT_TARGETS["MATH_PDF"]["MAX_TOKENS"];
        } else {
            minTokens = CONTEXT_TARGETS["PDF"]["MIN_TOKENS"];
            maxTokens = CONTEXT_TARGETS["PDF"]["MAX_TOKENS"];
        }
    } else if (OR((OR((OR((OR((OR((mimeType->includes(std::string("javascript"))), (mimeType->includes(std::string("typescript"))))), (mimeType->includes(std::string("python"))))), (mimeType->includes(std::string("java"))))), (mimeType->includes(std::string("c++"))))), (mimeType->includes(std::string("code"))))) {
        minTokens = CONTEXT_TARGETS["CODE"]["MIN_TOKENS"];
        maxTokens = CONTEXT_TARGETS["CODE"]["MAX_TOKENS"];
    } else if (OR((OR((isTechnicalDocumentation(chunkContent)), (mimeType->includes(std::string("markdown"))))), (mimeType->includes(std::string("text/html"))))) {
        minTokens = CONTEXT_TARGETS["TECHNICAL"]["MIN_TOKENS"];
        maxTokens = CONTEXT_TARGETS["TECHNICAL"]["MAX_TOKENS"];
    }
    return getCachingContextualizationPrompt(chunkContent, mimeType, minTokens, maxTokens);
};


boolean containsMathematicalContent(string content)
{
    auto latexMathPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\$\$.+?\$\$"))), (new RegExp(std::string("\$.+?\$"))), (new RegExp(std::string("\\begin\{equation\"))), (new RegExp(std::string("\\begin\{align\"))), (new RegExp(std::string("\\sum"))), (new RegExp(std::string("\\in"))), (new RegExp(std::string("\\frac\"))), (new RegExp(std::string("\\sqrt\"))), (new RegExp(std::string("\\alpha|\\beta|\\gamma|\\delta|\\theta|\\lambda|\\sigm"))), (new RegExp(std::string("\\nabla|\\partia"))) };
    auto generalMathPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("[≠≤≥±∞∫∂∑∏√∈∉⊆⊇⊂⊃∪∩"))), (new RegExp(std::string("\b[a-zA-Z]\^[0-9"))), (new RegExp(std::string("\(\s*-?\d+(\.\d+)?\s*,\s*-?\d+(\.\d+)?\s*\"))), (new RegExp(std::string("\b[xyz]\s*=\s*-?\d+(\.\d+)"))), (new RegExp(std::string("\[\s*-?\d+(\.\d+)?\s*,\s*-?\d+(\.\d+)?\s*\"))), (new RegExp(std::string("\b\d+\s*×\s*\d"))) };
    for (auto& pattern : latexMathPatterns)
    {
        if (pattern->test(content)) {
            return true;
        }
    }
    for (auto& pattern : generalMathPatterns)
    {
        if (pattern->test(content)) {
            return true;
        }
    }
    auto mathKeywords = array<string>{ std::string("theorem"), std::string("lemma"), std::string("proof"), std::string("equation"), std::string("function"), std::string("derivative"), std::string("integral"), std::string("matrix"), std::string("vector"), std::string("algorithm"), std::string("constraint"), std::string("coefficient") };
    shared contentLower = content->toLowerCase();
    auto mathKeywordCount = mathKeywords->filter([=](auto keyword) mutable
    {
        return contentLower->includes(keyword);
    }
    )->get_length();
    return mathKeywordCount >= 2;
};


boolean isTechnicalDocumentation(string content)
{
    auto technicalPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\b(version|v)\s*\d+\.\d+(\.\d+)?"))), (new RegExp(std::string("\b(api|sdk|cli)\b"))), (new RegExp(std::string("\b(http|https|ftp):\/\/"))), (new RegExp(std::string("\b(GET|POST|PUT|DELETE)\"))), (new RegExp(std::string("<\/?[a-z][\s\S]*>"))), (new RegExp(std::string("\bREADME\b|\bCHANGELOG\b"))), (new RegExp(std::string("\b(config|configuration)\b"))), (new RegExp(std::string("\b(parameter|param|argument|arg)\b"))) };
    auto docHeadings = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\b(Introduction|Overview|Getting Started|Installation|Usage|API Reference|Troubleshooting)\b"))) };
    for (auto& pattern : array<std::shared_ptr<RegExp>>{ technicalPatterns, docHeadings })
    {
        if (pattern->test(content)) {
            return true;
        }
    }
    auto listPatterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\d+\.\s.+\n\d+\.\s."))), (new RegExp(std::string("•\s.+\n•\s."))), (new RegExp(std::string("\*\s.+\n\*\s."))), (new RegExp(std::string("-\s.+\n-\s."))) };
    for (auto& pattern : listPatterns)
    {
        if (pattern->test(content)) {
            return true;
        }
    }
    return false;
};


string getChunkWithContext(string chunkContent, string generatedContext)
{
    if (OR((!generatedContext), (generatedContext->trim() == string_empty))) {
        console->warn(std::string("Generated context is empty. Falling back to original chunk content."));
        return chunkContent;
    }
    return generatedContext->trim();
};


double DEFAULT_CHUNK_TOKEN_SIZE = 500;
double DEFAULT_CHUNK_OVERLAP_TOKENS = 100;
double DEFAULT_CHARS_PER_TOKEN = 3.5;
object CONTEXT_TARGETS = object{
    object::pair{std::string("DEFAULT"), object{
        object::pair{std::string("MIN_TOKENS"), 60}, 
        object::pair{std::string("MAX_TOKENS"), 120}
    }}, 
    object::pair{std::string("PDF"), object{
        object::pair{std::string("MIN_TOKENS"), 80}, 
        object::pair{std::string("MAX_TOKENS"), 150}
    }}, 
    object::pair{std::string("MATH_PDF"), object{
        object::pair{std::string("MIN_TOKENS"), 100}, 
        object::pair{std::string("MAX_TOKENS"), 180}
    }}, 
    object::pair{std::string("CODE"), object{
        object::pair{std::string("MIN_TOKENS"), 100}, 
        object::pair{std::string("MAX_TOKENS"), 200}
    }}, 
    object::pair{std::string("TECHNICAL"), object{
        object::pair{std::string("MIN_TOKENS"), 80}, 
        object::pair{std::string("MAX_TOKENS"), 160}
    }}
};
string SYSTEM_PROMPT = std::string("You are a precision text augmentation tool. Your task is to expand a given text chunk with its direct context from a larger document. You must: 1) Keep the original chunk intact; 2) Add critical context from surrounding text; 3) Never summarize or rephrase the original chunk; 4) Create contextually rich output for improved semantic retrieval.");
object SYSTEM_PROMPTS = object{
    object::pair{std::string("DEFAULT"), std::string("You are a precision text augmentation tool. Your task is to expand a given text chunk with its direct context from a larger document. You must: 1) Keep the original chunk intact; 2) Add critical context from surrounding text; 3) Never summarize or rephrase the original chunk; 4) Create contextually rich output for improved semantic retrieval.")}, 
    object::pair{std::string("CODE"), std::string("You are a precision code augmentation tool. Your task is to expand a given code chunk with necessary context from the larger codebase. You must: 1) Keep the original code chunk intact with exact syntax and indentation; 2) Add relevant imports, function signatures, or class definitions; 3) Include critical surrounding code context; 4) Create contextually rich output that maintains correct syntax.")}, 
    object::pair{std::string("PDF"), std::string("You are a precision document augmentation tool. Your task is to expand a given PDF text chunk with its direct context from the larger document. You must: 1) Keep the original chunk intact; 2) Add section headings, references, or figure captions; 3) Include text that immediately precedes and follows the chunk; 4) Create contextually rich output that maintains the document's original structure.")}, 
    object::pair{std::string("MATH_PDF"), std::string("You are a precision mathematical content augmentation tool. Your task is to expand a given mathematical text chunk with essential context. You must: 1) Keep original mathematical notations and expressions exactly as they appear; 2) Add relevant definitions, theorems, or equations from elsewhere in the document; 3) Preserve all LaTeX or mathematical formatting; 4) Create contextually rich output for improved mathematical comprehension.")}, 
    object::pair{std::string("TECHNICAL"), std::string("You are a precision technical documentation augmentation tool. Your task is to expand a technical document chunk with critical context. You must: 1) Keep the original chunk intact including all technical terminology; 2) Add relevant configuration examples, parameter definitions, or API references; 3) Include any prerequisite information; 4) Create contextually rich output that maintains technical accuracy.")}
};
string CONTEXTUAL_CHUNK_ENRICHMENT_PROMPT_TEMPLATE = std::string("\
<document>\
{doc_content}\
</document>\
\
Here is the chunk we want to situate within the whole document:\
<chunk>\
{chunk_content}\
</chunk>\
\
Create an enriched version of this chunk by adding critical surrounding context. Follow these guidelines:\
\
1. Identify the document's main topic and key information relevant to understanding this chunk\
2. Include 2-3 sentences before the chunk that provide essential context\
3. Include 2-3 sentences after the chunk that complete thoughts or provide resolution\
4. For technical documents, include any definitions or explanations of terms used in the chunk\
5. For narrative content, include character or setting information needed to understand the chunk\
6. Keep the original chunk text COMPLETELY INTACT and UNCHANGED in your response\
7. Do not use phrases like "this chunk discusses" - directly present the context\
8. The total length should be between {min_tokens} and {max_tokens} tokens\
9. Format the response as a single coherent paragraph\
\
Provide ONLY the enriched chunk text in your response:");
string CACHED_CHUNK_PROMPT_TEMPLATE = std::string("\
Here is the chunk we want to situate within the whole document:\
<chunk>\
{chunk_content}\
</chunk>\
\
Create an enriched version of this chunk by adding critical surrounding context. Follow these guidelines:\
\
1. Identify the document's main topic and key information relevant to understanding this chunk\
2. Include 2-3 sentences before the chunk that provide essential context\
3. Include 2-3 sentences after the chunk that complete thoughts or provide resolution\
4. For technical documents, include any definitions or explanations of terms used in the chunk\
5. For narrative content, include character or setting information needed to understand the chunk\
6. Keep the original chunk text COMPLETELY INTACT and UNCHANGED in your response\
7. Do not use phrases like "this chunk discusses" - directly present the context\
8. The total length should be between {min_tokens} and {max_tokens} tokens\
9. Format the response as a single coherent paragraph\
\
Provide ONLY the enriched chunk text in your response:");
string CACHED_CODE_CHUNK_PROMPT_TEMPLATE = std::string("\
Here is the chunk of code we want to situate within the whole document:\
<chunk>\
{chunk_content}\
</chunk>\
\
Create an enriched version of this code chunk by adding critical surrounding context. Follow these guidelines:\
\
1. Preserve ALL code syntax, indentation, and comments exactly as they appear\
2. Include any import statements, function definitions, or class declarations that this code depends on\
3. Add necessary type definitions or interfaces that are referenced in this chunk\
4. Include any crucial comments from elsewhere in the document that explain this code\
5. If there are key variable declarations or initializations earlier in the document, include those\
6. Keep the original chunk COMPLETELY INTACT and UNCHANGED in your response\
7. The total length should be between {min_tokens} and {max_tokens} tokens\
8. Do NOT include implementation details for functions that are only called but not defined in this chunk\
\
Provide ONLY the enriched code chunk in your response:");
string CACHED_MATH_PDF_PROMPT_TEMPLATE = std::string("\
Here is the chunk we want to situate within the whole document:\
<chunk>\
{chunk_content}\
</chunk>\
\
Create an enriched version of this chunk by adding critical surrounding context. This document contains mathematical content that requires special handling. Follow these guidelines:\
\
1. Preserve ALL mathematical notation exactly as it appears in the chunk\
2. Include any defining equations, variables, or parameters mentioned earlier in the document that relate to this chunk\
3. Add section/subsection names or figure references if they help situate the chunk\
4. If variables or symbols are defined elsewhere in the document, include these definitions\
5. If mathematical expressions appear corrupted, try to infer their meaning from context\
6. Keep the original chunk text COMPLETELY INTACT and UNCHANGED in your response\
7. The total length should be between {min_tokens} and {max_tokens} tokens\
8. Format the response as a coherent mathematical explanation\
\
Provide ONLY the enriched chunk text in your response:");
string CACHED_TECHNICAL_PROMPT_TEMPLATE = std::string("\
Here is the chunk we want to situate within the whole document:\
<chunk>\
{chunk_content}\
</chunk>\
\
Create an enriched version of this chunk by adding critical surrounding context. This appears to be technical documentation that requires special handling. Follow these guidelines:\
\
1. Preserve ALL technical terminology, product names, and version numbers exactly as they appear\
2. Include any prerequisite information or requirements mentioned earlier in the document\
3. Add section/subsection headings or navigation path to situate this chunk within the document structure\
4. Include any definitions of technical terms, acronyms, or jargon used in this chunk\
5. If this chunk references specific configurations, include relevant parameter explanations\
6. Keep the original chunk text COMPLETELY INTACT and UNCHANGED in your response\
7. The total length should be between {min_tokens} and {max_tokens} tokens\
8. Format the response maintaining any hierarchical structure present in the original\
\
Provide ONLY the enriched chunk text in your response:");
string MATH_PDF_PROMPT_TEMPLATE = std::string("\
<document>\
{doc_content}\
</document>\
\
Here is the chunk we want to situate within the whole document:\
<chunk>\
{chunk_content}\
</chunk>\
\
Create an enriched version of this chunk by adding critical surrounding context. This document contains mathematical content that requires special handling. Follow these guidelines:\
\
1. Preserve ALL mathematical notation exactly as it appears in the chunk\
2. Include any defining equations, variables, or parameters mentioned earlier in the document that relate to this chunk\
3. Add section/subsection names or figure references if they help situate the chunk\
4. If variables or symbols are defined elsewhere in the document, include these definitions\
5. If mathematical expressions appear corrupted, try to infer their meaning from context\
6. Keep the original chunk text COMPLETELY INTACT and UNCHANGED in your response\
7. The total length should be between {min_tokens} and {max_tokens} tokens\
8. Format the response as a coherent mathematical explanation\
\
Provide ONLY the enriched chunk text in your response:");
string CODE_PROMPT_TEMPLATE = std::string("\
<document>\
{doc_content}\
</document>\
\
Here is the chunk of code we want to situate within the whole document:\
<chunk>\
{chunk_content}\
</chunk>\
\
Create an enriched version of this code chunk by adding critical surrounding context. Follow these guidelines:\
\
1. Preserve ALL code syntax, indentation, and comments exactly as they appear\
2. Include any import statements, function definitions, or class declarations that this code depends on\
3. Add necessary type definitions or interfaces that are referenced in this chunk\
4. Include any crucial comments from elsewhere in the document that explain this code\
5. If there are key variable declarations or initializations earlier in the document, include those\
6. Keep the original chunk COMPLETELY INTACT and UNCHANGED in your response\
7. The total length should be between {min_tokens} and {max_tokens} tokens\
8. Do NOT include implementation details for functions that are only called but not defined in this chunk\
\
Provide ONLY the enriched code chunk in your response:");
string TECHNICAL_PROMPT_TEMPLATE = std::string("\
<document>\
{doc_content}\
</document>\
\
Here is the chunk we want to situate within the whole document:\
<chunk>\
{chunk_content}\
</chunk>\
\
Create an enriched version of this chunk by adding critical surrounding context. This appears to be technical documentation that requires special handling. Follow these guidelines:\
\
1. Preserve ALL technical terminology, product names, and version numbers exactly as they appear\
2. Include any prerequisite information or requirements mentioned earlier in the document\
3. Add section/subsection headings or navigation path to situate this chunk within the document structure\
4. Include any definitions of technical terms, acronyms, or jargon used in this chunk\
5. If this chunk references specific configurations, include relevant parameter explanations\
6. Keep the original chunk text COMPLETELY INTACT and UNCHANGED in your response\
7. The total length should be between {min_tokens} and {max_tokens} tokens\
8. Format the response maintaining any hierarchical structure present in the original\
\
Provide ONLY the enriched chunk text in your response:");

void Main(void)
{
}

MAIN
