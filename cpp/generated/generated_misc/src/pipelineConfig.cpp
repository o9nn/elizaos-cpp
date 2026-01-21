#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/pipelineConfig.h"

std::any ScoringConfigSchema = z->object(object{
    object::pair{std::string("pullRequest"), z->object(object{
        object::pair{std::string("base"), z->number()->default(7)}, 
        object::pair{std::string("merged"), z->number()->default(3)}, 
        object::pair{std::string("perReview"), z->number()->default(3)}, 
        object::pair{std::string("perApproval"), z->number()->default(2)}, 
        object::pair{std::string("perComment"), z->number()->default(0.5)}, 
        object::pair{std::string("descriptionMultiplier"), z->number()->default(0.006)}, 
        object::pair{std::string("complexityMultiplier"), z->number()->default(0.5)}, 
        object::pair{std::string("optimalSizeBonus"), z->number()->default(5)}, 
        object::pair{std::string("maxPerDay"), z->number()->default(20)}, 
        object::pair{std::string("closingIssueBonus"), z->number()->default(4)}
    })}, 
    object::pair{std::string("issue"), z->object(object{
        object::pair{std::string("base"), z->number()->default(5)}, 
        object::pair{std::string("perComment"), z->number()->default(0.5)}, 
        object::pair{std::string("withLabelsMultiplier"), z->record(z->std::string(), z->number())->default(object{
            object::pair{std::string("bug"), 1.5}, 
            object::pair{std::string("enhancement"), 1.2}, 
            object::pair{std::string("documentation"), 1}
        })}, 
        object::pair{std::string("closedBonus"), z->number()->default(3)}, 
        object::pair{std::string("resolutionSpeedMultiplier"), z->number()->default(0.8)}
    })}, 
    object::pair{std::string("review"), z->object(object{
        object::pair{std::string("base"), z->number()->default(2)}, 
        object::pair{std::string("approved"), z->number()->default(1)}, 
        object::pair{std::string("changesRequested"), z->number()->default(1.5)}, 
        object::pair{std::string("commented"), z->number()->default(0.5)}, 
        object::pair{std::string("detailedFeedbackMultiplier"), z->number()->default(0.006)}, 
        object::pair{std::string("thoroughnessMultiplier"), z->number()->default(1.2)}, 
        object::pair{std::string("maxPerDay"), z->number()->default(15)}
    })}, 
    object::pair{std::string("comment"), z->object(object{
        object::pair{std::string("base"), z->number()->default(0.5)}, 
        object::pair{std::string("substantiveMultiplier"), z->number()->default(0.006)}, 
        object::pair{std::string("diminishingReturns"), z->number()->default(0.8)}, 
        object::pair{std::string("maxPerThread"), z->number()->default(5)}
    })}, 
    object::pair{std::string("reaction"), z->object(object{
        object::pair{std::string("base"), z->number()->default(0.2)}, 
        object::pair{std::string("received"), z->number()->default(0.1)}, 
        object::pair{std::string("types"), z->record(z->std::string(), z->number())->default(object{
            object::pair{std::string("thumbs_up"), 1}, 
            object::pair{std::string("thumbs_down"), 0.5}, 
            object::pair{std::string("laugh"), 1}, 
            object::pair{std::string("hooray"), 1.5}, 
            object::pair{std::string("confused"), 0.5}, 
            object::pair{std::string("heart"), 1.5}, 
            object::pair{std::string("rocket"), 1.2}, 
            object::pair{std::string("eyes"), 1}
        })}, 
        object::pair{std::string("maxPerDay"), z->number()->default(20)}, 
        object::pair{std::string("diminishingReturns"), z->number()->default(0.8)}
    })}, 
    object::pair{std::string("codeChange"), z->object(object{
        object::pair{std::string("perLineAddition"), z->number()->default(0.01)}, 
        object::pair{std::string("perLineDeletion"), z->number()->default(0.005)}, 
        object::pair{std::string("perFile"), z->number()->default(0.1)}, 
        object::pair{std::string("maxLines"), z->number()->default(1000)}, 
        object::pair{std::string("testCoverageBonus"), z->number()->default(1.5)}
    })}
});
std::any TagTypeSchema = z->enum(array<string>{ std::string("AREA"), std::string("ROLE"), std::string("TECH") });
std::any TagConfigSchema = z->object(object{
    object::pair{std::string("name"), z->std::string()}, 
    object::pair{std::string("category"), TagTypeSchema}, 
    object::pair{std::string("patterns"), z->array(z->std::string())}, 
    object::pair{std::string("weight"), z->number()->default(1)}, 
    object::pair{std::string("description"), z->std::string()->std::optional()}
});
std::any RepositoryConfigSchema = z->object(object{
    object::pair{std::string("owner"), z->std::string()}, 
    object::pair{std::string("name"), z->std::string()}, 
    object::pair{std::string("defaultBranch"), z->std::string()->default(std::string("main"))}
});
std::any PipelineConfigSchema = z->object(object{
    object::pair{std::string("contributionStartDate"), z->std::string()->regex((new RegExp(std::string("^\d{4}-\d{2}-\d{2}"))))->std::optional()}, 
    object::pair{std::string("repositories"), z->array(RepositoryConfigSchema)}, 
    object::pair{std::string("scoring"), ScoringConfigSchema}, 
    object::pair{std::string("tags"), z->object(object{
        object::pair{std::string("area"), z->array(TagConfigSchema)}, 
        object::pair{std::string("role"), z->array(TagConfigSchema)}, 
        object::pair{std::string("tech"), z->array(TagConfigSchema)}
    })}, 
    object::pair{std::string("botUsers"), z->array(z->std::string())->std::optional()}, 
    object::pair{std::string("aiSummary"), AISummaryConfigSchema}, 
    object::pair{std::string("walletAddresses"), z->object(object{
        object::pair{std::string("enabled"), z->boolean()->default(true)}
    })}
});

void Main(void)
{
}

MAIN
