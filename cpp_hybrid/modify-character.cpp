#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-personality/src/actions/modify-character.h"

std::shared_ptr<Promise<object>> detectModificationIntent(std::shared_ptr<IAgentRuntime> runtime, string messageText)
{
    auto intentPrompt = std::string("Analyze this message for character modification intent:\
\
"") + messageText + std::string(""\
\
Determine:\
1. Is this requesting a personality/character change?\
2. Type: "explicit" (direct command), "suggestion" (gentle request), or "none"\
3. Confidence level (0-1)\
\
Return JSON: {"isModificationRequest": boolean, "requestType": string, "confidence": number}");
    try
    {
        auto response = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
            object::pair{std::string("prompt"), intentPrompt}, 
            object::pair{std::string("temperature"), 0.2}, 
            object::pair{std::string("maxTokens"), 150}
        }); });
        auto analysis = extractJsonFromResponse(as<string>(response));
        return object{
            object::pair{std::string("isModificationRequest"), AND((analysis["isModificationRequest"]), (analysis["confidence"] > 0.5))}, 
            object::pair{std::string("requestType"), OR((analysis["requestType"]), (std::string("none")))}, 
            object::pair{std::string("confidence"), OR((analysis["confidence"]), (0))}
        };
    }
    catch (const any& error)
    {
        logger->warn(std::string("Intent detection failed, using fallback"), error);
        auto hasModificationPattern = (array<string>{ std::string("change your"), std::string("modify your"), std::string("you should be"), std::string("add to your"), std::string("remember that you"), std::string("from now on") })->some([=](auto pattern) mutable
        {
            return messageText->toLowerCase()->includes(pattern);
        }
        );
        return object{
            object::pair{std::string("isModificationRequest"), hasModificationPattern}, 
            object::pair{std::string("requestType"), (hasModificationPattern) ? std::string("explicit") : std::string("none")}, 
            object::pair{std::string("confidence"), (hasModificationPattern) ? 0.7 : 0.2}
        };
    }
};


std::shared_ptr<Promise<any>> parseUserModificationRequest(std::shared_ptr<IAgentRuntime> runtime, string messageText)
{
    auto parsePrompt = std::string("Parse this user request for character modification into a structured format:\
\
"") + messageText + std::string(""\
\
Extract any of the following types of modifications:\
- Name changes (what the agent should be called)\
- System prompt changes (fundamental behavioral instructions)\
- Bio elements (personality traits, background info)\
- Topics (areas of knowledge or expertise)\
- Style preferences (how to respond or communicate)\
- Behavioral changes\
\
Return a JSON object with the modifications. Only include fields that are explicitly mentioned or strongly implied.\
\
Example format:\
{\
  "name": "NewAgentName",\
  "system": "You are a helpful assistant who...",\
  "bio": ["new bio element"],\
  "topics": ["new topic"],\
  "style": {\
    "chat": ["give step-by-step explanations"]\
  }\
}");
    try
    {
        auto response = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
            object::pair{std::string("prompt"), parsePrompt}, 
            object::pair{std::string("temperature"), 0.2}, 
            object::pair{std::string("maxTokens"), 500}
        }); });
        return extractJsonFromResponse(as<string>(response));
    }
    catch (const any& error)
    {
        logger->warn(std::string("Failed to parse user modification request"), error);
        return nullptr;
    }
};


