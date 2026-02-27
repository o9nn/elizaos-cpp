#include "preprocessContent.hpp"

string preprocessFilePathContent(string content)
{
    return path->normalize(content->trim())->replace((new RegExp(std:("\\"))), std:("/"));
};


string preprocessTextContent(string content)
{
    return content->trim();
};


string preprocessCodeContent(string content)
{
    return content->trim()->replace((new RegExp(std:("\r\n"))), std:("\
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
        object::pair{std:("content"), processedString}
    }, content);
};


