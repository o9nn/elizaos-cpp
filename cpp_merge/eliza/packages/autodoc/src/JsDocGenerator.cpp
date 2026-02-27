#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/autodoc/src/JsDocGenerator.h"

JsDocGenerator::JsDocGenerator(std::shared_ptr<AIService> aiService_) : aiService(aiService_)  {
}

std::shared_ptr<Promise<string>> JsDocGenerator::generateComment(std::shared_ptr<ASTQueueItem> queueItem)
{
    auto prompt = this->buildPrompt(queueItem);
    auto comment = std::async([=]() { this->aiService->generateComment(prompt); });
    return comment;
}

std::shared_ptr<Promise<string>> JsDocGenerator::generateClassComment(std::shared_ptr<ASTQueueItem> queueItem)
{
    auto prompt = this->buildClassPrompt(queueItem);
    auto comment = std::async([=]() { this->aiService->generateComment(prompt); });
    return comment;
}

string JsDocGenerator::buildPrompt(std::shared_ptr<ASTQueueItem> queueItem)
{
    return std:("Generate JSDoc comment for the following code:\
\
\
        """typescript\
        ") + queueItem->code + std:("\
        """\
\
        Only return the JSDoc comment, not the code itself.\
        ");
}

string JsDocGenerator::buildClassPrompt(std::shared_ptr<ASTQueueItem> queueItem)
{
    return std:("Generate JSDoc comment for the following Class:\
\
        Class name: ") + const_(queueItem->code->match((new RegExp(std:("class (\w+")))))[1] + std:("\
\
        Only return the JSDoc for the Class itself, not the methods or anything in the class.\
\
        Only return the JSDoc comment for the class, no other text or code.\
\
        Example:\
        """\
        /**\
         * This is a class that does something. It has a method that does something.\
         */\
        """\
        ");
}