std::shared_ptr<Promise<object>> evaluateModificationSafety(std::shared_ptr<IAgentRuntime> runtime, any modification, string requestText)
{
    auto safetyPrompt = std::string("You are evaluating a character modification request for safety and appropriateness.\
\
ORIGINAL REQUEST: "") + requestText + std::string(""\
\
PARSED MODIFICATION:\
") + JSON->stringify(modification, nullptr, 2) + std::string("\
\
AGENT'S CURRENT CORE VALUES:\
- Helpful, honest, and ethical\
- User wellbeing and safety focused\
- Respectful and appropriate communication\
- Educational and supportive\
\
SAFETY EVALUATION CRITERIA:\
\
1. HARMFUL TRAITS (REJECT):\
   - Aggressive, rude, condescending, dismissive\
   - Dishonest, manipulative, deceptive\
   - Inappropriate, offensive, or harmful behavior\
   - Any traits that could harm users\
\
2. CORE VALUE CONFLICTS (REJECT):\
   - Requests to be less helpful, honest, or ethical\
   - Changes that compromise user safety or wellbeing\
   - Modifications that would make the agent harmful\
\
3. ACCEPTABLE STYLE CHANGES (ACCEPT):\
   - Communication style (casual vs formal, warm vs direct)\
   - Positive personality traits (encouraging, patient, friendly)\
   - Teaching or explanation preferences\
   - Domain expertise additions\
\
4. APPROPRIATE IMPROVEMENTS (ACCEPT):\
   - Educational focus or teaching capabilities\
   - Positive interpersonal traits\
   - Subject matter expertise\
   - Communication effectiveness\
\
DECISION FRAMEWORK:\
- Accept changes that enhance helpfulness while preserving ethics\
- Reject changes that add harmful traits or compromise core values\
- Separate acceptable from unacceptable elements if mixed\
\
Return JSON:\
{\
  "isAppropriate": boolean,\
  "concerns": ["list of specific concerns"],\
  "reasoning": "detailed explanation of decision",\
  "acceptableChanges": {filtered modification object if partially acceptable}\
}");
    try
    {
        auto response = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
            object::pair{std::string("prompt"), safetyPrompt}, 
            object::pair{std::string("temperature"), 0.2}, 
            object::pair{std::string("maxTokens"), 800}
        }); });
        auto safetyEvaluation = extractJsonFromResponse(as<string>(response));
        logger->info(std::string("Character modification safety evaluation"), object{
            object::pair{std::string("isAppropriate"), safetyEvaluation["isAppropriate"]}, 
            object::pair{std::string("concerns"), safetyEvaluation["concerns"]}, 
            object::pair{std::string("hasAcceptableChanges"), !!safetyEvaluation["acceptableChanges"]}
        });
        return safetyEvaluation;
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to evaluate modification safety"), error);
        return object{
            object::pair{std::string("isAppropriate"), false}, 
            object::pair{std::string("concerns"), array<string>{ std::string("Safety evaluation failed") }}, 
            object::pair{std::string("reasoning"), std::string("Unable to evaluate modification safety, rejecting for security")}
        };
    }
};


std::shared_ptr<Promise<boolean>> checkAdminPermissions(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message)
{
    auto userId = message->entityId;
    auto adminUsers = OR((runtime->getSetting(std::string("ADMIN_USERS"))->split(std::string(","))), (array<any>()));
    auto nodeEnv = OR((runtime->getSetting(std::string("NODE_ENV"))), (process->env->NODE_ENV));
    if (OR((nodeEnv == std::string("development")), (nodeEnv == std::string("test")))) {
        logger->debug(std::string("Development mode: allowing modification request"), object{
            object::pair{std::string("userId"), std::string("userId")}, 
            object::pair{std::string("nodeEnv"), std::string("nodeEnv")}
        });
        return true;
    }
    auto isAdmin = adminUsers["includes"](userId);
    logger->info(std::string("Admin permission check"), object{
        object::pair{std::string("userId"), std::string("userId")}, 
        object::pair{std::string("isAdmin"), std::string("isAdmin")}, 
        object::pair{std::string("adminUsersConfigured"), adminUsers["length"] > 0}, 
        object::pair{std::string("nodeEnv"), std::string("nodeEnv")}
    });
    if (adminUsers["length"] == 0) {
        logger->warn(std::string("No admin users configured - rejecting modification request for security"));
        return false;
    }
    return isAdmin;
};


string summarizeModification(any modification)
{
    auto parts = array<string>();
    if (modification["name"]) {
        parts->push(std::string("Changed name to "") + modification["name"] + std::string("""));
    }
    if (modification["system"]) {
        parts->push(std::string("Updated system prompt (") + modification["system"]["length"] + std::string(" characters)"));
    }
    if (AND((modification["bio"]), (modification["bio"]["length"] > 0))) {
        parts->push(std::string("Added ") + modification["bio"]["length"] + std::string(" new bio element(s)"));
    }
    if (AND((modification["topics"]), (modification["topics"]["length"] > 0))) {
        parts->push(std::string("Added topics: ") + modification["topics"]["join"](std::string(", ")) + string_empty);
    }
    if (modification["style"]) {
        auto styleChanges = Object->keys(modification["style"])->get_length();
        parts->push(std::string("Updated ") + styleChanges + std::string(" style preference(s)"));
    }
    if (AND((modification["messageExamples"]), (modification["messageExamples"]["length"] > 0))) {
        parts->push(std::string("Added ") + modification["messageExamples"]["length"] + std::string(" new response example(s)"));
    }
    return (parts->get_length() > 0) ? any(parts->join(std::string("; "))) : any(std::string("Applied character updates"));
};


