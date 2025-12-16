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
;
;

dotenv.config();

const ORG_NAME = "elizaos-plugins";
const TARGET_BRANCH = "1.x";

std::future<void> main();

  const octokit = new Octokit({ auth: token });

  const repos = await octokit.paginate(octokit.repos.listForOrg, {
    org: ORG_NAME,
    per_page: 100,
  });

  for (const repo of repos) {
    let fileData;

    // Check if 1.x branch exists, if not skip
    try {
      await octokit.repos.getBranch({
        owner: ORG_NAME,
        repo: repo.name,
        branch: TARGET_BRANCH,
      });
    } catch (error: any) {
      if (error.status === 404) {
        console.log(
          `Skipping ${ORG_NAME}/${repo.name} (no ${TARGET_BRANCH} branch)`
        );
        continue;
      }
      throw error;
    }

    try {
      const response = await octokit.repos.getContent({
        owner: ORG_NAME,
        repo: repo.name,
        path: "package.json",
        ref: TARGET_BRANCH,
      });
      if (Array.isArray(response.data) || !("content" in response.data)) {
        continue;
      }
      fileData = response.data;
    } catch (error: any) {
      if (error.status === 404) {
        console.log(
          `Skipping ${ORG_NAME}/${repo.name} (no package.json on ${TARGET_BRANCH})`
        );
        continue;
      }
      throw error;
    }

    const raw = Buffer.from(fileData.content, "base64").toString("utf8");
    const pkg = JSON.parse(raw) as {
      name?: string;
      version?: string;
      [key: string]: any;
    };
    const oldName = pkg.name;

    // Skip if name is already correct (uses @elizaos/ scope)
    if (oldName && oldName.startsWith("@elizaos/")) {
      console.log(
        `Skipping ${ORG_NAME}/${repo.name} (package name already correct: ${oldName})`
      );
      continue;
    }

    // Skip if name exists but doesn't start with @elizaos-plugins/ (and isn't empty)
    if (oldName && !oldName.startsWith("@elizaos-plugins/")) {
      console.log(
        `Skipping ${ORG_NAME}/${repo.name} (package name doesn't match expected pattern: ${oldName})`
      );
      continue;
    }

    // Determine the new name
    let newName: string;
    if (!oldName) {
      // No name field - generate the correct name from repo name
      newName = `@elizaos/${repo.name}`;
      console.log(
        `Adding missing package name for ${ORG_NAME}/${repo.name}: ${newName}`
      );
    } else {
      // Has @elizaos-plugins/ name - convert it
      newName = oldName.replace(/^@elizaos-plugins\//, "@elizaos/");
      console.log(
        `Renaming ${oldName} to ${newName} for ${ORG_NAME}/${repo.name}`
      );
    }

    pkg.name = newName;

    // Bump the version number appropriately
    if (pkg.version) {
      if (pkg.version.includes("-beta.")) {
        // Handle beta versions (e.g., 1.0.0-beta.55 -> 1.0.0-beta.56)
        const betaMatch = pkg.version.match(/^(.+)-beta\.(\d+)$/);
        if (betaMatch && betaMatch[1] && betaMatch[2]) {
          const baseVersion = betaMatch[1];
          const betaNumber = parseInt(betaMatch[2], 10);
          if (!isNaN(betaNumber)) {
            pkg.version = `${baseVersion}-beta.${betaNumber + 1}`;
          }
        }
      } else {
        // Handle regular semantic versions
        const versionParts = pkg.version.split(".");
        if (versionParts.length >= 3 && versionParts[2]) {
          // Increment patch version
          const patchVersion = parseInt(versionParts[2], 10);
          if (!isNaN(patchVersion)) {
            versionParts[2] = (patchVersion + 1).toString();
            pkg.version = versionParts.join(".");
          }
        }
      }
    }

    const updated = Buffer.from(
      JSON.stringify(pkg, null, 2) + "\n",
      "utf8"
    ).toString("base64");

    await octokit.repos.createOrUpdateFileContents({
      owner: ORG_NAME,
      repo: repo.name,
      path: "package.json",
      branch: TARGET_BRANCH,
      message: `chore: rename scope to @elizaos and bump version in package.json`,
      content: updated,
      sha: fileData.sha,
    });

    console.log(
      `Updated package.json in ${ORG_NAME}/${repo.name} on ${TARGET_BRANCH} branch - renamed ${oldName} to ${newName} and bumped version to ${pkg.version}`
    );
  }
}

main().catch((error) => {
  console.error(error);
  process.exit(1);
});

} // namespace elizaos
