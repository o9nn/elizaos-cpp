#include "repo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::string> getGitResetCommands(const std::string& baseCommit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return [;
    'git fetch',
    'git status',
    'git restore .',
    'git reset --hard',
    "git checkout " + std::to_string(baseCommit)
    'git clean -fdq',
    ];

}

Repo repoFromSimplifiedInput(const std::string& input, string = 'HEAD' baseCommit, const std::variant<'local', 'github', 'preexisting', 'auto' = 'auto'>& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (type == 'auto') {
            // Auto-detect type
            if (input.includes('github.com')) {
                type = 'github';
                } else if (fs.existsSync(input)) {
                    type = 'local';
                    } else {
                        type = 'preexisting';
                    }
                }

                switch (type) {
                    case 'github':
                    return new GithubRepo({ githubUrl: input, baseCommit, type: 'github', cloneTimeout: 500 });
                    case 'local':
                    return new LocalRepo({ path: input, baseCommit, type: 'local' });
                    case 'preexisting':
                    return new PreExistingRepo({ repoName: input, baseCommit, type: 'preexisting', reset: true });
                    default:
                    throw std::runtime_error(`Unknown repo type: ${type}`);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
