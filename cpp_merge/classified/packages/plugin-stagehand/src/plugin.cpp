#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/plugin.h"

any extractUrl(string text)
{
    auto quotedUrlMatch = text->match((new RegExp(std::string("["']([^"']+)["'"))));
    if (AND((quotedUrlMatch), ((OR(((*const_(quotedUrlMatch))[1]->startsWith(std::string("http"))), ((*const_(quotedUrlMatch))[1]->includes(std::string(".")))))))) {
        return (*const_(quotedUrlMatch))[1];
    }
    auto urlMatch = text->match((new RegExp(std::string("(https?:\/\/[^\s]+"))));
    if (urlMatch) {
        return (*const_(urlMatch))[1];
    }
    auto domainMatch = text->match((new RegExp(std::string("(?:go to|navigate to|open|visit)\s+([a-zA-Z0-9][a-zA-Z0-9-]{0,61}[a-zA-Z0-9]?\.[a-zA-Z]{2,})"))));
    if (domainMatch) {
        return std::string("https://") + (*const_(domainMatch))[1] + string_empty;
    }
    return nullptr;
};


std::shared_ptr<Promise<void>> testStagehandConnection(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        logger->info(std::string("[Stagehand] Running Google.com test to verify browser automation..."));
        auto service = nullptr;
        auto retries = 0;
        auto maxRetries = 5;
        while (AND((!service), (retries < maxRetries)))
        {
            service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                if (retries == maxRetries - 1) {
                    logger->warn(std::string("[Stagehand] Service not available after retries. The plugin may not be fully initialized."));
                    return std::shared_ptr<Promise<void>>();
                }
                shared waitTime = Math->pow(2, retries) * 1000;
                logger->debug(std::string("[Stagehand] Service not available yet, retrying in ") + waitTime + std::string("ms..."));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, waitTime);
                }
                ); });
                retries++;
            }
        }
        if (!service) {
            logger->warn(std::string("[Stagehand] Service not available for test"));
            return std::shared_ptr<Promise<void>>();
        }
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, 2000);
        }
        ); });
        auto sessionId = std::string("test-google-") + Date->now() + string_empty;
        auto session = std::async([=]() { service->createSession(sessionId); });
        auto client = service->getClient();
        auto result = std::async([=]() { client->navigate(session->id, std::string("https://www.google.com")); });
        logger->info(std::string("[Stagehand] Successfully navigated to Google.com:"), object{
            object::pair{std::string("url"), result["url"]}, 
            object::pair{std::string("title"), result["title"]}
        });
        std::async([=]() { service->destroySession(sessionId); });
        logger->info(std::string("[Stagehand] Google.com test completed successfully! Browser automation is working."));
    }
    catch (const any& error)
    {
        logger->error(std::string("[Stagehand] Google.com test failed:"), error);
    }
};


