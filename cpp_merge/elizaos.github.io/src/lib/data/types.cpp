#include "types.hpp"
#include <string>

any GithubUserSchema = z->object(object{
    object::pair{std::string("login"), z->string()}, 
    object::pair{std::string("avatarUrl"), z->string()->nullable()->optional()}
});
any RawReactionSchema = z->object(object{
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("content"), z->string()}, 
    object::pair{std::string("createdAt"), z->string()}, 
    object::pair{std::string("user"), GithubUserSchema->nullable()->optional()}
});
any RawCommitSchema = z->object(object{
    object::pair{std::string("oid"), z->string()}, 
    object::pair{std::string("message"), z->string()}, 
    object::pair{std::string("messageHeadline"), z->string()->optional()}, 
    object::pair{std::string("committedDate"), z->string()}, 
    object::pair{std::string("author"), z->object(object{
        object::pair{std::string("name"), z->string()}, 
        object::pair{std::string("email"), z->string()}, 
        object::pair{std::string("date"), z->string()}, 
        object::pair{std::string("user"), GithubUserSchema->nullable()->optional()}
    })}, 
    object::pair{std::string("additions"), z->number()->default(0)}, 
    object::pair{std::string("deletions"), z->number()->default(0)}, 
    object::pair{std::string("changedFiles"), z->number()->default(0)}
});
any RawPRFileSchema = z->object(object{
    object::pair{std::string("path"), z->string()}, 
    object::pair{std::string("additions"), z->number()->default(0)}, 
    object::pair{std::string("deletions"), z->number()->default(0)}, 
    object::pair{std::string("changeType"), z->string()->optional()}
});
any RawPRReviewSchema = z->object(object{
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("state"), z->string()}, 
    object::pair{std::string("body"), z->string()->nullable()->optional()}, 
    object::pair{std::string("createdAt"), z->string()->optional()}, 
    object::pair{std::string("author"), GithubUserSchema->nullable()->optional()}, 
    object::pair{std::string("url"), z->string()->optional()}
});
any RawCommentSchema = z->object(object{
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("body"), z->string()->nullable()->optional()}, 
    object::pair{std::string("createdAt"), z->string()->optional()}, 
    object::pair{std::string("updatedAt"), z->string()->optional()}, 
    object::pair{std::string("author"), GithubUserSchema->nullable()->optional()}, 
    object::pair{std::string("url"), z->string()->optional()}, 
    object::pair{std::string("reactions"), z->object(object{
        object::pair{std::string("totalCount"), z->number()}, 
        object::pair{std::string("nodes"), z->array(RawReactionSchema)}
    })->optional()}
});
any RawLabelSchema = z->object(object{
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("name"), z->string()}, 
    object::pair{std::string("color"), z->string()}, 
    object::pair{std::string("description"), z->string()->nullable()->optional()}
});
any RawClosingIssueReferenceSchema = z->object(object{
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("number"), z->number()}, 
    object::pair{std::string("title"), z->string()}, 
    object::pair{std::string("state"), z->string()}
});
any RawPullRequestSchema = z->object(object{
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("number"), z->number()}, 
    object::pair{std::string("title"), z->string()}, 
    object::pair{std::string("body"), z->string()->nullable()->optional()}, 
    object::pair{std::string("state"), z->string()}, 
    object::pair{std::string("merged"), z->boolean()}, 
    object::pair{std::string("createdAt"), z->string()}, 
    object::pair{std::string("updatedAt"), z->string()}, 
    object::pair{std::string("closedAt"), z->string()->nullable()->optional()}, 
    object::pair{std::string("mergedAt"), z->string()->nullable()->optional()}, 
    object::pair{std::string("headRefOid"), z->string()}, 
    object::pair{std::string("baseRefOid"), z->string()}, 
    object::pair{std::string("additions"), z->number()->default(0)}, 
    object::pair{std::string("deletions"), z->number()->default(0)}, 
    object::pair{std::string("changedFiles"), z->number()->default(0)}, 
    object::pair{std::string("author"), GithubUserSchema->nullable()->optional()}, 
    object::pair{std::string("labels"), z->object(object{
        object::pair{std::string("nodes"), z->array(RawLabelSchema)}
    })->optional()}, 
    object::pair{std::string("commits"), z->object(object{
        object::pair{std::string("totalCount"), z->number()}, 
        object::pair{std::string("nodes"), z->array(z->object(object{
            object::pair{std::string("commit"), RawCommitSchema}
        }))}
    })->optional()}, 
    object::pair{std::string("reviews"), z->object(object{
        object::pair{std::string("nodes"), z->array(RawPRReviewSchema)}
    })->optional()}, 
    object::pair{std::string("comments"), z->object(object{
        object::pair{std::string("nodes"), z->array(RawCommentSchema)}
    })->optional()}, 
    object::pair{std::string("files"), z->object(object{
        object::pair{std::string("nodes"), z->array(RawPRFileSchema)}
    })->optional()}, 
    object::pair{std::string("reactions"), z->object(object{
        object::pair{std::string("totalCount"), z->number()}, 
        object::pair{std::string("nodes"), z->array(RawReactionSchema)}
    })->optional()}, 
    object::pair{std::string("closingIssuesReferences"), z->object(object{
        object::pair{std::string("nodes"), z->array(RawClosingIssueReferenceSchema)}
    })->optional()}
});
any RawIssueSchema = z->object(object{
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("number"), z->number()}, 
    object::pair{std::string("title"), z->string()}, 
    object::pair{std::string("body"), z->string()->nullable()->optional()}, 
    object::pair{std::string("state"), z->string()}, 
    object::pair{std::string("locked"), z->boolean()->optional()}, 
    object::pair{std::string("createdAt"), z->string()}, 
    object::pair{std::string("updatedAt"), z->string()}, 
    object::pair{std::string("closedAt"), z->string()->nullable()->optional()}, 
    object::pair{std::string("author"), GithubUserSchema->nullable()->optional()}, 
    object::pair{std::string("labels"), z->object(object{
        object::pair{std::string("nodes"), z->array(RawLabelSchema)}
    })->optional()}, 
    object::pair{std::string("comments"), z->object(object{
        object::pair{std::string("totalCount"), z->number()}, 
        object::pair{std::string("nodes"), z->array(RawCommentSchema)}
    })->optional()}, 
    object::pair{std::string("reactions"), z->object(object{
        object::pair{std::string("totalCount"), z->number()}, 
        object::pair{std::string("nodes"), z->array(RawReactionSchema)}
    })->optional()}
});

void Main(void)
{
}

MAIN
