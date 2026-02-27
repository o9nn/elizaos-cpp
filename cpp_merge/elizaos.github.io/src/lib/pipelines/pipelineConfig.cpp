#include "pipelineConfig.hpp"

any ScoringConfigSchema = z->object(object{
    object::pair{std:("pullRequest"), z->object(object{
        object::pair{std:("base"), z->number()->default(7)}, 
        object::pair{std:("merged"), z->number()->default(3)}, 
        object::pair{std:("perReview"), z->number()->default(3)}, 
        object::pair{std:("perApproval"), z->number()->default(2)}, 
        object::pair{std:("perComment"), z->number()->default(0.5)}, 
        object::pair{std:("descriptionMultiplier"), z->number()->default(0.006)}, 
        object::pair{std:("complexityMultiplier"), z->number()->default(0.5)}, 
        object::pair{std:("optimalSizeBonus"), z->number()->default(5)}, 
        object::pair{std:("maxPerDay"), z->number()->default(20)}, 
        object::pair{std:("closingIssueBonus"), z->number()->default(4)}
    })}, 
    object::pair{std:("issue"), z->object(object{
        object::pair{std:("base"), z->number()->default(5)}, 
        object::pair{std:("perComment"), z->number()->default(0.5)}, 
        object::pair{std:("withLabelsMultiplier"), z->record(z->string(), z->number())->default(object{
            object::pair{std:("bug"), 1.5}, 
            object::pair{std:("enhancement"), 1.2}, 
            object::pair{std:("documentation"), 1}
        })}, 
        object::pair{std:("closedBonus"), z->number()->default(3)}, 
        object::pair{std:("resolutionSpeedMultiplier"), z->number()->default(0.8)}
    })}, 
    object::pair{std:("review"), z->object(object{
        object::pair{std:("base"), z->number()->default(2)}, 
        object::pair{std:("approved"), z->number()->default(1)}, 
        object::pair{std:("changesRequested"), z->number()->default(1.5)}, 
        object::pair{std:("commented"), z->number()->default(0.5)}, 
        object::pair{std:("detailedFeedbackMultiplier"), z->number()->default(0.006)}, 
        object::pair{std:("thoroughnessMultiplier"), z->number()->default(1.2)}, 
        object::pair{std:("maxPerDay"), z->number()->default(15)}
    })}, 
    object::pair{std:("comment"), z->object(object{
        object::pair{std:("base"), z->number()->default(0.5)}, 
        object::pair{std:("substantiveMultiplier"), z->number()->default(0.006)}, 
        object::pair{std:("diminishingReturns"), z->number()->default(0.8)}, 
        object::pair{std:("maxPerThread"), z->number()->default(5)}
    })}, 
    object::pair{std:("reaction"), z->object(object{
        object::pair{std:("base"), z->number()->default(0.2)}, 
        object::pair{std:("received"), z->number()->default(0.1)}, 
        object::pair{std:("types"), z->record(z->string(), z->number())->default(object{
            object::pair{std:("thumbs_up"), 1}, 
            object::pair{std:("thumbs_down"), 0.5}, 
            object::pair{std:("laugh"), 1}, 
            object::pair{std:("hooray"), 1.5}, 
            object::pair{std:("confused"), 0.5}, 
            object::pair{std:("heart"), 1.5}, 
            object::pair{std:("rocket"), 1.2}, 
            object::pair{std:("eyes"), 1}
        })}, 
        object::pair{std:("maxPerDay"), z->number()->default(20)}, 
        object::pair{std:("diminishingReturns"), z->number()->default(0.8)}
    })}, 
    object::pair{std:("codeChange"), z->object(object{
        object::pair{std:("perLineAddition"), z->number()->default(0.01)}, 
        object::pair{std:("perLineDeletion"), z->number()->default(0.005)}, 
        object::pair{std:("perFile"), z->number()->default(0.1)}, 
        object::pair{std:("maxLines"), z->number()->default(1000)}, 
        object::pair{std:("testCoverageBonus"), z->number()->default(1.5)}
    })}
});
any TagTypeSchema = z->enum(array<string>{ std:("AREA"), std:("ROLE"), std:("TECH") });
any TagConfigSchema = z->object(object{
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("category"), TagTypeSchema}, 
    object::pair{std:("patterns"), z->array(z->string())}, 
    object::pair{std:("weight"), z->number()->default(1)}, 
    object::pair{std:("description"), z->string()->optional()}
});
any RepositoryConfigSchema = z->object(object{
    object::pair{std:("owner"), z->string()}, 
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("defaultBranch"), z->string()->default(std:("main"))}
});
any PipelineConfigSchema = z->object(object{
    object::pair{std:("contributionStartDate"), z->string()->regex((new RegExp(std:("^\d{4}-\d{2}-\d{2}"))))->optional()}, 
    object::pair{std:("repositories"), z->array(RepositoryConfigSchema)}, 
    object::pair{std:("scoring"), ScoringConfigSchema}, 
    object::pair{std:("tags"), z->object(object{
        object::pair{std:("area"), z->array(TagConfigSchema)}, 
        object::pair{std:("role"), z->array(TagConfigSchema)}, 
        object::pair{std:("tech"), z->array(TagConfigSchema)}
    })}, 
    object::pair{std:("botUsers"), z->array(z->string())->optional()}, 
    object::pair{std:("aiSummary"), AISummaryConfigSchema}, 
    object::pair{std:("walletAddresses"), z->object(object{
        object::pair{std:("enabled"), z->boolean()->default(true)}
    })}
});

void Main(void)
{
}

MAIN
