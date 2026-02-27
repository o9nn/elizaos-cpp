#include "JSDocValidator.hpp"

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
        console->log(std:("[✓] JSDoc comment in ") + fileName + std:(" was fixed using regex patterns"));
        return fixedComment;
    }
    console->log(std:("[❌] JSDoc comment in ") + fileName + std:(" regex patterns failed, making AI call for help"));
    try
    {
        auto regeneratedComment = std::async([=]() { this->regenerateJSDoc(code); });
        auto codeWithRegeneratedComment = code->replace(originalComment, regeneratedComment);
        if (this->isValidTypeScript(codeWithRegeneratedComment)) {
            console->log(std:("[✓] JSDoc comment in ") + fileName + std:(" was regenerated using AI"));
            return regeneratedComment;
        }
    }
    catch (const any& error)
    {
        console->error(std:("Error during AI regeneration for ") + fileName + std:(":"), error);
    }
    console->warn(std:("[⚠️] HUMAN INTERVENTION NEEDED - Invalid JSDoc in ") + fileName + string_empty);
    console->warn(std:("Original comment:"), originalComment);
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
    comment = comment->replace((new RegExp(std:("^"""[\s\S]*?\"))), string_empty);
    comment = comment->replace((new RegExp(std:("\n""""))), string_empty);
    auto fixes = array<array<any>>{ array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\/\*\*?(?!\*"))), std:("/**") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\*{3,}"))), std:("**") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\*(?!\s|\*|\/)"))), std:("* ") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("^(\s*)\*\s\s+/"))), std:("$1* ") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\*\/\s*\n\s*\*\*\/"))), std:("*/") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\n\s*\*\s*\n\s*\*\/"))), std:("\
 */") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\*+\/"))), std:("*/") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("(?<!\s)\*\/"))), std:(" */") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\*\/\s+\*\/"))), std:("*/") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\n\*"))), std:("\
 *") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("^\s*\*\s*$/"))), std:(" *") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\s+$/"))), string_empty }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("@(param|returns?|throws?|example)\s{2,}"))), std:("@$1 ") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\{(\w+)\}"))), std:("{ $1 }") }, array<std::shared_ptr<RegExp>>{ (new RegExp(std:("\r\n"))), std:("\
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
    auto prompt = std:("Fix the following JSDoc comment to be syntactically valid.\
        Ensure proper formatting:\
        - Start with /**\
        - Each line should start with a single *\
        - End with */\
        - No extra asterisks\
        - Space after each asterisk\
        - Space before closing tag\
\
        Code:\
        ") + code + std:("\
\
        Return ONLY the fixed JSDoc comment, nothing else.");
    return std::async([=]() { this->aiService->generateComment(prompt); });
}

