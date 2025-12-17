#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "elizaos/plugin-forms.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Plugin registry types - will be loaded dynamically
using PluginSearchResult = {

// Dynamic imports for plugin registry functions

// Define types that were imported before
struct CodeGenerationRequest {
    std::string projectName;
    std::string description;
    ProjectType targetType;
    std::vector<std::string> requirements;
    std::vector<std::string> apis;
    std::optional<std::vector<std::string>> testScenarios;
    std::optional<std::string> githubRepo;
};

struct GenerationFile {
    std::string path;
    std::string content;
};

struct GenerationResult {
    bool success;
    std::optional<std::string> projectPath;
    std::optional<std::string> githubUrl;
    std::optional<std::vector<GenerationFile>> files;
    std::optional<std::vector<std::string>> errors;
    std::optional<std::vector<std::string>> warnings;
    std::optional<ExecutionResults> executionResults;
};

struct ExecutionResults {
    std::optional<bool> testsPass;
    std::optional<bool> lintPass;
    std::optional<bool> typesPass;
    std::optional<bool> buildPass;
    std::optional<bool> buildSuccess;
    std::optional<bool> securityPass;
};

struct QualityResults {
    bool passed;
    double lintErrors;
    double typeErrors;
    double testsFailed;
    bool buildSuccess;
    std::vector<std::string> securityIssues;
    std::vector<std::string> details;
    std::optional<bool> testsPassed;
    std::optional<bool> lintPassed;
    std::optional<bool> typesPassed;
    std::optional<bool> buildPassed;
    std::optional<std::vector<std::string>> testErrors;
    std::optional<std::vector<std::string>> buildErrors;
};

struct APIResearch {
    std::string name;
    std::string documentation;
    std::vector<std::string> examples;
    std::vector<std::string> bestPractices;
};

struct SimilarProject {
    std::string name;
    std::string description;
    std::optional<std::vector<std::string>> relevantCode;
    std::vector<std::string> patterns;
};

struct ElizaContext {
    std::vector<std::string> coreTypes;
    std::vector<std::string> patterns;
    std::vector<std::string> conventions;
};

struct ResearchResult {
    std::vector<APIResearch> apis;
    std::vector<SimilarProject> similarProjects;
    ElizaContext elizaContext;
    std::vector<PluginSearchResult> existingPlugins;
};

struct PRDDocument {
    std::string title;
    std::string overview;
    std::vector<std::string> objectives;
    std::vector<std::string> functional;
    std::vector<std::string> nonFunctional;
    std::vector<std::string> technical;
    std::variant<'clone-existing', 'extend-existing', 'new-plugin'> approach;
    std::optional<std::string> basePlugin;
    std::vector<std::string> components;
    std::vector<std::string> dependencies;
    std::vector<std::string> apiKeys;
    std::vector<std::string> testScenarios;
    std::vector<std::string> successCriteria;
};

class CodeGenerationService {
public:
    CodeGenerationService(IAgentRuntime runtime, std::optional<FormsService> formsService);
    std::future<Service> start(IAgentRuntime runtime);
    std::string capabilityDescription() const;
    std::future<void> start();
    std::future<void> stop();
    std::future<ResearchResult> performResearch(CodeGenerationRequest request);
    std::future<PRDDocument> generatePRD(CodeGenerationRequest request, ResearchResult research);
    std::future<QualityResults> performQA(const std::string& projectPath);
    std::future<void> setupProjectWithStarter(const std::string& projectPath, CodeGenerationRequest request);
    std::future<void> createProjectDocumentation(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> generatePluginCode(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> cloneExistingPlugin(const std::string& projectPath, PRDDocument prd);
    std::future<void> extendExistingPlugin(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> generateNewPlugin(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> validateApiKeys(const std::vector<std::string>& requiredKeys);
    std::future<void> copyDirectory(const std::string& src, const std::string& dest);
    std::future<void> generateExtensionCode(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> generateIndexFile(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> generateActions(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> generateServices(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> generateProviders(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> generateTests(const std::string& projectPath, CodeGenerationRequest request, PRDDocument prd);
    std::future<void> fixValidationIssues(const std::string& projectPath, QualityResults validationResult);
    std::string buildIterativePrompt(CodeGenerationRequest request, double iteration);
    std::future<void> installDependencies(const std::string& projectPath);
    std::future<QualityResults> runValidationSuite(const std::string& projectPath);
    std::future<void> prepareFeedbackForNextIteration(const std::string& projectPath, QualityResults validationResult);
    std::future<GenerationResult> generateCode(CodeGenerationRequest request);
    std::future<GenerationResult> generateCodeWithoutSandbox(CodeGenerationRequest request);
    bool isTimeoutError(Error error);
     getTimeoutConfig();
    void if(auto isLocal || isDev);
    std::future<std::string> generateCodeWithTimeout(const std::string& prompt, number = 8000 maxTokens, std::optional<double> timeoutMs);
    std::future<GenerationResult> generateCodeInChunks(CodeGenerationRequest request);
    std::future<std::vector<GenerationFile>> generateEssentialFiles(CodeGenerationRequest request);
    std::future<std::string> generateWithTimeout(const std::string& prompt, double maxTokens, double timeoutMs);
    std::future<std::vector<GenerationFile>> getGeneratedFiles(const std::string& projectPath);
    std::vector<std::string> extractCodeExamples(const std::string& text);
    std::vector<std::string> extractBestPractices(const std::string& text);
    std::future<std::vector<SimilarProject>> searchSimilarProjects(CodeGenerationRequest request);
    std::future<ElizaContext> getElizaContext(const std::string& _targetType);
    double countErrors(const std::string& text, const std::string& errorType);
    std::vector<GenerationFile> parseGeneratedCode(const std::string& generatedContent, const std::any& _context);
    std::vector<std::string> extractSecurityIssues(const std::string& text);

private:
    IAgentRuntime runtime_;
    std::string projectsDir_;
};


} // namespace elizaos
