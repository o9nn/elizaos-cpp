#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-knowledge/src/actions.h"

std::shared_ptr<Action> processKnowledgeAction = object{
    object::pair{std::string("name"), std::string("PROCESS_KNOWLEDGE")}, 
    object::pair{std::string("description"), std::string("Process and store knowledge from a file path or text content into the knowledge base")}, 
    object::pair{std::string("similes"), array<any>()}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Process the document at /path/to/document.pdf")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll process the document at /path/to/document.pdf and add it to my knowledge base.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("PROCESS_KNOWLEDGE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Add this to your knowledge: The capital of France is Paris.")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll add that information to my knowledge base.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("PROCESS_KNOWLEDGE") }}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto knowledgeKeywords = array<string>{ std::string("process"), std::string("add"), std::string("upload"), std::string("document"), std::string("knowledge"), std::string("learn"), std::string("remember"), std::string("store"), std::string("ingest"), std::string("file") };
        auto hasKeyword = knowledgeKeywords->some([=](auto keyword) mutable
        {
            return text["includes"](keyword);
        }
        );
        auto pathPattern = (new RegExp(std::string("(?:\/[\w.-]+)+|(?:[a-zA-Z]:[\\/][\w\s.-]+(?:[\\/][\w\s.-]+)*")));
        auto hasPath = pathPattern->test(text);
        auto service = runtime->getService(KnowledgeService->serviceType);
        if (!service) {
            logger->warn(std::string("Knowledge service not available for PROCESS_KNOWLEDGE action"));
            return false;
        }
        return OR((hasKeyword), (hasPath));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<KnowledgeService>>(KnowledgeService->serviceType);
            if (!service) {
                throw any(std::make_shared<Error>(std::string("Knowledge service not available")));
            }
            auto text = OR((message->content->text), (string_empty));
            auto pathPattern = (new RegExp(std::string("(?:\/[\w.-]+)+|(?:[a-zA-Z]:[\\/][\w\s.-]+(?:[\\/][\w\s.-]+)*")));
            auto pathMatch = text["match"](pathPattern);
            std::shared_ptr<Content> response;
            if (pathMatch) {
                auto filePath = const_(pathMatch)[0];
                if (!fs::existsSync(filePath)) {
                    response = object{
                        object::pair{std::string("text"), std::string("I couldn't find the file at ") + filePath + std::string(". Please check the path and try again.")}
                    };
                    if (callback) {
                        std::async([=]() { callback(response); });
                    }
                    return std::shared_ptr<Promise<void>>();
                }
                auto fileBuffer = fs::readFileSync(filePath);
                auto fileName = path->basename(filePath);
                auto fileExt = path->extname(filePath)->toLowerCase();
                auto contentType = std::string("text/plain");
                if (fileExt == std::string(".pdf")) contentType = std::string("application/pdf"); else if (fileExt == std::string(".docx")) contentType = std::string("application/vnd.openxmlformats-officedocument.wordprocessingml.document"); else if (fileExt == std::string(".doc")) contentType = std::string("application/msword"); else if ((array<string>{ std::string(".txt"), std::string(".md"), std::string(".tson"), std::string(".xml"), std::string(".csv") })->includes(fileExt)) contentType = std::string("text/plain");
                auto knowledgeOptions = object{
                    object::pair{std::string("clientDocumentId"), stringToUuid(runtime->agentId + fileName + Date->now())}, 
                    object::pair{std::string("contentType"), std::string("contentType")}, 
                    object::pair{std::string("originalFilename"), fileName}, 
                    object::pair{std::string("worldId"), runtime->agentId}, 
                    object::pair{std::string("content"), fileBuffer->toString(std::string("base64"))}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("entityId"), message->entityId}
                };
                auto result = std::async([=]() { service->addKnowledge(knowledgeOptions); });
                response = object{
                    object::pair{std::string("text"), std::string("I've successfully processed the document "") + fileName + std::string("". It has been split into ") + result->fragmentCount + std::string(" searchable fragments and added to my knowledge base.")}
                };
            } else {
                auto knowledgeContent = text["replace"]((new RegExp(std::string("^(add|store|remember|process|learn)\s+(this|that|the following)?:?\s*"))), string_empty)["trim"]();
                if (!knowledgeContent) {
                    response = object{
                        object::pair{std::string("text"), std::string("I need some content to add to my knowledge base. Please provide text or a file path.")}
                    };
                    if (callback) {
                        std::async([=]() { callback(response); });
                    }
                    return std::shared_ptr<Promise<void>>();
                }
                auto knowledgeOptions = object{
                    object::pair{std::string("clientDocumentId"), stringToUuid(runtime->agentId + std::string("text") + Date->now() + std::string("user-knowledge"))}, 
                    object::pair{std::string("contentType"), std::string("text/plain")}, 
                    object::pair{std::string("originalFilename"), std::string("user-knowledge.txt")}, 
                    object::pair{std::string("worldId"), runtime->agentId}, 
                    object::pair{std::string("content"), knowledgeContent}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("entityId"), message->entityId}
                };
                auto result = std::async([=]() { service->addKnowledge(knowledgeOptions); });
                response = object{
                    object::pair{std::string("text"), std::string("I've added that information to my knowledge base. It has been stored and indexed for future reference.")}
                };
            }
            if (callback) {
                std::async([=]() { callback(response); });
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in PROCESS_KNOWLEDGE action:"), error);
            auto errorResponse = object{
                object::pair{std::string("text"), std::string("I encountered an error while processing the knowledge: ") + (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error")) + string_empty}
            };
            if (callback) {
                std::async([=]() { callback(errorResponse); });
            }
        }
    }
    }
};
std::shared_ptr<Action> searchKnowledgeAction = object{
    object::pair{std::string("name"), std::string("SEARCH_KNOWLEDGE")}, 
    object::pair{std::string("description"), std::string("Search the knowledge base for specific information")}, 
    object::pair{std::string("similes"), array<string>{ std::string("search knowledge"), std::string("find information"), std::string("look up"), std::string("query knowledge base"), std::string("search documents"), std::string("find in knowledge") }}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("user")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Search your knowledge for information about quantum computing")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("assistant")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll search my knowledge base for information about quantum computing.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("SEARCH_KNOWLEDGE") }}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        shared text = OR((message->content->text->toLowerCase()), (string_empty));
        auto searchKeywords = array<string>{ std::string("search"), std::string("find"), std::string("look up"), std::string("query"), std::string("what do you know about") };
        auto knowledgeKeywords = array<string>{ std::string("knowledge"), std::string("information"), std::string("document"), std::string("database") };
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
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<KnowledgeService>>(KnowledgeService->serviceType);
            if (!service) {
                throw any(std::make_shared<Error>(std::string("Knowledge service not available")));
            }
            shared text = OR((message->content->text), (string_empty));
            auto query = text["replace"]((new RegExp(std::string("^(search|find|look up|query)\s+(your\s+)?knowledge\s+(base\s+)?(for\s+)?"))), string_empty)["trim"]();
            if (!query) {
                auto response = object{
                    object::pair{std::string("text"), std::string("What would you like me to search for in my knowledge base?")}
                };
                if (callback) {
                    std::async([=]() { callback(response); });
                }
                return std::shared_ptr<Promise<void>>();
            }
            auto searchMessage = utils::assign(object{
                , 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), query}
                }}
            }, message);
            auto results = std::async([=]() { service->getKnowledge(searchMessage); });
            std::shared_ptr<Content> response;
            if (results->length == 0) {
                response = object{
                    object::pair{std::string("text"), std::string("I couldn't find any information about "") + query + std::string("" in my knowledge base.")}
                };
            } else {
                auto formattedResults = results->slice(0, 3)->map([=](auto item, auto index) mutable
                {
                    return string_empty + (index + 1) + std::string(". ") + item["content"]->text + string_empty;
                }
                )->join(std::string("\
\
"));
                response = object{
                    object::pair{std::string("text"), std::string("Here's what I found about "") + query + std::string("":\
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
            logger->error(std::string("Error in SEARCH_KNOWLEDGE action:"), error);
            auto errorResponse = object{
                object::pair{std::string("text"), std::string("I encountered an error while searching the knowledge base: ") + (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error")) + string_empty}
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
