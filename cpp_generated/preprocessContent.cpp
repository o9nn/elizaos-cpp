#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/preprocessContent.h"

string preprocessFilePathContent(string content)
{
    return path->normalize(content->trim())->replace((new RegExp(std::string("\\"))), std::string("/"));
};


string preprocessTextContent(string content)
{
    return content->trim();
};


string preprocessCodeContent(string content)
{
    return content->trim()->replace((new RegExp(std::string("\r\n"))), std::string("\
"));
};


string preprocessLabelContent(string content)
{
    return content->trim()->toLowerCase();
};


MatchContent preprocessContent(MatchContent content)
{
    string processedString;
    switch (content["contentType"])
    {
    case TagPatternType::FILE_PATH:
        processedString = preprocessFilePathContent(content["content"]);
        break;
    case TagPatternType::COMMIT_MESSAGE:
    case TagPatternType::PR_TITLE:
    case TagPatternType::PR_DESCRIPTION:
    case TagPatternType::ISSUE_TITLE:
    case TagPatternType::ISSUE_BODY:
    case TagPatternType::COMMENT:
        processedString = preprocessTextContent(content["content"]);
        break;
    case TagPatternType::CODE_CONTENT:
        processedString = preprocessCodeContent(content["content"]);
        break;
    case TagPatternType::LABEL:
        processedString = preprocessLabelContent(content["content"]);
        break;
    default:
        processedString = content["content"];
        break;
    }
    return utils::assign(object{
        , 
        object::pair{std::string("content"), processedString}
    }, content);
};


