#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

// GitHub User Schema (matches GraphQL Author type)
const GithubUserSchema = z.object({
  login: z.string(),
  avatarUrl: z.string().nullable().optional(),
});

// Reaction schema for GitHub reactions
const RawReactionSchema = z.object({
  id: z.string(),
  content: z.string(),
  createdAt: z.string(),
  user: GithubUserSchema.nullable().optional(),
});

// GitHub Raw Data Schemas
const RawCommitSchema = z.object({
  oid: z.string(),
  message: z.string(),
  messageHeadline: z.string().optional(),
  committedDate: z.string(),
  author: z.object({
    name: z.string(),
    email: z.string(),
    date: z.string(),
    user: GithubUserSchema.nullable().optional(),
  }),
  additions: z.number().default(0),
  deletions: z.number().default(0),
  changedFiles: z.number().default(0),
});

const RawPRFileSchema = z.object({
  path: z.string(),
  additions: z.number().default(0),
  deletions: z.number().default(0),
  changeType: z.string().optional(),
});

const RawPRReviewSchema = z.object({
  id: z.string(),
  state: z.string(),
  body: z.string().nullable().optional(),
  createdAt: z.string().optional(),
  author: GithubUserSchema.nullable().optional(),
  url: z.string().optional(),
});

const RawCommentSchema = z.object({
  id: z.string(),
  body: z.string().nullable().optional(),
  createdAt: z.string().optional(),
  updatedAt: z.string().optional(),
  author: GithubUserSchema.nullable().optional(),
  url: z.string().optional(),
  reactions: z
    .object({
      totalCount: z.number(),
      nodes: z.array(RawReactionSchema),
    })
    .optional(),
});

const RawLabelSchema = z.object({
  id: z.string(),
  name: z.string(),
  color: z.string(),
  description: z.string().nullable().optional(),
});

const RawClosingIssueReferenceSchema = z.object({
  id: z.string(),
  number: z.number(),
  title: z.string(),
  state: z.string(),
});

const RawPullRequestSchema = z.object({
  id: z.string(),
  number: z.number(),
  title: z.string(),
  body: z.string().nullable().optional(),
  state: z.string(),
  merged: z.boolean(),
  createdAt: z.string(),
  updatedAt: z.string(),
  closedAt: z.string().nullable().optional(),
  mergedAt: z.string().nullable().optional(),
  headRefOid: z.string(),
  baseRefOid: z.string(),
  additions: z.number().default(0),
  deletions: z.number().default(0),
  changedFiles: z.number().default(0),
  author: GithubUserSchema.nullable().optional(),
  labels: z
    .object({
      nodes: z.array(RawLabelSchema),
    })
    .optional(),
  commits: z
    .object({
      totalCount: z.number(),
      nodes: z.array(
        z.object({
          commit: RawCommitSchema,
        }),
      ),
    })
    .optional(),
  reviews: z
    .object({
      nodes: z.array(RawPRReviewSchema),
    })
    .optional(),
  comments: z
    .object({
      nodes: z.array(RawCommentSchema),
    })
    .optional(),
  files: z
    .object({
      nodes: z.array(RawPRFileSchema),
    })
    .optional(),
  reactions: z
    .object({
      totalCount: z.number(),
      nodes: z.array(RawReactionSchema),
    })
    .optional(),
  closingIssuesReferences: z
    .object({
      nodes: z.array(RawClosingIssueReferenceSchema),
    })
    .optional(),
});

const RawIssueSchema = z.object({
  id: z.string(),
  number: z.number(),
  title: z.string(),
  body: z.string().nullable().optional(),
  state: z.string(),
  locked: z.boolean().optional(),
  createdAt: z.string(),
  updatedAt: z.string(),
  closedAt: z.string().nullable().optional(),
  author: GithubUserSchema.nullable().optional(),
  labels: z
    .object({
      nodes: z.array(RawLabelSchema),
    })
    .optional(),
  comments: z
    .object({
      totalCount: z.number(),
      nodes: z.array(RawCommentSchema),
    })
    .optional(),
  reactions: z
    .object({
      totalCount: z.number(),
      nodes: z.array(RawReactionSchema),
    })
    .optional(),
});

using GithubUser = z.infer<typeof GithubUserSchema>;

struct DateRange {
    std::optional<std::string> startDate;
    std::optional<std::string> endDate;
};


} // namespace elizaos