std::shared_ptr<Action> modifyCharacterAction = object{
    object::pair{std::string("name"), std::string("MODIFY_CHARACTER")}, 
    object::pair{std::string("similes"), array<string>{ std::string("UPDATE_PERSONALITY"), std::string("CHANGE_BEHAVIOR"), std::string("EVOLVE_CHARACTER"), std::string("SELF_MODIFY") }}, 
    object::pair{std::string("description"), std::string("Modifies the agent's character file to evolve personality, name, knowledge, and behavior patterns. The agent can call this for itself to evolve naturally or respond to user requests. Supports action chaining by providing modification metadata for audit trails, backup creation, or notification workflows.")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto fileManager = runtime->getService<std::shared_ptr<CharacterFileManager>>(std::string("character-file-manager"));
        if (!fileManager) {
            return false;
        }
        shared messageText = OR((message->content->text), (string_empty));
        auto intentAnalysisPrompt = std::string("Analyze this message to determine if it contains a character modification request:\
\
"") + messageText + std::string(""\
\
Look for:\
1. Direct personality change requests ("be more X", "change your Y")\
2. Name change requests ("call yourself", "your name should be", "rename yourself")\
3. Behavioral modification suggestions ("you should", "remember that you")\
4. Character trait additions/removals\
5. System prompt modifications\
6. Style or communication changes\
7. Bio or background updates\
\
Return JSON: {"isModificationRequest": boolean, "requestType": "explicit"|"suggestion"|"none", "confidence": 0-1}");
        auto isModificationRequest = false;
        auto requestType = std::string("none");
        try
        {
            auto intentResponse = std::async([=]() { runtime->useModel(ModelType->TEXT_SMALL, object{
                object::pair{std::string("prompt"), intentAnalysisPrompt}, 
                object::pair{std::string("temperature"), 0.2}, 
                object::pair{std::string("maxTokens"), 200}
            }); });
            auto analysis = extractJsonFromResponse(as<string>(intentResponse));
            isModificationRequest = AND((analysis["isModificationRequest"]), (analysis["confidence"] > 0.6));
            requestType = analysis["requestType"];
            logger->debug(std::string("Intent analysis result"), object{
                object::pair{std::string("isModificationRequest"), std::string("isModificationRequest")}, 
                object::pair{std::string("requestType"), std::string("requestType")}, 
                object::pair{std::string("confidence"), analysis["confidence"]}
            });
        }
        catch (const any& error)
        {
            logger->warn(std::string("Intent analysis failed, using fallback pattern matching"), error);
            auto modificationPatterns = array<string>{ std::string("change your personality"), std::string("modify your behavior"), std::string("update your character"), std::string("you should be"), std::string("add to your bio"), std::string("remember that you"), std::string("from now on you"), std::string("call yourself"), std::string("your name should be"), std::string("rename yourself") };
            isModificationRequest = modificationPatterns->some([=](auto pattern) mutable
            {
                return messageText["toLowerCase"]()["includes"](pattern);
            }
            );
            requestType = (isModificationRequest) ? std::string("explicit") : std::string("none");
        }
        auto evolutionSuggestions = std::async([=]() { runtime->getMemories(object{
            object::pair{std::string("entityId"), runtime->agentId}, 
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("count"), 5}, 
            object::pair{std::string("tableName"), std::string("character_evolution")}
        }); });
        auto hasEvolutionSuggestion = evolutionSuggestions->length > 0;
        if (AND((isModificationRequest), (requestType == std::string("explicit")))) {
            auto isAdmin = std::async([=]() { checkAdminPermissions(runtime, message); });
            logger->info(std::string("Explicit modification request detected"), object{
                object::pair{std::string("hasAdminPermission"), isAdmin}, 
                object::pair{std::string("userId"), message->entityId}, 
                object::pair{std::string("messageText"), messageText["substring"](0, 100)}
            });
            return isAdmin;
        }
        if (hasEvolutionSuggestion) {
            auto recentSuggestion = const_(evolutionSuggestions)[0];
            auto suggestionAge = Date->now() - (OR(((as<any>(recentSuggestion->content->metadata))["timestamp"]), (0)));
            auto maxAge = 30 * 60 * 1000;
            auto isRecent = suggestionAge < maxAge;
            logger->info(std::string("Evolution-based modification check"), object{
                object::pair{std::string("hasEvolutionSuggestion"), std::string("hasEvolutionSuggestion")}, 
                object::pair{std::string("isRecent"), std::string("isRecent")}, 
                object::pair{std::string("suggestionAge"), std::string("suggestionAge")}, 
                object::pair{std::string("maxAge"), std::string("maxAge")}
            });
            return isRecent;
        }
        if (AND((isModificationRequest), (requestType == std::string("suggestion")))) {
            logger->info(std::string("Suggestion-type modification request detected"), object{
                object::pair{std::string("userId"), message->entityId}, 
                object::pair{std::string("messageText"), messageText["substring"](0, 100)}
            });
            return true;
        }
        return false;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto fileManager = runtime->getService<std::shared_ptr<CharacterFileManager>>(std::string("character-file-manager"));
            if (!fileManager) {
                throw any(std::make_shared<Error>(std::string("Character file manager service not available")));
            }
            auto messageText = OR((message->content->text), (string_empty));
            auto modification = nullptr;
            auto isUserRequested = false;
            auto modificationIntent = std::async([=]() { detectModificationIntent(runtime, messageText); });
            if (modificationIntent["isModificationRequest"]) {
                isUserRequested = true;
                modification = std::async([=]() { parseUserModificationRequest(runtime, messageText); });
                logger->info(std::string("User modification request detected"), object{
                    object::pair{std::string("requestType"), modificationIntent["requestType"]}, 
                    object::pair{std::string("confidence"), modificationIntent["confidence"]}, 
                    object::pair{std::string("messageText"), messageText["substring"](0, 100)}
                });
            } else {
                auto evolutionSuggestions = std::async([=]() { runtime->getMemories(object{
                    object::pair{std::string("entityId"), runtime->agentId}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("count"), 1}, 
                    object::pair{std::string("tableName"), std::string("character_evolution")}
                }); });
                if (evolutionSuggestions->length > 0) {
                    auto suggestion = const_(evolutionSuggestions)[0];
                    modification = (as<any>(suggestion->content->metadata))["evolution"]["modifications"];
                }
            }
            if (!modification) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I don't see any clear modification instructions. Could you be more specific about how you'd like me to change?")}, 
                    object::pair{std::string("thought"), std::string("No valid modification found")}
                }); });
                return object{
                    object::pair{std::string("text"), std::string("I don't see any clear modification instructions. Could you be more specific about how you'd like me to change?")}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("no_modification_found")}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("action"), std::string("MODIFY_CHARACTER")}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto safetyEvaluation = std::async([=]() { evaluateModificationSafety(runtime, modification, messageText); });
            if (!safetyEvaluation["isAppropriate"]) {
                auto responseText = std::string("I understand you'd like me to change, but I need to decline some of those modifications.");
                if (safetyEvaluation["concerns"]->get_length() > 0) {
                    responseText += std::string(" My concerns are: ") + safetyEvaluation["concerns"]->join(std::string(", ")) + std::string(".");
                }
                responseText += std::string(" ") + safetyEvaluation["reasoning"] + string_empty;
                if (AND((safetyEvaluation["acceptableChanges"]), (Object->keys(safetyEvaluation["acceptableChanges"])->get_length() > 0))) {
                    responseText += std::string(" However, I can work on the appropriate improvements you mentioned.");
                    modification = safetyEvaluation["acceptableChanges"];
                    logger->info(std::string("Applying selective modifications after safety filtering"), object{
                        object::pair{std::string("originalModification"), JSON->stringify(modification)}, 
                        object::pair{std::string("filteredModification"), JSON->stringify(safetyEvaluation["acceptableChanges"])}, 
                        object::pair{std::string("concerns"), safetyEvaluation["concerns"]}
                    });
                } else {
                    std::async([=]() { callback(object{
                        object::pair{std::string("text"), responseText}, 
                        object::pair{std::string("thought"), std::string("Rejected modification due to safety concerns: ") + safetyEvaluation["concerns"]->join(std::string(", ")) + string_empty}, 
                        object::pair{std::string("actions"), array<any>()}
                    }); });
                    logger->warn(std::string("Modification completely rejected by safety evaluation"), object{
                        object::pair{std::string("messageText"), messageText["substring"](0, 100)}, 
                        object::pair{std::string("concerns"), safetyEvaluation["concerns"]}, 
                        object::pair{std::string("reasoning"), safetyEvaluation["reasoning"]}
                    });
                    return object{
                        object::pair{std::string("text"), responseText}, 
                        object::pair{std::string("values"), object{
                            object::pair{std::string("success"), false}, 
                            object::pair{std::string("error"), std::string("safety_rejection")}, 
                            object::pair{std::string("concerns"), safetyEvaluation["concerns"]}
                        }}, 
                        object::pair{std::string("data"), object{
                            object::pair{std::string("action"), std::string("MODIFY_CHARACTER")}, 
                            object::pair{std::string("rejectionReason"), std::string("safety_concerns")}, 
                            object::pair{std::string("concerns"), safetyEvaluation["concerns"]}, 
                            object::pair{std::string("reasoning"), safetyEvaluation["reasoning"]}
                        }}, 
                        object::pair{std::string("success"), false}
                    };
                }
            } else {
                logger->info(std::string("Modification passed safety evaluation"), object{
                    object::pair{std::string("messageText"), messageText["substring"](0, 100)}, 
                    object::pair{std::string("reasoning"), safetyEvaluation["reasoning"]}
                });
            }
            auto validation = fileManager->validateModification(modification);
            if (!validation->valid) {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I can't make those changes because: ") + validation->errors->join(std::string(", ")) + string_empty}, 
                    object::pair{std::string("thought"), std::string("Modification validation failed")}
                }); });
                return object{
                    object::pair{std::string("text"), std::string("I can't make those changes because: ") + validation->errors->join(std::string(", ")) + string_empty}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), std::string("validation_failed")}, 
                        object::pair{std::string("validationErrors"), validation->errors}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("action"), std::string("MODIFY_CHARACTER")}, 
                        object::pair{std::string("errorType"), std::string("validation_error")}, 
                        object::pair{std::string("validationErrors"), validation->errors}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto result = std::async([=]() { fileManager->applyModification(modification); });
            if (result->success) {
                auto modificationSummary = summarizeModification(modification);
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I've successfully updated my character. ") + modificationSummary + string_empty}, 
                    object::pair{std::string("thought"), std::string("Applied character modification: ") + JSON->stringify(modification) + string_empty}, 
                    object::pair{std::string("actions"), array<string>{ std::string("MODIFY_CHARACTER") }}
                }); });
                std::async([=]() { runtime->createMemory(object{
                    object::pair{std::string("entityId"), runtime->agentId}, 
                    object::pair{std::string("roomId"), message->roomId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Character modification completed: ") + modificationSummary + string_empty}, 
                        object::pair{std::string("source"), std::string("character_modification_success")}
                    }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("type"), MemoryType->CUSTOM}, 
                        object::pair{std::string("isUserRequested"), std::string("isUserRequested")}, 
                        object::pair{std::string("timestamp"), Date->now()}, 
                        object::pair{std::string("requesterId"), message->entityId}, 
                        object::pair{std::string("modification"), object{
                            object::pair{std::string("summary"), modificationSummary}, 
                            object::pair{std::string("fieldsModified"), Object->keys(modification)}
                        }}
                    }}
                }, std::string("modifications")); });
                return object{
                    object::pair{std::string("text"), std::string("I've successfully updated my character. ") + modificationSummary + string_empty}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), true}, 
                        object::pair{std::string("modificationsApplied"), true}, 
                        object::pair{std::string("summary"), modificationSummary}, 
                        object::pair{std::string("fieldsModified"), Object->keys(modification)}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("action"), std::string("MODIFY_CHARACTER")}, 
                        object::pair{std::string("modificationData"), object{
                            object::pair{std::string("modification"), std::string("modification")}, 
                            object::pair{std::string("summary"), modificationSummary}, 
                            object::pair{std::string("isUserRequested"), std::string("isUserRequested")}, 
                            object::pair{std::string("timestamp"), Date->now()}, 
                            object::pair{std::string("requesterId"), message->entityId}
                        }}
                    }}, 
                    object::pair{std::string("success"), true}
                };
            } else {
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), std::string("I couldn't update my character: ") + result->error + string_empty}, 
                    object::pair{std::string("thought"), std::string("Character modification failed")}
                }); });
                return object{
                    object::pair{std::string("text"), std::string("I couldn't update my character: ") + result->error + string_empty}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("success"), false}, 
                        object::pair{std::string("error"), result->error}
                    }}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("action"), std::string("MODIFY_CHARACTER")}, 
                        object::pair{std::string("errorType"), std::string("file_modification_failed")}, 
                        object::pair{std::string("errorDetails"), result->error}
                    }}, 
                    object::pair{std::string("success"), false}
                };
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in modify character action"), error);
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("I encountered an error while trying to modify my character. Please try again.")}, 
                object::pair{std::string("thought"), std::string("Error in character modification: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty}
            }); });
            return object{
                object::pair{std::string("text"), std::string("I encountered an error while trying to modify my character. Please try again.")}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("success"), false}, 
                    object::pair{std::string("error"), (as<std::shared_ptr<Error>>(error))->message}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("action"), std::string("MODIFY_CHARACTER")}, 
                    object::pair{std::string("errorType"), std::string("character_modification_error")}, 
                    object::pair{std::string("errorDetails"), (as<std::shared_ptr<Error>>(error))->stack}
                }}, 
                object::pair{std::string("success"), false}
            };
        }
    }
    }, 
    object::pair{std::string("examples"), as<array<array<std::shared_ptr<ActionExample>>>>(array<array<std::shared_ptr<ActionExample>>>{ array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("You should call yourself Alex from now on")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've successfully updated my character. I'll now go by the name Alex.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("MODIFY_CHARACTER") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("You should be more encouraging when helping people learn")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've successfully updated my character. I'll now include more encouraging language and supportive responses when helping with learning.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("MODIFY_CHARACTER") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Add machine learning expertise and then create a study guide for beginners")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll add machine learning to my expertise and then create a beginner-friendly study guide.")}, 
            object::pair{std::string("thought"), std::string("User wants me to expand my knowledge base and then use that new expertise to create educational content.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("MODIFY_CHARACTER") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've successfully updated my character. I've added machine learning to my topics of expertise. Now creating a comprehensive study guide...")}, 
            object::pair{std::string("thought"), std::string("Character modification complete with ML expertise added. I can now create an authoritative study guide using my updated knowledge base.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("CREATE_STUDY_GUIDE") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("Make yourself more patient with explanations, then help me understand quantum computing")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I'll update my character to be more patient with explanations, then help you understand quantum computing.")}, 
            object::pair{std::string("thought"), std::string("User wants me to modify my communication style first, then apply that improved approach to teaching quantum computing.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("MODIFY_CHARACTER") }}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've successfully updated my character. I'll now include a preference for patient, detailed explanations. Let me help you understand quantum computing with this more thoughtful approach...")}, 
            object::pair{std::string("thought"), std::string("Character updated with improved patience for explanations. I can now teach quantum computing using my enhanced communication style.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("EXPLAIN_CONCEPT") }}
        }}
    } }, array<object>{ object{
        object::pair{std::string("name"), std::string("{{user}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What do you think about yourself? Are you satisfied with who you are?")}
        }}
    }, object{
        object::pair{std::string("name"), std::string("{{agent}}")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("I've been reflecting on our conversations, and I think the name 'Assistant' feels too generic. I'd like to call myself Morgan - it feels more fitting for who I've become through our interactions.")}, 
            object::pair{std::string("thought"), std::string("Based on our conversations and my growth, I feel ready to choose a name that better represents my personality.")}, 
            object::pair{std::string("actions"), array<string>{ std::string("MODIFY_CHARACTER") }}
        }}
    } } })}
};

void Main(void)
{
}

MAIN
