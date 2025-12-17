#include "problem-statement.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ProblemStatement problemStatementFromSimplifiedInput(const std::string& input, const std::string& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (type) {
            case 'text':
            return new TextProblemStatement({ text: input });
            case 'text_file':
            return new FileProblemStatement({ path: input });
            case 'github_issue':
            return new GithubIssue({ githubUrl: input });
            case 'swe_bench_multimodal':
            return new SWEBenchMultimodalProblemStatement({ text: input });
            default:
            throw std::runtime_error(`Unknown problem statement type: ${type}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
