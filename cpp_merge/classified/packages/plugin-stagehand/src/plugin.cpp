#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/plugin.h"

any extractUrl(string text)
{
    auto quotedUrlMatch = text->match((new RegExp(std:("["']([^"']+)["'"))));
    if (AND((quotedUrlMatch), ((OR(((*const_(quotedUrlMatch))[1]->startsWith(std:("http"))), ((*const_(quotedUrlMatch))[1]->includes(std:(".")))))))) {
        return (*const_(quotedUrlMatch))[1];
    }
    auto urlMatch = text->match((new RegExp(std:("(https?:\/\/[^\s]+"))));
    if (urlMatch) {
        return (*const_(urlMatch))[1];
    }
    auto domainMatch = text->match((new RegExp(std:("(?:go to|navigate to|open|visit)\s+([a-zA-Z0-9][a-zA-Z0-9-]{0,61}[a-zA-Z0-9]?\.[a-zA-Z]{2,})"))));
    if (domainMatch) {
        return std:("https://") + (*const_(domainMatch))[1] + string_empty;
    }
    return nullptr;
};


std::shared_ptr<Promise<void>> testStagehandConnection(std::shared_ptr<IAgentRuntime> runtime)
{
    try
    {
        logger->info(std:("[Stagehand] Running Google.com test to verify browser automation..."));
        auto service = nullptr;
        auto retries = 0;
        auto maxRetries = 5;
        while (AND((!service), (retries < maxRetries)))
        {
            service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                if (retries == maxRetries - 1) {
                    logger->warn(std:("[Stagehand] Service not available after retries. The plugin may not be fully initialized."));
                    return std::shared_ptr<Promise<void>>();
                }
                shared waitTime = Math->pow(2, retries) * 1000;
                logger->debug(std:("[Stagehand] Service not available yet, retrying in ") + waitTime + std:("ms..."));
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, waitTime);
                }
                ); });
                retries++;
            }
        }
        if (!service) {
            logger->warn(std:("[Stagehand] Service not available for test"));
            return std::shared_ptr<Promise<void>>();
        }
        std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
        {
            return setTimeout(resolve, 2000);
        }
        ); });
        auto sessionId = std:("test-google-") + Date->now() + string_empty;
        auto session = std::async([=]() { service->createSession(sessionId); });
        auto client = service->getClient();
        auto result = std::async([=]() { client->navigate(session->id, std:("https://www.google.com")); });
        logger->info(std:("[Stagehand] Successfully navigated to Google.com:"), object{
            object::pair{std:("url"), result["url"]}, 
            object::pair{std:("title"), result["title"]}
        });
        std::async([=]() { service->destroySession(sessionId); });
        logger->info(std:("[Stagehand] Google.com test completed successfully! Browser automation is working."));
    }
    catch (const any& error)
    {
        logger->error(std:("[Stagehand] Google.com test failed:"), error);
    }
};


