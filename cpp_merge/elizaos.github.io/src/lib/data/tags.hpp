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
    FILE_PATH = std:("FILE_PATH"), COMMIT_MESSAGE = std:("COMMIT_MESSAGE"), CODE_CONTENT = std:("CODE_CONTENT"), PR_TITLE = std:("PR_TITLE"), PR_DESCRIPTION = std:("PR_DESCRIPTION"), PR_CLOSES_ISSUE = std:("PR_CLOSES_ISSUE"), ISSUE_TITLE = std:("ISSUE_TITLE"), ISSUE_BODY = std:("ISSUE_BODY"), COMMENT = std:("COMMENT"), LABEL = std:("LABEL"), REACTION = std:("REACTION")
};
enum struct TagCategory {
    AREA = std:("AREA"), ROLE = std:("ROLE"), TECH = std:("TECH")
};
extern any TagScoringSchema;
extern any TagPatternSchema;
extern any TagRuleSchema;
#endif