any configSchema = z->object(object{
    object::pair{std::string("BROWSERBASE_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("BROWSERBASE_PROJECT_ID"), z->string()->optional()}, 
    object::pair{std::string("OPENAI_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("ANTHROPIC_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("OLLAMA_BASE_URL"), z->string()->optional()}, 
    object::pair{std::string("OLLAMA_MODEL"), z->string()->optional()}, 
    object::pair{std::string("BROWSER_HEADLESS"), z->string()->transform([=](auto val) mutable
    {
        return val == std::string("true");
    }
    )->optional()->default(std::string("true"))}, 
    object::pair{std::string("CAPSOLVER_API_KEY"), z->string()->optional()}, 
    object::pair{std::string("STAGEHAND_SERVER_PORT"), z->string()->optional()->default(std::string("3456"))}
});
std::shared_ptr<Action> browserNavigateAction = object{
    object::pair{std::string("name"), std::string("BROWSER_NAVIGATE")}, 
    object::pair{std::string("similes"), array<string>{ std::string("GO_TO_URL"), std::string("OPEN_WEBSITE"), std::string("VISIT_PAGE"), std::string("NAVIGATE_TO") }}, 
    object::pair{std::string("description"), std::string("Navigate the browser to a specified URL. Can be chained with BROWSER_EXTRACT to get content or BROWSER_SCREENSHOT to capture the page")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std::string("ENABLE_BROWSER")) == std::string("true")), (runtime->getSetting(std::string("BROWSER_ENABLED")) == std::string("true")));
        if (!browserEnabled) {
            logger->debug(std::string("Browser capability disabled in settings."));
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            logger->debug(std::string("Stagehand service not available."));
            return false;
        }
        auto url = extractUrl(OR((message->content->text), (string_empty)));
        return url != nullptr;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        try
        {
            logger->info(std::string("Handling BROWSER_NAVIGATE action"));
            shared service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std::string("navigate to the requested page"));
                return object{
                    object::pair{std::string("text"), std::string("Browser service is not available")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_NAVIGATE")}, 
                        object::pair{std::string("error"), std::string("service_not_available")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("service_not_available")}
                    }}
                };
            }
            shared url = extractUrl(OR((message->content->text), (string_empty)));
            if (!url) {
                auto error = std::make_shared<StagehandError>(std::string("No URL found in message"), std::string("NO_URL_FOUND"), std::string("I couldn't find a URL in your request. Please provide a valid URL to navigate to."), false);
                handleBrowserError(error, callback, std::string("navigate to a page"));
                return object{
                    object::pair{std::string("text"), std::string("I couldn't find a URL in your request. Please provide a valid URL to navigate to.")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_NAVIGATE")}, 
                        object::pair{std::string("error"), std::string("no_url_found")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("no_url_found")}
                    }}
                };
            }
            try
            {
                validateSecureAction(url, defaultUrlValidator);
            }
            catch (const any& error)
            {
                if (is<BrowserSecurityError>(error)) {
                    handleBrowserError(error, callback);
                    return object{
                        object::pair{std::string("text"), std::string("Security error: Cannot navigate to restricted URL")}, 
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("data"), object{
                            object::pair{std::string("actionName"), std::string("BROWSER_NAVIGATE")}, 
                            object::pair{std::string("error"), std::string("security_error")}, 
                            object::pair{std::string("url"), std::string("url")}
                        }}, 
                        object::pair{std::string("values"), object{
                            object::pair{std::string("success"), false}, 
                            object::pair{std::string("errorType"), std::string("security_error")}
                        }}
                    };
                }
                throw any(error);
            }
            shared session = std::async([=]() { service->getCurrentSession(); });
            if (!session) {
                auto sessionId = std::string("session-") + Date->now() + string_empty;
                session = std::async([=]() { service->createSession(sessionId); });
            }
            auto result = std::async([=]() { retryWithBackoff([=]() mutable
            {
                auto client = service->getClient();
                return std::async([=]() { client->navigate(session->id, url); });
            }
            , browserRetryConfigs["navigation"], std::string("navigate to ") + url + string_empty); });
            auto responseContent = object{
                object::pair{std::string("text"), std::string("I've navigated to ") + url + std::string(". The page title is: "") + result->title + std::string(""")}, 
                object::pair{std::string("actions"), array<string>{ std::string("BROWSER_NAVIGATE") }}, 
                object::pair{std::string("source"), message->content->source}
            };
            std::async([=]() { callback(responseContent); });
            std::async([=]() { runtime->emitEvent(EventType->BROWSER_ACTION_PERFORMED, object{
                object::pair{std::string("action"), std::string("navigation")}, 
                object::pair{std::string("url"), result->url}, 
                object::pair{std::string("title"), result->title}
            }); });
            return object{
                object::pair{std::string("text"), responseContent->text}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_NAVIGATE")}, 
                    object::pair{std::string("url"), result->url}, 
                    object::pair{std::string("title"), result->title}, 
                    object::pair{std::string("sessionId"), session->id}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("url"), result->url}, 
                    object::pair{std::string("pageTitle"), result->title}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in BROWSER_NAVIGATE action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserNavigationError>(OR((extractUrl(OR((message->content->text), (string_empty)))), (std::string("the requested page"))), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std::string("text"), std::string("Failed to navigate to the requested page")}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_NAVIGATE")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("unknown_error"))}, 
                    object::pair{std::string("url"), OR((extractUrl(OR((message->content->text), (string_empty)))), (std::string("unknown")))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("errorType"), std::string("navigation_error")}
                }}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Go to google.com")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've navigated to https://google.com. The page title is: "Google"")}, 
            object::pair{std::string("actions"), array<string>{ std::string("BROWSER_NAVIGATE") }}
        }}
    } } }}
};
std::shared_ptr<Action> browserClickAction = object{
    object::pair{std::string("name"), std::string("BROWSER_CLICK")}, 
    object::pair{std::string("similes"), array<string>{ std::string("CLICK_ELEMENT"), std::string("TAP"), std::string("PRESS_BUTTON") }}, 
    object::pair{std::string("description"), std::string("Click on an element on the webpage")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Click on the search button")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've clicked on the search button.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("BROWSER_CLICK") }}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std::string("ENABLE_BROWSER")) == std::string("true")), (runtime->getSetting(std::string("BROWSER_ENABLED")) == std::string("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return OR((OR((text["includes"](std::string("click"))), (text["includes"](std::string("tap"))))), (text["includes"](std::string("press"))));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std::string("click on element"));
                return object{
                    object::pair{std::string("text"), std::string("Browser service is not available")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_CLICK")}, 
                        object::pair{std::string("error"), std::string("service_not_available")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std::string("No active browser session"));
                handleBrowserError(error, callback, std::string("click on element"));
                return object{
                    object::pair{std::string("text"), std::string("No active browser session")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_CLICK")}, 
                        object::pair{std::string("error"), std::string("no_session")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("no_session")}
                    }}
                };
            }
            auto text = OR((message->content->text), (string_empty));
            auto match = text["match"]((new RegExp(std::string("click (?:on |the )?(.+)$"))));
            auto description = (match) ? any(const_(match)[1]) : any(std::string("element"));
            auto result = std::async([=]() { service->getClient()->click(session->id, description); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std::string("click"), description, std::make_shared<Error>(OR((result->error), (std::string("Click failed"))))));
            }
            auto responseContent = object{
                object::pair{std::string("text"), std::string("I've successfully clicked on "") + description + std::string(""")}, 
                object::pair{std::string("actions"), array<string>{ std::string("BROWSER_CLICK") }}, 
                object::pair{std::string("source"), OR((message->content->source), (std::string("action")))}
            };
            std::async([=]() { callback(responseContent); });
            std::async([=]() { runtime->emitEvent(EventType->BROWSER_ACTION_PERFORMED, object{
                object::pair{std::string("action"), std::string("click")}, 
                object::pair{std::string("element"), description}
            }); });
            auto lowerDesc = description["toLowerCase"]();
            if (OR((OR((OR((lowerDesc["includes"](std::string("submit"))), (lowerDesc["includes"](std::string("send"))))), (lowerDesc["includes"](std::string("post"))))), (lowerDesc["includes"](std::string("save"))))) {
                std::async([=]() { runtime->emitEvent(EventType->FORM_SUBMITTED, object{
                    object::pair{std::string("details"), object{
                        object::pair{std::string("element"), description}, 
                        object::pair{std::string("action"), std::string("click")}
                    }}
                }); });
            }
            return object{
                object::pair{std::string("text"), responseContent->text}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_CLICK")}, 
                    object::pair{std::string("element"), description}, 
                    object::pair{std::string("sessionId"), session->id}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("element"), description}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in BROWSER_CLICK action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std::string("click"), std::string("element"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std::string("text"), std::string("Failed to click on the requested element")}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_CLICK")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("unknown_error"))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("errorType"), std::string("click_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Action> browserTypeAction = object{
    object::pair{std::string("name"), std::string("BROWSER_TYPE")}, 
    object::pair{std::string("similes"), array<string>{ std::string("TYPE_TEXT"), std::string("INPUT"), std::string("ENTER_TEXT") }}, 
    object::pair{std::string("description"), std::string("Type text into an input field on the webpage")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Type "hello world" in the search box")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've typed "hello world" in the search box.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("BROWSER_TYPE") }}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std::string("ENABLE_BROWSER")) == std::string("true")), (runtime->getSetting(std::string("BROWSER_ENABLED")) == std::string("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return OR((OR((text["includes"](std::string("type"))), (text["includes"](std::string("input"))))), (text["includes"](std::string("enter"))));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std::string("type text"));
                return object{
                    object::pair{std::string("text"), std::string("Browser service is not available")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_TYPE")}, 
                        object::pair{std::string("error"), std::string("service_not_available")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std::string("No active browser session"));
                handleBrowserError(error, callback, std::string("type text"));
                return object{
                    object::pair{std::string("text"), std::string("No active browser session")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_TYPE")}, 
                        object::pair{std::string("error"), std::string("no_session")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("no_session")}
                    }}
                };
            }
            auto text = OR((message->content->text), (string_empty));
            auto match = text["match"]((new RegExp(std::string("["']([^"']+)["'"))));
            auto textToType = (match) ? any(const_(match)[1]) : any(string_empty);
            auto fieldMatch = text["match"]((new RegExp(std::string("(?:in|into) (?:the )?(.+)$"))));
            auto field = (fieldMatch) ? any(const_(fieldMatch)[1]) : any(std::string("input field"));
            if (!textToType) {
                throw any(std::make_shared<BrowserActionError>(std::string("type"), field, std::make_shared<Error>(std::string("No text specified to type"))));
            }
            auto result = std::async([=]() { service->getClient()->type(session->id, textToType, field); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std::string("type"), field, std::make_shared<Error>(OR((result->error), (std::string("Type failed"))))));
            }
            auto responseContent = object{
                object::pair{std::string("text"), std::string("I've typed "") + textToType + std::string("" in the ") + field + string_empty}, 
                object::pair{std::string("actions"), array<string>{ std::string("BROWSER_TYPE") }}, 
                object::pair{std::string("source"), OR((message->content->source), (std::string("action")))}
            };
            std::async([=]() { callback(responseContent); });
            std::async([=]() { runtime->emitEvent(EventType->BROWSER_ACTION_PERFORMED, object{
                object::pair{std::string("action"), std::string("type")}, 
                object::pair{std::string("field"), std::string("field")}, 
                object::pair{std::string("text"), textToType}
            }); });
            if (OR((field["toLowerCase"]()["includes"](std::string("form"))), (field["toLowerCase"]()["includes"](std::string("submit"))))) {
                std::async([=]() { runtime->emitEvent(EventType->FORM_SUBMITTED, object{
                    object::pair{std::string("details"), object{
                        object::pair{std::string("field"), std::string("field")}, 
                        object::pair{std::string("text"), textToType}, 
                        object::pair{std::string("action"), std::string("type")}
                    }}
                }); });
            }
            return object{
                object::pair{std::string("text"), responseContent->text}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_TYPE")}, 
                    object::pair{std::string("textTyped"), textToType}, 
                    object::pair{std::string("field"), std::string("field")}, 
                    object::pair{std::string("sessionId"), session->id}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("textTyped"), textToType}, 
                    object::pair{std::string("field"), std::string("field")}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in BROWSER_TYPE action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std::string("type"), std::string("input field"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std::string("text"), std::string("Failed to type text")}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_TYPE")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("unknown_error"))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("errorType"), std::string("type_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Action> browserSelectAction = object{
    object::pair{std::string("name"), std::string("BROWSER_SELECT")}, 
    object::pair{std::string("similes"), array<string>{ std::string("SELECT_OPTION"), std::string("CHOOSE"), std::string("PICK") }}, 
    object::pair{std::string("description"), std::string("Select an option from a dropdown on the webpage")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Select "United States" from the country dropdown")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've selected "United States" from the country dropdown.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("BROWSER_SELECT") }}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std::string("ENABLE_BROWSER")) == std::string("true")), (runtime->getSetting(std::string("BROWSER_ENABLED")) == std::string("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return OR((OR((text["includes"](std::string("select"))), (text["includes"](std::string("choose"))))), (text["includes"](std::string("pick"))));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std::string("select option"));
                return object{
                    object::pair{std::string("text"), std::string("Browser service is not available")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_SELECT")}, 
                        object::pair{std::string("error"), std::string("service_not_available")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std::string("No active browser session"));
                handleBrowserError(error, callback, std::string("select option"));
                return object{
                    object::pair{std::string("text"), std::string("No active browser session")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_SELECT")}, 
                        object::pair{std::string("error"), std::string("no_session")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("no_session")}
                    }}
                };
            }
            auto text = OR((message->content->text), (string_empty));
            auto match = text["match"]((new RegExp(std::string("["']([^"']+)["'"))));
            auto option = (match) ? any(const_(match)[1]) : any(string_empty);
            auto dropdownMatch = text["match"]((new RegExp(std::string("from (?:the )?(.+)$"))));
            auto dropdown = (dropdownMatch) ? any(const_(dropdownMatch)[1]) : any(std::string("dropdown"));
            if (!option) {
                throw any(std::make_shared<BrowserActionError>(std::string("select"), dropdown, std::make_shared<Error>(std::string("No option specified to select"))));
            }
            auto result = std::async([=]() { service->getClient()->select(session->id, option, dropdown); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std::string("select"), dropdown, std::make_shared<Error>(OR((result->error), (std::string("Select failed"))))));
            }
            auto responseContent = object{
                object::pair{std::string("text"), std::string("I've selected "") + option + std::string("" from the ") + dropdown + string_empty}, 
                object::pair{std::string("actions"), array<string>{ std::string("BROWSER_SELECT") }}, 
                object::pair{std::string("source"), OR((message->content->source), (std::string("action")))}
            };
            std::async([=]() { callback(responseContent); });
            return object{
                object::pair{std::string("text"), responseContent->text}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_SELECT")}, 
                    object::pair{std::string("option"), std::string("option")}, 
                    object::pair{std::string("dropdown"), std::string("dropdown")}, 
                    object::pair{std::string("sessionId"), session->id}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("option"), std::string("option")}, 
                    object::pair{std::string("dropdown"), std::string("dropdown")}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in BROWSER_SELECT action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std::string("select"), std::string("dropdown"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std::string("text"), std::string("Failed to select option")}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_SELECT")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("unknown_error"))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("errorType"), std::string("select_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Action> browserExtractAction = object{
    object::pair{std::string("name"), std::string("BROWSER_EXTRACT")}, 
    object::pair{std::string("similes"), array<string>{ std::string("EXTRACT_DATA"), std::string("GET_TEXT"), std::string("SCRAPE") }}, 
    object::pair{std::string("description"), std::string("Extract data from the webpage")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Extract the main heading from the page")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I extracted the main heading: "Welcome to Our Website"")}, 
            object::pair{std::string("actions"), array<string>{ std::string("BROWSER_EXTRACT") }}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std::string("ENABLE_BROWSER")) == std::string("true")), (runtime->getSetting(std::string("BROWSER_ENABLED")) == std::string("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return (OR((OR((OR((OR((text["includes"](std::string("extract"))), (text["includes"](std::string("get"))))), (text["includes"](std::string("scrape"))))), (text["includes"](std::string("find"))))), (text["includes"](std::string("read")))));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std::string("extract data"));
                return object{
                    object::pair{std::string("text"), std::string("Browser service is not available")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_EXTRACT")}, 
                        object::pair{std::string("error"), std::string("service_not_available")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std::string("No active browser session"));
                handleBrowserError(error, callback, std::string("extract data"));
                return object{
                    object::pair{std::string("text"), std::string("No active browser session")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_EXTRACT")}, 
                        object::pair{std::string("error"), std::string("no_session")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("no_session")}
                    }}
                };
            }
            auto text = OR((message->content->text), (string_empty));
            auto match = text["match"]((new RegExp(std::string("(?:extract|get|find|scrape|read) (?:the )?(.+?)(?:\s+from|\s*$)"))));
            auto instruction = (match) ? const_(match)[1] : text;
            auto result = std::async([=]() { service->getClient()->extract(session->id, instruction); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std::string("extract"), std::string("page"), std::make_shared<Error>(OR((result->error), (std::string("Extraction failed"))))));
            }
            auto extractedData = result->data;
            auto foundText = OR((extractedData->data), (std::string("No data found")));
            auto found = OR((extractedData->found), (false));
            auto responseContent = object{
                object::pair{std::string("text"), (found) ? std::string("I found the ") + instruction + std::string(": "") + foundText + std::string(""") : std::string("I couldn't find the requested ") + instruction + std::string(" on the page.")}, 
                object::pair{std::string("actions"), array<string>{ std::string("BROWSER_EXTRACT") }}, 
                object::pair{std::string("source"), OR((message->content->source), (std::string("action")))}
            };
            std::async([=]() { callback(responseContent); });
            return object{
                object::pair{std::string("text"), responseContent->text}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_EXTRACT")}, 
                    object::pair{std::string("instruction"), std::string("instruction")}, 
                    object::pair{std::string("found"), std::string("found")}, 
                    object::pair{std::string("data"), foundText}, 
                    object::pair{std::string("sessionId"), session->id}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("found"), std::string("found")}, 
                    object::pair{std::string("data"), foundText}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in BROWSER_EXTRACT action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std::string("extract"), std::string("page"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std::string("text"), std::string("Failed to extract data from the page")}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_EXTRACT")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("unknown_error"))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("errorType"), std::string("extract_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Action> browserScreenshotAction = object{
    object::pair{std::string("name"), std::string("BROWSER_SCREENSHOT")}, 
    object::pair{std::string("similes"), array<string>{ std::string("TAKE_SCREENSHOT"), std::string("CAPTURE_PAGE"), std::string("SCREENSHOT") }}, 
    object::pair{std::string("description"), std::string("Take a screenshot of the current page")}, 
    object::pair{std::string("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Take a screenshot of the page")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've taken a screenshot of the page.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("BROWSER_SCREENSHOT") }}
        }}
    } } }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std::string("ENABLE_BROWSER")) == std::string("true")), (runtime->getSetting(std::string("BROWSER_ENABLED")) == std::string("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return OR((OR((text["includes"](std::string("screenshot"))), (text["includes"](std::string("capture"))))), (text["includes"](std::string("snap"))));
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std::string("take screenshot"));
                return object{
                    object::pair{std::string("text"), std::string("Browser service is not available")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_SCREENSHOT")}, 
                        object::pair{std::string("error"), std::string("service_not_available")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std::string("No active browser session"));
                handleBrowserError(error, callback, std::string("take screenshot"));
                return object{
                    object::pair{std::string("text"), std::string("No active browser session")}, 
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("actionName"), std::string("BROWSER_SCREENSHOT")}, 
                        object::pair{std::string("error"), std::string("no_session")}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("errorType"), std::string("no_session")}
                    }}
                };
            }
            auto result = std::async([=]() { service->getClient()->screenshot(session->id); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std::string("screenshot"), std::string("page"), std::make_shared<Error>(OR((result->error), (std::string("Screenshot failed"))))));
            }
            auto screenshotData = result->data;
            auto url = OR((screenshotData->url), (std::string("unknown")));
            auto title = OR((screenshotData->title), (std::string("Untitled")));
            auto responseContent = object{
                object::pair{std::string("text"), std::string("I've taken a screenshot of the page "") + title + std::string("" at ") + url + string_empty}, 
                object::pair{std::string("actions"), array<string>{ std::string("BROWSER_SCREENSHOT") }}, 
                object::pair{std::string("source"), OR((message->content->source), (std::string("action")))}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("screenshot"), screenshotData->screenshot}, 
                    object::pair{std::string("mimeType"), OR((screenshotData->mimeType), (std::string("image/png")))}, 
                    object::pair{std::string("url"), std::string("url")}, 
                    object::pair{std::string("title"), std::string("title")}
                }}
            };
            std::async([=]() { callback(responseContent); });
            return object{
                object::pair{std::string("text"), responseContent->text}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_SCREENSHOT")}, 
                    object::pair{std::string("url"), std::string("url")}, 
                    object::pair{std::string("title"), std::string("title")}, 
                    object::pair{std::string("sessionId"), session->id}, 
                    object::pair{std::string("screenshot"), screenshotData->screenshot}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("url"), std::string("url")}, 
                    object::pair{std::string("title"), std::string("title")}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in BROWSER_SCREENSHOT action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std::string("screenshot"), std::string("page"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std::string("text"), std::string("Failed to take screenshot")}, 
                object::pair{std::string("success"), false}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("actionName"), std::string("BROWSER_SCREENSHOT")}, 
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("unknown_error"))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("errorType"), std::string("screenshot_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Provider> browserStateProvider = object{
    object::pair{std::string("name"), std::string("BROWSER_STATE")}, 
    object::pair{std::string("description"), std::string("Provides current browser state information including active session status, current page URL, and page title")}, 
    object::pair{std::string("get"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        auto session = std::async([=]() { service->getCurrentSession(); });
        if (OR((!session), (!service))) {
            return object{
                object::pair{std::string("text"), std::string("No active browser session")}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("hasSession"), false}
                }}, 
                object::pair{std::string("data"), object{}}
            };
        }
        try
        {
            auto client = service->getClient();
            auto state = std::async([=]() { client->getState(session->id); });
            return object{
                object::pair{std::string("text"), std::string("Current browser page: "") + state->title + std::string("" at ") + state->url + string_empty}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("hasSession"), true}, 
                    object::pair{std::string("url"), state->url}, 
                    object::pair{std::string("title"), state->title}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("sessionId"), session->id}, 
                    object::pair{std::string("createdAt"), session->createdAt}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("Error getting browser state:"), error);
            return object{
                object::pair{std::string("text"), std::string("Error getting browser state")}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("hasSession"), true}, 
                    object::pair{std::string("error"), true}
                }}, 
                object::pair{std::string("data"), object{}}
            };
        }
    }
    }
};
std::shared_ptr<Plugin> stagehandPlugin = object{
    object::pair{std::string("name"), std::string("plugin-stagehand")}, 
    object::pair{std::string("description"), std::string("Browser automation plugin using Stagehand - stagehand is goated for web interactions")}, 
    object::pair{std::string("config"), object{
        object::pair{std::string("BROWSERBASE_API_KEY"), process->env->BROWSERBASE_API_KEY}, 
        object::pair{std::string("BROWSERBASE_PROJECT_ID"), process->env->BROWSERBASE_PROJECT_ID}, 
        object::pair{std::string("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
        object::pair{std::string("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}, 
        object::pair{std::string("OLLAMA_BASE_URL"), process->env->OLLAMA_BASE_URL}, 
        object::pair{std::string("OLLAMA_MODEL"), process->env->OLLAMA_MODEL}, 
        object::pair{std::string("BROWSER_HEADLESS"), process->env->BROWSER_HEADLESS}, 
        object::pair{std::string("CAPSOLVER_API_KEY"), process->env->CAPSOLVER_API_KEY}, 
        object::pair{std::string("STAGEHAND_SERVER_PORT"), process->env->STAGEHAND_SERVER_PORT}
    }}, 
    , 
    object::pair{std::string("services"), array<StagehandService>{ StagehandService }}, 
    object::pair{std::string("actions"), array<any>{ browserNavigateAction, browserClickAction, browserTypeAction, browserSelectAction, browserExtractAction, browserScreenshotAction }}, 
    object::pair{std::string("providers"), array<any>{ browserStateProvider }}
};

void Main(void)
{
}

MAIN
