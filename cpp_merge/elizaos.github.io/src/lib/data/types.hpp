#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_DATA_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_DATA_TYPES_H
#include "core.h"
#include "zod.h"

typedef z::infer<GithubUserSchema> GithubUser;

class DateRange;

extern any GithubUserSchema;
extern any RawReactionSchema;
extern any RawCommitSchema;
extern any RawPRFileSchema;
extern any RawPRReviewSchema;
extern any RawCommentSchema;
extern any RawLabelSchema;
extern any RawClosingIssueReferenceSchema;
extern any RawPullRequestSchema;
extern any RawIssueSchema;
class DateRange : public object, public std::enable_shared_from_this<DateRange> {
public:
    using std::enable_shared_from_this<DateRange>::shared_from_this;
    string startDate;

    string endDate;
};

#endif
