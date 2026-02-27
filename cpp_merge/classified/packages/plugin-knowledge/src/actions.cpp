#include "actions.hpp"

std::shared_ptr<Action> processKnowledgeAction = object{
    object::pair{std:("name"), std:("PROCESS_KNOWLEDGE")}, 
    object::pair{std:("description"), std:("Process and store knowledge from a file path or text content into the knowledge base")}, 
    object::pair{std:("similes"), array<any>()}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Process the document at /path/to/document.pdf")}
        }}
    }, object{
        object::pair{std:("name"), std:("assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll process the document at /path/to/document.pdf and add it to my knowledge base.")}, 
            object::pair{std:("actions"), array<string>{ std:("PROCESS_KNOWLEDGE") }}
        }}
    } }, array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Add this to your knowledge: The capital of France is Paris.")}
        }}
    }, object{
        object::pair{std:("name"), std:("assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll add that information to my knowledge base.")}, 
            object::pair{std:("actions"), array<string>{ std:("PROCESS_KNOWLEDGE") }}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto knowledgeKeywords = array<string>{ std:("process"), std:("add"), std:("upload"), std:("document"), std:("knowledge"), std:("learn"), std:("remember"), std:("store"), std:("ingest"), std:("file") };
        auto hasKeyword = knowledgeKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
        auto pathPattern = (new RegExp(std:("(?:\/[\w.-]+)+|(?:[a-zA-Z]:[\\/][\w\s.-]+(?:[\\/][\w\s.-]+)*")));
        auto hasPath = pathPattern->test(text);
        auto service = runtime->getService(KnowledgeService->serviceType);
        if (!service) {
            logger->warn(std:("Knowledge service not available for PROCESS_KNOWLEDGE action"));
            return false;
        }
        return OR((hasKeyword), (hasPath));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<KnowledgeService>>(KnowledgeService->serviceType);
            if (!service) {
                throw any(std::make_shared<Error>(std:("Knowledge service not available")));
            }
            auto text = OR((message->content->text), (string_empty));
            auto pathPattern = (new RegExp(std:("(?:\/[\w.-]+)+|(?:[a-zA-Z]:[\\/][\w\s.-]+(?:[\\/][\w\s.-]+)*")));
            auto pathMatch = text["match"](pathPattern);
            std::shared_ptr<Content> response;
            if (pathMatch) {
                auto filePath = const_(pathMatch)[0];
                if (!fs::existsSync(filePath)) {
                    response = object{
                        object::pair{std:("text"), std:("I couldn't find the file at ") + filePath + std:(". Please check the path and try again.")}
                    };
                    if (callback) {
                        std::async([=]() { callback(response); });
                    }
                    return std::shared_ptr<Promise<void>>();
                }
                auto fileBuffer = fs::readFileSync(filePath);
                auto fileName = path->basename(filePath);
                auto fileExt = path->extname(filePath)->toLowerCase();
                auto contentType = std:("text/plain");
                if (fileExt == std:(".pdf")) contentType = std:("application/pdf"); else if (fileExt == std:(".docx")) contentType = std:("application/vnd.openxmlformats-officedocument.wordprocessingml.document"); else if (fileExt == std:(".doc")) contentType = std:("application/msword"); else if ((array<string>{ std:(".txt"), std:(".md"), std:(".tson"), std:(".xml"), std:(".csv") })->includes(fileExt)) contentType = std:("text/plain");
                auto knowledgeOptions = object{
                    object::pair{std:("clientDocumentId"), stringToUuid(runtime->agentId + fileName + Date->now())}, 
                    object::pair{std:("contentType"), std:("contentType")}, 
                    object::pair{std:("originalFilename"), fileName}, 
                    object::pair{std:("worldId"), runtime->agentId}, 
                    object::pair{std:("content"), fileBuffer->toString(std:("base64"))}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("entityId"), message->entityId}
                };
                auto result = std::async([=]() { service->addKnowledge(knowledgeOptions); });
                response = object{
                    object::pair{std:("text"), std:("I've successfully processed the document "") + fileName + std:("". It has been split into ") + result->fragmentCount + std:(" searchable fragments and added to my knowledge base.")}
                };
            } else {
                auto knowledgeContent = text["replace"]((new RegExp(std:("^(add|store|remember|process|learn)\s+(this|that|the following)?:?\s*"))), string_empty)["trim"]();
                if (!knowledgeContent) {
                    response = object{
                        object::pair{std:("text"), std:("I need some content to add to my knowledge base. Please provide text or a file path.")}
                    };
                    if (callback) {
                        std::async([=]() { callback(response); });
                    }
                    return std::shared_ptr<Promise<void>>();
                }
                auto knowledgeOptions = object{
                    object::pair{std:("clientDocumentId"), stringToUuid(runtime->agentId + std:("text") + Date->now() + std:("user-knowledge"))}, 
                    object::pair{std:("contentType"), std:("text/plain")}, 
                    object::pair{std:("originalFilename"), std:("user-knowledge.txt")}, 
                    object::pair{std:("worldId"), runtime->agentId}, 
                    object::pair{std:("content"), knowledgeContent}, 
                    object::pair{std:("roomId"), message->roomId}, 
                    object::pair{std:("entityId"), message->entityId}
                };
                auto result = std::async([=]() { service->addKnowledge(knowledgeOptions); });
                response = object{
                    object::pair{std:("text"), std:("I've added that information to my knowledge base. It has been stored and indexed for future reference.")}
                };
            }
            if (callback) {
                std::async([=]() { callback(response); });
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Error in PROCESS_KNOWLEDGE action:"), error);
            auto errorResponse = object{
                object::pair{std:("text"), std:("I encountered an error while processing the knowledge: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty}
            };
            if (callback) {
                std::async([=]() { callback(errorResponse); });
            }
        }
    }
    }
};
std::shared_ptr<Action> searchKnowledgeAction = object{
    object::pair{std:("name"), std:("SEARCH_KNOWLEDGE")}, 
    object::pair{std:("description"), std:("Search the knowledge base for specific information")}, 
    object::pair{std:("similes"), array<string>{ std:("search knowledge"), std:("find information"), std:("look up"), std:("query knowledge base"), std:("search documents"), std:("find in knowledge") }}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("user")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Search your knowledge for information about quantum computing")}
        }}
    }, object{
        object::pair{std:("name"), std:("assistant")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I'll search my knowledge base for information about quantum computing.")}, 
            object::pair{std:("actions"), array<string>{ std:("SEARCH_KNOWLEDGE") }}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto searchKeywords = array<string>{ std:("search"), std:("find"), std:("look up"), std:("query"), std:("what do you know about") };
        auto knowledgeKeywords = array<string>{ std:("knowledge"), std:("information"), std:("document"), std:("database") };
        auto hasSearchKeyword = searchKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
        auto hasKnowledgeKeyword = knowledgeKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
        auto service = runtime->getService(KnowledgeService->serviceType);
        if (!service) {
            return false;
        }
        return AND((hasSearchKeyword), (hasKnowledgeKeyword));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<KnowledgeService>>(KnowledgeService->serviceType);
            if (!service) {
                throw any(std::make_shared<Error>(std:("Knowledge service not available")));
            }
            shared text = OR((message->content->text), (string_empty));
            auto query = text["replace"]((new RegExp(std:("^(search|find|look up|query)\s+(your\s+)?knowledge\s+(base\s+)?(for\s+)?"))), string_empty)["trim"]();
            if (!query) {
                auto response = object{
                    object::pair{std:("text"), std:("What would you like me to search for in my knowledge base?")}
                };
                if (callback) {
                    std::async([=]() { callback(response); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            auto searchMessage = utils::assign(object{
                , 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), query}
                }}
            }, message);
            auto results = std::async([=]() { service->getKnowledge(searchMessage); });
            std::shared_ptr<Content> response;
            if (results->length == 0) {
                response = object{
                    object::pair{std:("text"), std:("I couldn't find any information about "") + query + std:("" in my knowledge base.")}
                };
            } else {
                auto formattedResults = results->slice(0, 3)->map([=](auto item, auto index) mutable
                {
                    return string_empty + (index + 1) + std:(". ") + item["content"]->text + string_empty;
                }
                )->join(std:("\
\
"));
                response = object{
                    object::pair{std:("text"), std:("Here's what I found about "") + query + std:("":\
\
") + formattedResults + string_empty}
                };
            }
            if (callback) {
                std::async([=]() { callback(response); });
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Error in SEARCH_KNOWLEDGE action:"), error);
            auto errorResponse = object{
                object::pair{std:("text"), std:("I encountered an error while searching the knowledge base: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty}
            };
            if (callback) {
                std::async([=]() { callback(errorResponse); });
            }
        }
    }
    }
};
array<any> knowledgeActions = array<any>{ processKnowledgeAction, searchKnowledgeAction };

void Main(void)
{
}

MAIN
