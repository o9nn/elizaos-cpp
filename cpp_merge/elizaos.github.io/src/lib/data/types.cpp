#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/data/types.h"

any GithubUserSchema = z->object(object{
    object::pair{std:("login"), z->string()}, 
    object::pair{std:("avatarUrl"), z->string()->nullable()->optional()}
});
any RawReactionSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("content"), z->string()}, 
    object::pair{std:("createdAt"), z->string()}, 
    object::pair{std:("user"), GithubUserSchema->nullable()->optional()}
});
any RawCommitSchema = z->object(object{
    object::pair{std:("oid"), z->string()}, 
    object::pair{std:("message"), z->string()}, 
    object::pair{std:("messageHeadline"), z->string()->optional()}, 
    object::pair{std:("committedDate"), z->string()}, 
    object::pair{std:("author"), z->object(object{
        object::pair{std:("name"), z->string()}, 
        object::pair{std:("email"), z->string()}, 
        object::pair{std:("date"), z->string()}, 
        object::pair{std:("user"), GithubUserSchema->nullable()->optional()}
    })}, 
    object::pair{std:("additions"), z->number()->default(0)}, 
    object::pair{std:("deletions"), z->number()->default(0)}, 
    object::pair{std:("changedFiles"), z->number()->default(0)}
});
any RawPRFileSchema = z->object(object{
    object::pair{std:("path"), z->string()}, 
    object::pair{std:("additions"), z->number()->default(0)}, 
    object::pair{std:("deletions"), z->number()->default(0)}, 
    object::pair{std:("changeType"), z->string()->optional()}
});
any RawPRReviewSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("state"), z->string()}, 
    object::pair{std:("body"), z->string()->nullable()->optional()}, 
    object::pair{std:("createdAt"), z->string()->optional()}, 
    object::pair{std:("author"), GithubUserSchema->nullable()->optional()}, 
    object::pair{std:("url"), z->string()->optional()}
});
any RawCommentSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("body"), z->string()->nullable()->optional()}, 
    object::pair{std:("createdAt"), z->string()->optional()}, 
    object::pair{std:("updatedAt"), z->string()->optional()}, 
    object::pair{std:("author"), GithubUserSchema->nullable()->optional()}, 
    object::pair{std:("url"), z->string()->optional()}, 
    object::pair{std:("reactions"), z->object(object{
        object::pair{std:("totalCount"), z->number()}, 
        object::pair{std:("nodes"), z->array(RawReactionSchema)}
    })->optional()}
});
any RawLabelSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("color"), z->string()}, 
    object::pair{std:("description"), z->string()->nullable()->optional()}
});
any RawClosingIssueReferenceSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("number"), z->number()}, 
    object::pair{std:("title"), z->string()}, 
    object::pair{std:("state"), z->string()}
});
any RawPullRequestSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("number"), z->number()}, 
    object::pair{std:("title"), z->string()}, 
    object::pair{std:("body"), z->string()->nullable()->optional()}, 
    object::pair{std:("state"), z->string()}, 
    object::pair{std:("merged"), z->boolean()}, 
    object::pair{std:("createdAt"), z->string()}, 
    object::pair{std:("updatedAt"), z->string()}, 
    object::pair{std:("closedAt"), z->string()->nullable()->optional()}, 
    object::pair{std:("mergedAt"), z->string()->nullable()->optional()}, 
    object::pair{std:("headRefOid"), z->string()}, 
    object::pair{std:("baseRefOid"), z->string()}, 
    object::pair{std:("additions"), z->number()->default(0)}, 
    object::pair{std:("deletions"), z->number()->default(0)}, 
    object::pair{std:("changedFiles"), z->number()->default(0)}, 
    object::pair{std:("author"), GithubUserSchema->nullable()->optional()}, 
    object::pair{std:("labels"), z->object(object{
        object::pair{std:("nodes"), z->array(RawLabelSchema)}
    })->optional()}, 
    object::pair{std:("commits"), z->object(object{
        object::pair{std:("totalCount"), z->number()}, 
        object::pair{std:("nodes"), z->array(z->object(object{
            object::pair{std:("commit"), RawCommitSchema}
        }))}
    })->optional()}, 
    object::pair{std:("reviews"), z->object(object{
        object::pair{std:("nodes"), z->array(RawPRReviewSchema)}
    })->optional()}, 
    object::pair{std:("comments"), z->object(object{
        object::pair{std:("nodes"), z->array(RawCommentSchema)}
    })->optional()}, 
    object::pair{std:("files"), z->object(object{
        object::pair{std:("nodes"), z->array(RawPRFileSchema)}
    })->optional()}, 
    object::pair{std:("reactions"), z->object(object{
        object::pair{std:("totalCount"), z->number()}, 
        object::pair{std:("nodes"), z->array(RawReactionSchema)}
    })->optional()}, 
    object::pair{std:("closingIssuesReferences"), z->object(object{
        object::pair{std:("nodes"), z->array(RawClosingIssueReferenceSchema)}
    })->optional()}
});
any RawIssueSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("number"), z->number()}, 
    object::pair{std:("title"), z->string()}, 
    object::pair{std:("body"), z->string()->nullable()->optional()}, 
    object::pair{std:("state"), z->string()}, 
    object::pair{std:("locked"), z->boolean()->optional()}, 
    object::pair{std:("createdAt"), z->string()}, 
    object::pair{std:("updatedAt"), z->string()}, 
    object::pair{std:("closedAt"), z->string()->nullable()->optional()}, 
    object::pair{std:("author"), GithubUserSchema->nullable()->optional()}, 
    object::pair{std:("labels"), z->object(object{
        object::pair{std:("nodes"), z->array(RawLabelSchema)}
    })->optional()}, 
    object::pair{std:("comments"), z->object(object{
        object::pair{std:("totalCount"), z->number()}, 
        object::pair{std:("nodes"), z->array(RawCommentSchema)}
    })->optional()}, 
    object::pair{std:("reactions"), z->object(object{
        object::pair{std:("totalCount"), z->number()}, 
        object::pair{std:("nodes"), z->array(RawReactionSchema)}
    })->optional()}
});

void Main(void)
{
}

MAIN
