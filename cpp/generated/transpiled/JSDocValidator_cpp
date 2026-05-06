#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/autodoc/src/JSDocValidator.h"

JSDocValidator::JSDocValidator(std::shared_ptr<AIService> aiService_) : aiService(aiService_)  {
}

std::shared_ptr<Promise<string>> JSDocValidator::validateAndFixJSDoc(string fileName, string code, string originalComment)
{
    if (this->isValidTypeScript(code)) {
        return originalComment;
    }
    auto fixedComment = this->fixCommonJSDocIssues(originalComment);
    auto codeWithFixedComment = code->replace(originalComment, fixedComment);
    if (this->isValidTypeScript(codeWithFixedComment)) {
        console->log(std::string("✓ JSDoc comment in ") + fileName + std::string(" was fixed using regex patterns"));
        return fixedComment;
    }
    console->log(std::string("❌JSDoc comment in ") + fileName + std::string(" regex patterns failed, making AI call for help"));
    try
    {
        auto regeneratedComment = std::async([=]() { this->regenerateJSDoc(code); });
        auto codeWithRegeneratedComment = code->replace(originalComment, regeneratedComment);
        if (this->isValidTypeScript(codeWithRegeneratedComment)) {
            console->log(std::string("✓ JSDoc comment in ") + fileName + std::string(" was regenerated using AI"));
            return regeneratedComment;
        }
    }
    catch (const any& error)
    {
        console->error(std::string("Error during AI regeneration for ") + fileName + std::string(":"), error);
    }
    console->warn(std::string("⚠️ HUMAN INTERVENTION NEEDED - Invalid JSDoc in ") + fileName + string_empty);
    console->warn(std::string("Original comment:"), originalComment);
    return originalComment;
}

boolean JSDocValidator::isValidTypeScript(string code)
{
    try
    {
        parse(code, this->parserOptions);
        return true;
    }
    catch (const any& _error)
    {
        return false;
    }
}

string JSDocValidator::fixCommonJSDocIssues(string comment)
{
    comment = comment->replace((new RegExp(std::string("^```[\s\S]*?\"))), string_empty);
    comment = comment->replace((new RegExp(std::string("\n```"))), string_empty);
    auto fixes = array<array<any>>{ array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\/\*\*?(?!\*"))), std::string("/**") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\*{3,}"))), std::string("**") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\*(?!\s|\*|\/)"))), std::string("* ") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("^(\s*)\*\s\s+/"))), std::string("$1* ") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\*\/\s*\n\s*\*\*\/"))), std::string("*/") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\n\s*\*\s*\n\s*\*\/"))), std::string("\
 */") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\*+\/"))), std::string("*/") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("(?<!\s)\*\/"))), std::string(" */") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\*\/\s+\*\/"))), std::string("*/") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\n\*"))), std::string("\
 *") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("^\s*\*\s*$/"))), std::string(" *") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\s+$/"))), string_empty }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("@(param|returns?|throws?|example)\s{2,}"))), std::string("@$1 ") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\{(\w+)\}"))), std::string("{ $1 }") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("\r\n"))), std::string("\
") } };
    auto fixed = comment;
    for (auto& [pattern, replacement] : fixes)
    {
        fixed = fixed->replace(pattern, as<string>(replacement));
    }
    return fixed;
}

std::shared_ptr<Promise<string>> JSDocValidator::regenerateJSDoc(string code)
{
    auto prompt = std::string("Fix the following JSDoc comment to be syntactically valid.\
        Ensure proper formatting:\
        - Start with /**\
        - Each line should start with a single *\
        - End with */\
        - No extra asterisks\
        - Space after each asterisk\
        - Space before closing tag\
\
        Code:\
        ") + code + std::string("\
\
        Return ONLY the fixed JSDoc comment, nothing else.");
    return std::async([=]() { this->aiService->generateComment(prompt); });
}

