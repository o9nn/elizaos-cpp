#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_DATA_TAGS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_DATA_TAGS_H
#include "core.h"
#include "zod.h"

typedef z::infer<TagScoringSchema> TagScoring;

typedef z::infer<TagPatternSchema> TagPattern;

typedef z::infer<TagRuleSchema> TagRule;

enum struct TagPatternType;
enum struct TagCategory;

enum struct TagPatternType {
    FILE_PATH = std::string("FILE_PATH"), COMMIT_MESSAGE = std::string("COMMIT_MESSAGE"), CODE_CONTENT = std::string("CODE_CONTENT"), PR_TITLE = std::string("PR_TITLE"), PR_DESCRIPTION = std::string("PR_DESCRIPTION"), PR_CLOSES_ISSUE = std::string("PR_CLOSES_ISSUE"), ISSUE_TITLE = std::string("ISSUE_TITLE"), ISSUE_BODY = std::string("ISSUE_BODY"), COMMENT = std::string("COMMENT"), LABEL = std::string("LABEL"), REACTION = std::string("REACTION")
};
enum struct TagCategory {
    AREA = std::string("AREA"), ROLE = std::string("ROLE"), TECH = std::string("TECH")
};
extern any TagScoringSchema;
extern any TagPatternSchema;
extern any TagRuleSchema;
#endif
