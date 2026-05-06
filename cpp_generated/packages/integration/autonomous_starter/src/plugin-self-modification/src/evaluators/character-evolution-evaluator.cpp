#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/evaluators/character-evolution-evaluator.h"

string evolutionAnalysisTemplate = std::string("Analyze the recent conversation and determine if the agent's character should evolve based on what was learned.\
\
Current character state:\
{{characterState}}\
\
Recent conversation:\
{{recentMessages}}\
\
Consider:\
1. Are there new topics or interests that emerged?\
2. Did the conversation reveal areas where the character could improve?\
3. Were there communication patterns that worked particularly well?\
4. Did the user express preferences about interaction style?\
\
Respond with whether character evolution is recommended and why.");
std::shared_ptr<Evaluator> characterEvolutionEvaluator = object{
    object::pair{std::string("name"), std::string("characterEvolution")}, 
    object::pair{std::string("description"), std::string("Analyzes conversations to determine if character should evolve")}, 
    object::pair{std::string("validate"), [=](auto runtime) mutable
    {
        auto modService = runtime->getService(CharacterModificationService::serviceName);
        return modService != nullptr;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state) mutable
    {
        auto modService = as<std::shared_ptr<CharacterModificationService>>(runtime->getService(CharacterModificationService::serviceName));
        if (!modService) {
            logger->warn(std::string("Character modification service not available"));
            return std::shared_ptr<Promise<void>>();
        }
        auto recentMessages = std::async([=]() { runtime->getMemories(object{
            object::pair{std::string("roomId"), message->roomId}, 
            object::pair{std::string("count"), 10}, 
            object::pair{std::string("tableName"), std::string("messages")}
        }); });
        if (OR((!recentMessages), (recentMessages->length < 5))) {
            logger->debug(std::string("Not enough messages for evolution analysis"));
            return std::shared_ptr<Promise<void>>();
        }
        try
        {
            auto prompt = composePromptFromState(object{
                object::pair{std::string("state"), object{
                    object::pair{std::string("values"), object{
                        object::pair{std::string("characterState"), JSON->stringify(runtime->character)}, 
                        object::pair{std::string("recentMessages"), recentMessages->map([=](auto m) mutable
                        {
                            return string_empty + m["entityId"] + std::string(": ") + m["content"]->text + string_empty;
                        }
                        )->join(std::string("\
"))}
                    }}, 
                    object::pair{std::string("data"), object{}}, 
                    object::pair{std::string("text"), string_empty}
                }}, 
                object::pair{std::string("template"), evolutionAnalysisTemplate}
            });
            auto analysis = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
                object::pair{std::string("prompt"), std::string("prompt")}
            }); });
            auto shouldEvolve = AND((analysis), ((OR((OR((analysis->toLowerCase()->includes(std::string("recommend"))), (analysis->toLowerCase()->includes(std::string("should evolve"))))), (analysis->toLowerCase()->includes(std::string("would benefit")))))));
            if (shouldEvolve) {
                logger->info(std::string("Character evolution recommended based on conversation"));
                std::async([=]() { runtime->createTask(object{
                    object::pair{std::string("name"), std::string("modifyCharacter")}, 
                    object::pair{std::string("description"), std::string("Modify character based on recent conversation insights")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("character-evolution"), std::string("auto-triggered") }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("focusAreas"), std::string("recent conversation insights")}, 
                        object::pair{std::string("autoTrigger"), true}, 
                        object::pair{std::string("conversationId"), message->roomId}
                    }}, 
                    object::pair{std::string("roomId"), message->roomId}
                }); });
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error in character evolution evaluator:"), error);
        }
    }
    }, 
    object::pair{std::string("examples"), array<any>()}
};

void Main(void)
{
}

MAIN
