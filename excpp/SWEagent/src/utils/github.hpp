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

/**
 * GitHub utilities
 * Converted from sweagent/utils/github.py
 */

;

class InvalidGithubURL extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'InvalidGithubURL';
  }
}

/**
 * Check if URL is a GitHub repository URL
 */


/**
 * Check if URL is a GitHub issue URL
 */


/**
 * Parse GitHub issue URL
 */
 {
  const match = issueUrl.match(/github\.com\/([^\/]+)\/([^\/]+)\/issues\/(\d+)/);
  if (!match) {
    throw new InvalidGithubURL(`Invalid GitHub issue URL: ${issueUrl}`);
  }
  return {
    owner: match[1],
    repo: match[2],
    issueNumber: match[3],
  };
}

/**
 * Parse GitHub repository URL
 */
 {
  let match = repoUrl.match(/github\.com[\/:]([^\/]+)\/([^\/\.]+)/);
  if (!match) {
    match = repoUrl.match(/^gh:([^\/]+)\/([^\/]+)$/);
  }
  if (!match) {
    throw new InvalidGithubURL(`Invalid GitHub repository URL: ${repoUrl}`);
  }
  return {
    owner: match[1],
    repo: match[2].replace(/\.git$/, ''),
  };
}

/**
 * Get GitHub issue data
 */
std::future<std::any> getGhIssueData(const std::string& issueUrl, string = '' token); = parseGhIssueUrl(issueUrl);

  const headers: Record<string, string> = {
    Accept: 'application/vnd.github.v3+json',
  };

  if (token) {
    headers['Authorization'] = `token ${token}`;
  }

  const response = await axios.get(`https://api.github.com/repos/${owner}/${repo}/issues/${issueNumber}`, { headers });

  return response.data;
}

/**
 * Get problem statement from GitHub issue
 */
std::future<std::string> getProblemStatementFromGithubIssue(const std::string& owner, const std::string& repo, const std::string& issueNumber, string = '' token);;

  if (token) {
    headers['Authorization'] = `token ${token}`;
  }

  const response = await axios.get(`https://api.github.com/repos/${owner}/${repo}/issues/${issueNumber}`, { headers });

  const issue = response.data;
  return `${issue.title}\n\n${issue.body || ''}`;
}

/**
 * Get associated commit URLs for an issue
 */
std::future<std::vector<std::string>> getAssociatedCommitUrls(const std::string& org, const std::string& repo, const std::string& issueNumber, string = '' token);;

  if (token) {
    headers['Authorization'] = `token ${token}`;
  }

  // Get events for the issue
  const response = await axios.get(`https://api.github.com/repos/${org}/${repo}/issues/${issueNumber}/events`, {
    headers,
  });

  const commitUrls: string[] = [];
  for (const event of response.data) {
    if (event.event === 'referenced' && event.commit_id) {
      commitUrls.push(`https://github.com/${org}/${repo}/commit/${event.commit_id}`);
    }
  }

  return commitUrls;
}

} // namespace elizaos
