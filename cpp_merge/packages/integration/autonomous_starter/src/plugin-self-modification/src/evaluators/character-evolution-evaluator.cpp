#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/evaluators/character-evolution-evaluator.h"

string evolutionAnalysisTemplate = std:("Analyze the recent conversation and determine if the agent's character should evolve based on what was learned.\
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
    object::pair{std:("name"), std:("characterEvolution")}, 
    object::pair{std:("description"), std:("Analyzes conversations to determine if character should evolve")}, 
    object::pair{std:("validate"), [=](auto runtime) mutable
    {
        auto modService = runtime->getService(CharacterModificationService::serviceName);
        return modService != nullptr;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state) mutable
    {
        auto modService = as<std::shared_ptr<CharacterModificationService>>(runtime->getService(CharacterModificationService::serviceName));
        if (!modService) {
            logger->warn(std:("Character modification service not available"));
            return std::shared_ptr<Promise<void>>();
        }
        auto recentMessages = std::async([=]() { runtime->getMemories(object{
            object::pair{std:("roomId"), message->roomId}, 
            object::pair{std:("count"), 10}, 
            object::pair{std:("tableName"), std:("messages")}
        }); });
        if (OR((!recentMessages), (recentMessages->length < 5))) {
            logger->debug(std:("Not enough messages for evolution analysis"));
            return std::shared_ptr<Promise<void>>();
        }
        try
        {
            auto prompt = composePromptFromState(object{
                object::pair{std:("state"), object{
                    object::pair{std:("values"), object{
                        object::pair{std:("characterState"), JSON->stringify(runtime->character)}, 
                        object::pair{std:("recentMessages"), recentMessages->map([=](auto m) mutable
                        {
                            return string_empty + m["entityId"] + std:(": ") + m["content"]->text + string_empty;
                        }
                        )->join(std:("\
"))}
                    }}, 
                    object::pair{std:("data"), object{}}, 
                    object::pair{std:("text"), string_empty}
                }}, 
                object::pair{std:("template"), evolutionAnalysisTemplate}
            });
            auto analysis = std::async([=]() { runtime->useModel(ModelType->TEXT_LARGE, object{
                object::pair{std:("prompt"), std:("prompt")}
            }); });
            auto shouldEvolve = AND((analysis), ((OR((OR((analysis->toLowerCase()->includes(std:("recommend"))), (analysis->toLowerCase()->includes(std:("should evolve"))))), (analysis->toLowerCase()->includes(std:("would benefit")))))));
            if (shouldEvolve) {
                logger->info(std:("Character evolution recommended based on conversation"));
                std::async([=]() { runtime->createTask(object{
                    object::pair{std:("name"), std:("modifyCharacter")}, 
                    object::pair{std:("description"), std:("Modify character based on recent conversation insights")}, 
                    object::pair{std:("tags"), array<string>{ std:("character-evolution"), std:("auto-triggered") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("focusAreas"), std:("recent conversation insights")}, 
                        object::pair{std:("autoTrigger"), true}, 
                        object::pair{std:("conversationId"), message->roomId}
                    }}, 
                    object::pair{std:("roomId"), message->roomId}
                }); });
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Error in character evolution evaluator:"), error);
        }
    }
    }, 
    object::pair{std:("examples"), array<any>()}
};

void Main(void)
{
}

MAIN