any configSchema = z->object(object{
    object::pair{std:("BROWSERBASE_API_KEY"), z->string()->optional()}, 
    object::pair{std:("BROWSERBASE_PROJECT_ID"), z->string()->optional()}, 
    object::pair{std:("OPENAI_API_KEY"), z->string()->optional()}, 
    object::pair{std:("ANTHROPIC_API_KEY"), z->string()->optional()}, 
    object::pair{std:("OLLAMA_BASE_URL"), z->string()->optional()}, 
    object::pair{std:("OLLAMA_MODEL"), z->string()->optional()}, 
    object::pair{std:("BROWSER_HEADLESS"), z->string()->transform([=](auto val) mutable
    {
        return val == std:("true");
    }
    )->optional()->default(std:("true"))}, 
    object::pair{std:("CAPSOLVER_API_KEY"), z->string()->optional()}, 
    object::pair{std:("STAGEHAND_SERVER_PORT"), z->string()->optional()->default(std:("3456"))}
});
std::shared_ptr<Action> browserNavigateAction = object{
    object::pair{std:("name"), std:("BROWSER_NAVIGATE")}, 
    object::pair{std:("similes"), array<string>{ std:("GO_TO_URL"), std:("OPEN_WEBSITE"), std:("VISIT_PAGE"), std:("NAVIGATE_TO") }}, 
    object::pair{std:("description"), std:("Navigate the browser to a specified URL. Can be chained with BROWSER_EXTRACT to get content or BROWSER_SCREENSHOT to capture the page")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std:("ENABLE_BROWSER")) == std:("true")), (runtime->getSetting(std:("BROWSER_ENABLED")) == std:("true")));
        if (!browserEnabled) {
            logger->debug(std:("Browser capability disabled in settings."));
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            logger->debug(std:("Stagehand service not available."));
            return false;
        }
        auto url = extractUrl(OR((message->content->text), (string_empty)));
        return url != nullptr;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined, auto _responses = undefined) mutable
    {
        try
        {
            logger->info(std:("Handling BROWSER_NAVIGATE action"));
            shared service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std:("navigate to the requested page"));
                return object{
                    object::pair{std:("text"), std:("Browser service is not available")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_NAVIGATE")}, 
                        object::pair{std:("error"), std:("service_not_available")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("service_not_available")}
                    }}
                };
            }
            shared url = extractUrl(OR((message->content->text), (string_empty)));
            if (!url) {
                auto error = std::make_shared<StagehandError>(std:("No URL found in message"), std:("NO_URL_FOUND"), std:("I couldn't find a URL in your request. Please provide a valid URL to navigate to."), false);
                handleBrowserError(error, callback, std:("navigate to a page"));
                return object{
                    object::pair{std:("text"), std:("I couldn't find a URL in your request. Please provide a valid URL to navigate to.")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_NAVIGATE")}, 
                        object::pair{std:("error"), std:("no_url_found")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("no_url_found")}
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
                        object::pair{std:("text"), std:("Security error: Cannot navigate to restricted URL")}, 
                        object::pair{std:("success"), false}, 
                        object::pair{std:("data"), object{
                            object::pair{std:("actionName"), std:("BROWSER_NAVIGATE")}, 
                            object::pair{std:("error"), std:("security_error")}, 
                            object::pair{std:("url"), std:("url")}
                        }}, 
                        object::pair{std:("values"), object{
                            object::pair{std:("success"), false}, 
                            object::pair{std:("errorType"), std:("security_error")}
                        }}
                    };
                }
                throw any(error);
            }
            shared session = std::async([=]() { service->getCurrentSession(); });
            if (!session) {
                auto sessionId = std:("session-") + Date->now() + string_empty;
                session = std::async([=]() { service->createSession(sessionId); });
            }
            auto result = std::async([=]() { retryWithBackoff([=]() mutable
            {
                auto client = service->getClient();
                return std::async([=]() { client->navigate(session->id, url); });
            }
            , browserRetryConfigs["navigation"], std:("navigate to ") + url + string_empty); });
            auto responseContent = object{
                object::pair{std:("text"), std:("I've navigated to ") + url + std:(". The page title is: "") + result->title + std:(""")}, 
                object::pair{std:("actions"), array<string>{ std:("BROWSER_NAVIGATE") }}, 
                object::pair{std:("source"), message->content->source}
            };
            std::async([=]() { callback(responseContent); });
            std::async([=]() { runtime->emitEvent(EventType->BROWSER_ACTION_PERFORMED, object{
                object::pair{std:("action"), std:("navigation")}, 
                object::pair{std:("url"), result->url}, 
                object::pair{std:("title"), result->title}
            }); });
            return object{
                object::pair{std:("text"), responseContent->text}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_NAVIGATE")}, 
                    object::pair{std:("url"), result->url}, 
                    object::pair{std:("title"), result->title}, 
                    object::pair{std:("sessionId"), session->id}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("url"), result->url}, 
                    object::pair{std:("pageTitle"), result->title}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in BROWSER_NAVIGATE action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserNavigationError>(OR((extractUrl(OR((message->content->text), (string_empty)))), (std:("the requested page"))), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std:("text"), std:("Failed to navigate to the requested page")}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_NAVIGATE")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("unknown_error"))}, 
                    object::pair{std:("url"), OR((extractUrl(OR((message->content->text), (string_empty)))), (std:("unknown")))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("errorType"), std:("navigation_error")}
                }}
            };
        }
    }
    }, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Go to google.com")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've navigated to https://google.com. The page title is: "Google"")}, 
            object::pair{std:("actions"), array<string>{ std:("BROWSER_NAVIGATE") }}
        }}
    } } }}
};
std::shared_ptr<Action> browserClickAction = object{
    object::pair{std:("name"), std:("BROWSER_CLICK")}, 
    object::pair{std:("similes"), array<string>{ std:("CLICK_ELEMENT"), std:("TAP"), std:("PRESS_BUTTON") }}, 
    object::pair{std:("description"), std:("Click on an element on the webpage")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Click on the search button")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've clicked on the search button.")}, 
            object::pair{std:("actions"), array<string>{ std:("BROWSER_CLICK") }}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std:("ENABLE_BROWSER")) == std:("true")), (runtime->getSetting(std:("BROWSER_ENABLED")) == std:("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return OR((OR((text["includes"](std:("click"))), (text["includes"](std:("tap"))))), (text["includes"](std:("press"))));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std:("click on element"));
                return object{
                    object::pair{std:("text"), std:("Browser service is not available")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_CLICK")}, 
                        object::pair{std:("error"), std:("service_not_available")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std:("No active browser session"));
                handleBrowserError(error, callback, std:("click on element"));
                return object{
                    object::pair{std:("text"), std:("No active browser session")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_CLICK")}, 
                        object::pair{std:("error"), std:("no_session")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("no_session")}
                    }}
                };
            }
            auto text = OR((message->content->text), (string_empty));
            auto match = text["match"]((new RegExp(std:("click (?:on |the )?(.+)$"))));
            auto description = (match) ? any(const_(match)[1]) (std:("element"));
            auto result = std::async([=]() { service->getClient()->click(session->id, description); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std:("click"), description, std::make_shared<Error>(OR((result->error), (std:("Click failed"))))));
            }
            auto responseContent = object{
                object::pair{std:("text"), std:("I've successfully clicked on "") + description + std:(""")}, 
                object::pair{std:("actions"), array<string>{ std:("BROWSER_CLICK") }}, 
                object::pair{std:("source"), OR((message->content->source), (std:("action")))}
            };
            std::async([=]() { callback(responseContent); });
            std::async([=]() { runtime->emitEvent(EventType->BROWSER_ACTION_PERFORMED, object{
                object::pair{std:("action"), std:("click")}, 
                object::pair{std:("element"), description}
            }); });
            auto lowerDesc = description["toLowerCase"]();
            if (OR((OR((OR((lowerDesc["includes"](std:("submit"))), (lowerDesc["includes"](std:("send"))))), (lowerDesc["includes"](std:("post"))))), (lowerDesc["includes"](std:("save"))))) {
                std::async([=]() { runtime->emitEvent(EventType->FORM_SUBMITTED, object{
                    object::pair{std:("details"), object{
                        object::pair{std:("element"), description}, 
                        object::pair{std:("action"), std:("click")}
                    }}
                }); });
            }
            return object{
                object::pair{std:("text"), responseContent->text}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_CLICK")}, 
                    object::pair{std:("element"), description}, 
                    object::pair{std:("sessionId"), session->id}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("element"), description}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in BROWSER_CLICK action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std:("click"), std:("element"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std:("text"), std:("Failed to click on the requested element")}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_CLICK")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("unknown_error"))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("errorType"), std:("click_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Action> browserTypeAction = object{
    object::pair{std:("name"), std:("BROWSER_TYPE")}, 
    object::pair{std:("similes"), array<string>{ std:("TYPE_TEXT"), std:("INPUT"), std:("ENTER_TEXT") }}, 
    object::pair{std:("description"), std:("Type text into an input field on the webpage")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Type "hello world" in the search box")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've typed "hello world" in the search box.")}, 
            object::pair{std:("actions"), array<string>{ std:("BROWSER_TYPE") }}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std:("ENABLE_BROWSER")) == std:("true")), (runtime->getSetting(std:("BROWSER_ENABLED")) == std:("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return OR((OR((text["includes"](std:("type"))), (text["includes"](std:("input"))))), (text["includes"](std:("enter"))));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std:("type text"));
                return object{
                    object::pair{std:("text"), std:("Browser service is not available")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_TYPE")}, 
                        object::pair{std:("error"), std:("service_not_available")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std:("No active browser session"));
                handleBrowserError(error, callback, std:("type text"));
                return object{
                    object::pair{std:("text"), std:("No active browser session")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_TYPE")}, 
                        object::pair{std:("error"), std:("no_session")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("no_session")}
                    }}
                };
            }
            auto text = OR((message->content->text), (string_empty));
            auto match = text["match"]((new RegExp(std:("["']([^"']+)["'"))));
            auto textToType = (match) ? any(const_(match)[1]) (string_empty);
            auto fieldMatch = text["match"]((new RegExp(std:("(?:in|into) (?:the )?(.+)$"))));
            auto field = (fieldMatch) ? any(const_(fieldMatch)[1]) (std:("input field"));
            if (!textToType) {
                throw any(std::make_shared<BrowserActionError>(std:("type"), field, std::make_shared<Error>(std:("No text specified to type"))));
            }
            auto result = std::async([=]() { service->getClient()->type(session->id, textToType, field); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std:("type"), field, std::make_shared<Error>(OR((result->error), (std:("Type failed"))))));
            }
            auto responseContent = object{
                object::pair{std:("text"), std:("I've typed "") + textToType + std:("" in the ") + field + string_empty}, 
                object::pair{std:("actions"), array<string>{ std:("BROWSER_TYPE") }}, 
                object::pair{std:("source"), OR((message->content->source), (std:("action")))}
            };
            std::async([=]() { callback(responseContent); });
            std::async([=]() { runtime->emitEvent(EventType->BROWSER_ACTION_PERFORMED, object{
                object::pair{std:("action"), std:("type")}, 
                object::pair{std:("field"), std:("field")}, 
                object::pair{std:("text"), textToType}
            }); });
            if (OR((field["toLowerCase"]()["includes"](std:("form"))), (field["toLowerCase"]()["includes"](std:("submit"))))) {
                std::async([=]() { runtime->emitEvent(EventType->FORM_SUBMITTED, object{
                    object::pair{std:("details"), object{
                        object::pair{std:("field"), std:("field")}, 
                        object::pair{std:("text"), textToType}, 
                        object::pair{std:("action"), std:("type")}
                    }}
                }); });
            }
            return object{
                object::pair{std:("text"), responseContent->text}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_TYPE")}, 
                    object::pair{std:("textTyped"), textToType}, 
                    object::pair{std:("field"), std:("field")}, 
                    object::pair{std:("sessionId"), session->id}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("textTyped"), textToType}, 
                    object::pair{std:("field"), std:("field")}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in BROWSER_TYPE action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std:("type"), std:("input field"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std:("text"), std:("Failed to type text")}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_TYPE")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("unknown_error"))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("errorType"), std:("type_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Action> browserSelectAction = object{
    object::pair{std:("name"), std:("BROWSER_SELECT")}, 
    object::pair{std:("similes"), array<string>{ std:("SELECT_OPTION"), std:("CHOOSE"), std:("PICK") }}, 
    object::pair{std:("description"), std:("Select an option from a dropdown on the webpage")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Select "United States" from the country dropdown")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've selected "United States" from the country dropdown.")}, 
            object::pair{std:("actions"), array<string>{ std:("BROWSER_SELECT") }}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std:("ENABLE_BROWSER")) == std:("true")), (runtime->getSetting(std:("BROWSER_ENABLED")) == std:("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return OR((OR((text["includes"](std:("select"))), (text["includes"](std:("choose"))))), (text["includes"](std:("pick"))));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std:("select option"));
                return object{
                    object::pair{std:("text"), std:("Browser service is not available")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_SELECT")}, 
                        object::pair{std:("error"), std:("service_not_available")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std:("No active browser session"));
                handleBrowserError(error, callback, std:("select option"));
                return object{
                    object::pair{std:("text"), std:("No active browser session")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_SELECT")}, 
                        object::pair{std:("error"), std:("no_session")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("no_session")}
                    }}
                };
            }
            auto text = OR((message->content->text), (string_empty));
            auto match = text["match"]((new RegExp(std:("["']([^"']+)["'"))));
            auto option = (match) ? any(const_(match)[1]) (string_empty);
            auto dropdownMatch = text["match"]((new RegExp(std:("from (?:the )?(.+)$"))));
            auto dropdown = (dropdownMatch) ? any(const_(dropdownMatch)[1]) (std:("dropdown"));
            if (!option) {
                throw any(std::make_shared<BrowserActionError>(std:("select"), dropdown, std::make_shared<Error>(std:("No option specified to select"))));
            }
            auto result = std::async([=]() { service->getClient()->select(session->id, option, dropdown); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std:("select"), dropdown, std::make_shared<Error>(OR((result->error), (std:("Select failed"))))));
            }
            auto responseContent = object{
                object::pair{std:("text"), std:("I've selected "") + option + std:("" from the ") + dropdown + string_empty}, 
                object::pair{std:("actions"), array<string>{ std:("BROWSER_SELECT") }}, 
                object::pair{std:("source"), OR((message->content->source), (std:("action")))}
            };
            std::async([=]() { callback(responseContent); });
            return object{
                object::pair{std:("text"), responseContent->text}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_SELECT")}, 
                    object::pair{std:("option"), std:("option")}, 
                    object::pair{std:("dropdown"), std:("dropdown")}, 
                    object::pair{std:("sessionId"), session->id}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("option"), std:("option")}, 
                    object::pair{std:("dropdown"), std:("dropdown")}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in BROWSER_SELECT action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std:("select"), std:("dropdown"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std:("text"), std:("Failed to select option")}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_SELECT")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("unknown_error"))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("errorType"), std:("select_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Action> browserExtractAction = object{
    object::pair{std:("name"), std:("BROWSER_EXTRACT")}, 
    object::pair{std:("similes"), array<string>{ std:("EXTRACT_DATA"), std:("GET_TEXT"), std:("SCRAPE") }}, 
    object::pair{std:("description"), std:("Extract data from the webpage")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Extract the main heading from the page")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I extracted the main heading: "Welcome to Our Website"")}, 
            object::pair{std:("actions"), array<string>{ std:("BROWSER_EXTRACT") }}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std:("ENABLE_BROWSER")) == std:("true")), (runtime->getSetting(std:("BROWSER_ENABLED")) == std:("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return (OR((OR((OR((OR((text["includes"](std:("extract"))), (text["includes"](std:("get"))))), (text["includes"](std:("scrape"))))), (text["includes"](std:("find"))))), (text["includes"](std:("read")))));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std:("extract data"));
                return object{
                    object::pair{std:("text"), std:("Browser service is not available")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_EXTRACT")}, 
                        object::pair{std:("error"), std:("service_not_available")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std:("No active browser session"));
                handleBrowserError(error, callback, std:("extract data"));
                return object{
                    object::pair{std:("text"), std:("No active browser session")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_EXTRACT")}, 
                        object::pair{std:("error"), std:("no_session")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("no_session")}
                    }}
                };
            }
            auto text = OR((message->content->text), (string_empty));
            auto match = text["match"]((new RegExp(std:("(?:extract|get|find|scrape|read) (?:the )?(.+?)(?:\s+from|\s*$)"))));
            auto instruction = (match) ? const_(match)[1] : text;
            auto result = std::async([=]() { service->getClient()->extract(session->id, instruction); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std:("extract"), std:("page"), std::make_shared<Error>(OR((result->error), (std:("Extraction failed"))))));
            }
            auto extractedData = result->data;
            auto foundText = OR((extractedData->data), (std:("No data found")));
            auto found = OR((extractedData->found), (false));
            auto responseContent = object{
                object::pair{std:("text"), (found) ? std:("I found the ") + instruction + std:(": "") + foundText + std:(""") : std:("I couldn't find the requested ") + instruction + std:(" on the page.")}, 
                object::pair{std:("actions"), array<string>{ std:("BROWSER_EXTRACT") }}, 
                object::pair{std:("source"), OR((message->content->source), (std:("action")))}
            };
            std::async([=]() { callback(responseContent); });
            return object{
                object::pair{std:("text"), responseContent->text}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_EXTRACT")}, 
                    object::pair{std:("instruction"), std:("instruction")}, 
                    object::pair{std:("found"), std:("found")}, 
                    object::pair{std:("data"), foundText}, 
                    object::pair{std:("sessionId"), session->id}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("found"), std:("found")}, 
                    object::pair{std:("data"), foundText}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in BROWSER_EXTRACT action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std:("extract"), std:("page"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std:("text"), std:("Failed to extract data from the page")}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_EXTRACT")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("unknown_error"))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("errorType"), std:("extract_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Action> browserScreenshotAction = object{
    object::pair{std:("name"), std:("BROWSER_SCREENSHOT")}, 
    object::pair{std:("similes"), array<string>{ std:("TAKE_SCREENSHOT"), std:("CAPTURE_PAGE"), std:("SCREENSHOT") }}, 
    object::pair{std:("description"), std:("Take a screenshot of the current page")}, 
    object::pair{std:("examples"), array<array<any>>{ array<object>{ object{
        object::pair{std:("name"), std:("{{user}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("Take a screenshot of the page")}
        }}
    }, object{
        object::pair{std:("name"), std:("{{agent}}")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("I've taken a screenshot of the page.")}, 
            object::pair{std:("actions"), array<string>{ std:("BROWSER_SCREENSHOT") }}
        }}
    } } }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto browserEnabled = OR((runtime->getSetting(std:("ENABLE_BROWSER")) == std:("true")), (runtime->getSetting(std:("BROWSER_ENABLED")) == std:("true")));
        if (!browserEnabled) {
            return false;
        }
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        if (!service) {
            return false;
        }
        auto text = OR((message->content->text->toLowerCase()), (string_empty));
        return OR((OR((text["includes"](std:("screenshot"))), (text["includes"](std:("capture"))))), (text["includes"](std:("snap"))));
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto _state = undefined, auto _options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
            if (!service) {
                auto error = std::make_shared<BrowserServiceNotAvailableError>();
                handleBrowserError(error, callback, std:("take screenshot"));
                return object{
                    object::pair{std:("text"), std:("Browser service is not available")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_SCREENSHOT")}, 
                        object::pair{std:("error"), std:("service_not_available")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("service_not_available")}
                    }}
                };
            }
            auto session = std::async([=]() { service->getOrCreateSession(); });
            if (!session) {
                auto error = std::make_shared<BrowserSessionError>(std:("No active browser session"));
                handleBrowserError(error, callback, std:("take screenshot"));
                return object{
                    object::pair{std:("text"), std:("No active browser session")}, 
                    object::pair{std:("success"), false}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("actionName"), std:("BROWSER_SCREENSHOT")}, 
                        object::pair{std:("error"), std:("no_session")}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("success"), false}, 
                        object::pair{std:("errorType"), std:("no_session")}
                    }}
                };
            }
            auto result = std::async([=]() { service->getClient()->screenshot(session->id); });
            if (!result->success) {
                throw any(std::make_shared<BrowserActionError>(std:("screenshot"), std:("page"), std::make_shared<Error>(OR((result->error), (std:("Screenshot failed"))))));
            }
            auto screenshotData = result->data;
            auto url = OR((screenshotData->url), (std:("unknown")));
            auto title = OR((screenshotData->title), (std:("Untitled")));
            auto responseContent = object{
                object::pair{std:("text"), std:("I've taken a screenshot of the page "") + title + std:("" at ") + url + string_empty}, 
                object::pair{std:("actions"), array<string>{ std:("BROWSER_SCREENSHOT") }}, 
                object::pair{std:("source"), OR((message->content->source), (std:("action")))}, 
                object::pair{std:("data"), object{
                    object::pair{std:("screenshot"), screenshotData->screenshot}, 
                    object::pair{std:("mimeType"), OR((screenshotData->mimeType), (std:("image/png")))}, 
                    object::pair{std:("url"), std:("url")}, 
                    object::pair{std:("title"), std:("title")}
                }}
            };
            std::async([=]() { callback(responseContent); });
            return object{
                object::pair{std:("text"), responseContent->text}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_SCREENSHOT")}, 
                    object::pair{std:("url"), std:("url")}, 
                    object::pair{std:("title"), std:("title")}, 
                    object::pair{std:("sessionId"), session->id}, 
                    object::pair{std:("screenshot"), screenshotData->screenshot}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("url"), std:("url")}, 
                    object::pair{std:("title"), std:("title")}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error in BROWSER_SCREENSHOT action:"), error);
            if (is<StagehandError>(error)) {
                handleBrowserError(error, callback);
            } else {
                auto browserError = std::make_shared<BrowserActionError>(std:("screenshot"), std:("page"), as<std::shared_ptr<Error>>(error));
                handleBrowserError(browserError, callback);
            }
            return object{
                object::pair{std:("text"), std:("Failed to take screenshot")}, 
                object::pair{std:("success"), false}, 
                object::pair{std:("data"), object{
                    object::pair{std:("actionName"), std:("BROWSER_SCREENSHOT")}, 
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("unknown_error"))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("success"), false}, 
                    object::pair{std:("errorType"), std:("screenshot_error")}
                }}
            };
        }
    }
    }
};
std::shared_ptr<Provider> browserStateProvider = object{
    object::pair{std:("name"), std:("BROWSER_STATE")}, 
    object::pair{std:("description"), std:("Provides current browser state information including active session status, current page URL, and page title")}, 
    object::pair{std:("get"), [=](auto runtime, auto _message, auto _state = undefined) mutable
    {
        auto service = runtime->getService<std::shared_ptr<StagehandService>>(StagehandService::serviceType);
        auto session = std::async([=]() { service->getCurrentSession(); });
        if (OR((!session), (!service))) {
            return object{
                object::pair{std:("text"), std:("No active browser session")}, 
                object::pair{std:("values"), object{
                    object::pair{std:("hasSession"), false}
                }}, 
                object::pair{std:("data"), object{}}
            };
        }
        try
        {
            auto client = service->getClient();
            auto state = std::async([=]() { client->getState(session->id); });
            return object{
                object::pair{std:("text"), std:("Current browser page: "") + state->title + std:("" at ") + state->url + string_empty}, 
                object::pair{std:("values"), object{
                    object::pair{std:("hasSession"), true}, 
                    object::pair{std:("url"), state->url}, 
                    object::pair{std:("title"), state->title}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("sessionId"), session->id}, 
                    object::pair{std:("createdAt"), session->createdAt}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("Error getting browser state:"), error);
            return object{
                object::pair{std:("text"), std:("Error getting browser state")}, 
                object::pair{std:("values"), object{
                    object::pair{std:("hasSession"), true}, 
                    object::pair{std:("error"), true}
                }}, 
                object::pair{std:("data"), object{}}
            };
        }
    }
    }
};
std::shared_ptr<Plugin> stagehandPlugin = object{
    object::pair{std:("name"), std:("plugin-stagehand")}, 
    object::pair{std:("description"), std:("Browser automation plugin using Stagehand - stagehand is goated for web interactions")}, 
    object::pair{std:("config"), object{
        object::pair{std:("BROWSERBASE_API_KEY"), process->env->BROWSERBASE_API_KEY}, 
        object::pair{std:("BROWSERBASE_PROJECT_ID"), process->env->BROWSERBASE_PROJECT_ID}, 
        object::pair{std:("OPENAI_API_KEY"), process->env->OPENAI_API_KEY}, 
        object::pair{std:("ANTHROPIC_API_KEY"), process->env->ANTHROPIC_API_KEY}, 
        object::pair{std:("OLLAMA_BASE_URL"), process->env->OLLAMA_BASE_URL}, 
        object::pair{std:("OLLAMA_MODEL"), process->env->OLLAMA_MODEL}, 
        object::pair{std:("BROWSER_HEADLESS"), process->env->BROWSER_HEADLESS}, 
        object::pair{std:("CAPSOLVER_API_KEY"), process->env->CAPSOLVER_API_KEY}, 
        object::pair{std:("STAGEHAND_SERVER_PORT"), process->env->STAGEHAND_SERVER_PORT}
    }}, 
    , 
    object::pair{std:("services"), array<StagehandService>{ StagehandService }}, 
    object::pair{std:("actions"), array<any>{ browserNavigateAction, browserClickAction, browserTypeAction, browserSelectAction, browserExtractAction, browserScreenshotAction }}, 
    object::pair{std:("providers"), array<any>{ browserStateProvider }}
};

void Main(void)
{
}

MAIN
