#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/autodoc/src/DocumentationGenerator.h"

DocumentationGenerator::DocumentationGenerator(std::shared_ptr<DirectoryTraversal> directoryTraversal_, std::shared_ptr<TypeScriptParser> typeScriptParser_, std::shared_ptr<JsDocAnalyzer> jsDocAnalyzer_, std::shared_ptr<JsDocGenerator> jsDocGenerator_, std::shared_ptr<GitManager> gitManager_, std::shared_ptr<Configuration> configuration_, std::shared_ptr<AIService> aiService_) : directoryTraversal(directoryTraversal_), typeScriptParser(typeScriptParser_), jsDocAnalyzer(jsDocAnalyzer_), jsDocGenerator(jsDocGenerator_), gitManager(gitManager_), configuration(configuration_), aiService(aiService_)  {
    this->typeScriptFiles = this->directoryTraversal->traverse();
    this->jsDocValidator = std::make_shared<JSDocValidator>(aiService);
}

std::shared_ptr<Promise<object>> DocumentationGenerator::generate(double pullNumber)
{
    auto fileChanges = array<any>();
    this->fileOffsets->clear();
    if (pullNumber) {
        auto prFiles = std::async([=]() { this->gitManager->getFilesInPullRequest(pullNumber); });
        fileChanges = prFiles->filter([=](auto file) mutable
        {
            shared absolutePath = this->configuration->toAbsolutePath(file->filename);
            auto isInTargetDir = absolutePath->startsWith(this->configuration->get_absolutePath());
            shared relativeToTarget = path->relative(this->configuration->get_absolutePath(), absolutePath);
            auto isExcluded = OR((this->configuration->excludedDirectories->some([=](auto dir) mutable
            {
                return const_(relativeToTarget->split(path->sep))[0] == dir;
            }
            )), (this->configuration->excludedFiles->some([=](auto excludedFile) mutable
            {
                return path->basename(absolutePath) == excludedFile;
            }
            )));
            return AND((isInTargetDir), (!isExcluded));
        }
        );
    } else {
        auto typeScriptFiles = this->directoryTraversal->traverse();
        fileChanges = typeScriptFiles->map([=](auto file) mutable
        {
            return (object{
                object::pair{std::string("filename"), this->configuration->toRelativePath(file)}, 
                object::pair{std::string("status"), std::string("modified")}
            });
        }
        );
    }
    for (auto& fileChange : fileChanges)
    {
        if (fileChange->status == std::string("deleted")) continue;
        auto filePath = this->configuration->toAbsolutePath(fileChange->filename);
        this->fileOffsets->set(filePath, 0);
        if (AND((fileChange->status == std::string("added")), (in(std::string("contents_url"), fileChange)))) {
            console->log(std::string("Getting file content from GitHub API"));
            auto fileContent = std::async([=]() { this->getFileContent(fileChange->contents_url); });
            this->fileContents->set(filePath, fileContent);
        } else {
            auto fileContent = fs->readFileSync(filePath, std::string("utf-8"));
            this->fileContents->set(filePath, fileContent);
        }
        auto ast = this->typeScriptParser->parse(filePath);
        if (OR((!ast), (!ast["body"]))) {
            console->log(std::string("Invalid AST found for file"), filePath);
            continue;
        }
        this->jsDocAnalyzer->analyze(ast);
        for (auto& node : ast["body"])
        {
            this->processNode(node, filePath, ast);
        }
    }
    if (this->missingJsDocQueue->get_length() > 0) {
        if (this->configuration->get_generateJsDoc()) {
            this->branchName = std::string("docs-update-") + (OR((pullNumber), (std::string("full")))) + std::string("-") + Date->now() + string_empty;
            std::async([=]() { this->gitManager->createBranch(this->branchName, this->configuration->get_branch()); });
        }
        for (auto& queueItem : this->missingJsDocQueue)
        {
            auto comment = string_empty;
            if (queueItem->className != undefined) {
                comment = std::async([=]() { this->jsDocGenerator->generateClassComment(queueItem); });
            } else {
                comment = std::async([=]() { this->jsDocGenerator->generateComment(queueItem); });
            }
            if (this->configuration->get_generateJsDoc()) {
                std::async([=]() { this->updateFileWithJSDoc(queueItem->filePath, comment, queueItem->startLine); });
                this->hasChanges = true;
            }
            queueItem->jsDoc = comment;
            this->existingJsDocQueue->push(queueItem);
        }
        if (AND((this->hasChanges), (this->branchName))) {
            for (auto& [filePath, content] : this->fileContents)
            {
                std::async([=]() { this->gitManager->commitFile(this->branchName, this->configuration->toRelativePath(filePath), content, std::string("docs: Add JSDoc documentation to ") + path->basename(filePath) + string_empty); });
            }
            auto prContent = std::async([=]() { this->generatePRContent(pullNumber); });
            std::async([=]() { this->gitManager->createPullRequest(object{
                object::pair{std::string("title"), prContent["title"]}, 
                object::pair{std::string("body"), prContent["body"]}, 
                object::pair{std::string("head"), this->branchName}, 
                object::pair{std::string("base"), this->configuration->get_branch()}, 
                object::pair{std::string("labels"), array<string>{ std::string("documentation"), std::string("automated-pr") }}, 
                object::pair{std::string("reviewers"), OR((this->configuration->pullRequestReviewers), (array<any>()))}
            }); });
        }
    }
    return object{
        object::pair{std::string("documentedItems"), this->existingJsDocQueue}, 
        object::pair{std::string("branchName"), this->branchName}
    };
}

void DocumentationGenerator::processNode(std::shared_ptr<TSESTree::Node> node, string filePath, std::shared_ptr<TSESTree::Program> ast)
{
    if (!this->jsDocAnalyzer->shouldHaveJSDoc(node)) return;
    auto jsDocComment = this->jsDocAnalyzer->getJSDocComment(node, OR((ast->comments), (array<any>())));
    auto queueItem = this->jsDocAnalyzer->createQueueItem(node, filePath, this->getNodeCode(filePath, node));
    if (jsDocComment) {
        queueItem->jsDoc = jsDocComment;
        this->existingJsDocQueue->push(queueItem);
    } else {
        this->missingJsDocQueue->push(queueItem);
    }
    auto children = this->jsDocAnalyzer->getDocumentableChildren(node);
    for (auto& child : children)
    {
        auto childJsDocComment = this->jsDocAnalyzer->getJSDocComment(child, OR((ast->comments), (array<any>())));
        auto childQueueItem = this->jsDocAnalyzer->createQueueItem(child, filePath, this->getNodeCode(filePath, child));
        if (childJsDocComment) {
            childQueueItem->jsDoc = childJsDocComment;
            this->existingJsDocQueue->push(childQueueItem);
        } else {
            this->missingJsDocQueue->push(childQueueItem);
        }
    }
}

std::shared_ptr<Promise<void>> DocumentationGenerator::updateFileWithJSDoc(string filePath, string jsDoc, double insertLine)
{
    auto content = OR((this->fileContents->get(filePath)), (string_empty));
    auto lines = content->split(std::string("\
"));
    auto currentOffset = OR((this->fileOffsets->get(filePath)), (0));
    auto adjustedLine = insertLine + currentOffset;
    auto fileName = OR((filePath->split(std::string("/"))->pop()), (string_empty));
    lines->splice(adjustedLine - 1, 0, jsDoc);
    auto newContent = lines->join(std::string("\
"));
    try
    {
        auto validatedJSDoc = std::async([=]() { this->jsDocValidator->validateAndFixJSDoc(fileName, newContent, jsDoc); });
        if (validatedJSDoc != jsDoc) {
            lines[adjustedLine - 1] = validatedJSDoc;
            auto newLines = (OR((validatedJSDoc->match((new RegExp(std::string("\n"))))), (array<any>())))->length + 1;
            this->fileOffsets->set(filePath, currentOffset + newLines);
        } else {
            auto newLines = (OR((jsDoc->match((new RegExp(std::string("\n"))))), (array<any>())))->length + 1;
            this->fileOffsets->set(filePath, currentOffset + newLines);
        }
        this->fileContents->set(filePath, lines->join(std::string("\
")));
    }
    catch (const any& error)
    {
        console->error(std::string("Error validating JSDoc in ") + filePath + std::string(":"), error);
        throw any(error);
    }
    return std::shared_ptr<Promise<void>>();
}

string DocumentationGenerator::getNodeCode(string filePath, std::shared_ptr<TSESTree::Node> node)
{
    auto fileContent = fs->readFileSync(filePath, std::string("utf-8"));
    auto lines = fileContent->split(std::string("\
"));
    auto startLine = OR((node->loc->start->line), (0));
    auto endLine = OR((node->loc->end->line), (0));
    return lines->slice(startLine - 1, endLine)->join(std::string("\
"));
}

std::shared_ptr<Promise<string>> DocumentationGenerator::getFileContent(string contentsUrl)
{
    try
    {
        auto response = std::async([=]() { fetch(contentsUrl); });
        auto data = std::async([=]() { response->json(); });
        return Buffer::from(data["content"], std::string("base64"))->toString(std::string("utf-8"));
    }
    catch (const any& _error)
    {
        console->error(std::string("Error fetching file content from GitHub API, ensure the PR has been merged"));
        return string_empty;
    }
}

std::shared_ptr<Promise<object>> DocumentationGenerator::generatePRContent(double pullNumber)
{
    auto modifiedFiles = Array->from(this->fileContents->keys());
    auto filesContext = modifiedFiles->map([=](auto file) mutable
    {
        return std::string("- ") + file + string_empty;
    }
    )->join(std::string("\
"));
    auto prompt = std::string("Create a JSON object for a pull request about JSDoc documentation updates.\
    The JSON must have exactly this format, with no extra fields or markdown formatting:\
    {\
        "title": "Brief title describing JSDoc updates",\
        "body": "Detailed description of changes"\
    }\
\
    Context for generating the content:\
    - ") + modifiedFiles->get_length() + std::string(" files were modified\
    - Files modified:\
") + filesContext + std::string("\
    - This is ") + (pullNumber) ? any(std::string("related to PR #") + pullNumber + string_empty) : any(std::string("a full repository documentation update")) + std::string("\
    - This is an automated PR for adding JSDoc documentation\
\
    The title should be concise and follow conventional commit format.\
    The body should include:\
    1. A clear summary of changes\
    2. List of modified files\
    3. Brief instructions for reviewers\
\
    Return ONLY the JSON object, no other text.");
    auto response = std::async([=]() { this->aiService->generateComment(prompt); });
    try
    {
        auto jsonStart = response->indexOf(std::string("{"));
        auto jsonEnd = response->lastIndexOf(std::string("}")) + 1;
        if (OR((jsonStart == -1), (jsonEnd == -1))) {
            throw any(std::make_shared<Error>(std::string("No valid JSON object found in response")));
        }
        auto jsonStr = response->slice(jsonStart, jsonEnd)->replace((new RegExp(std::string("```json"))), string_empty)->replace((new RegExp(std::string("```"))), string_empty)->trim();
        auto content = JSON->parse(jsonStr);
        if (OR((OR((OR((!content["title"]), (!content["body"]))), (type_of(content["title"]) != std::string("string")))), (type_of(content["body"]) != std::string("string")))) {
            throw any(std::make_shared<Error>(std::string("Invalid JSON structure")));
        }
        return object{
            object::pair{std::string("title"), content["title"]}, 
            object::pair{std::string("body"), content["body"]}
        };
    }
    catch (const any& error)
    {
        console->error(std::string("Error parsing AI response for PR content:"), error);
        console->error(std::string("Raw response:"), response);
        return object{
            object::pair{std::string("title"), std::string("docs: Add JSDoc documentation") + (pullNumber) ? any(std::string(" for PR #") + pullNumber + string_empty) : any(string_empty) + string_empty}, 
            object::pair{std::string("body"), this->generateDefaultPRBody()}
        };
    }
}

string DocumentationGenerator::generateDefaultPRBody()
{
    auto changes = Array->from(this->fileContents->keys())->map([=](auto filePath) mutable
    {
        return std::string("- Added JSDoc documentation to `") + filePath + std::string("`");
    }
    )->join(std::string("\
"));
    return std::string("## 📝 Documentation Updates\
        This PR adds JSDoc documentation to TypeScript files that were missing proper documentation.\
\
        ### 🔍 Changes Made:\
        ") + changes + std::string("\
\
        ### 🤖 Generated by Documentation Bot\
        This is an automated PR created by the documentation generator tool.");
}

std::shared_ptr<Promise<object>> DocumentationGenerator::analyzeCodebase()
{
    auto todoItems = array<std::shared_ptr<TodoItem>>();
    auto envUsages = array<std::shared_ptr<EnvUsage>>();
    for (auto& filePath : this->typeScriptFiles)
    {
        auto ast = this->typeScriptParser->parse(filePath);
        if (!ast) continue;
        auto sourceCode = fs->readFileSync(filePath, std::string("utf-8"));
        this->jsDocAnalyzer->findTodoComments(ast, OR((ast["comments"]), (array<any>())), sourceCode);
        todoItems->push(const_(this->jsDocAnalyzer->todoItems)[0]);
        this->jsDocAnalyzer->findEnvUsages(ast, sourceCode);
        envUsages->push(const_(this->jsDocAnalyzer->envUsages)[0]);
    }
    return object{
        object::pair{std::string("todoItems"), std::string("todoItems")}, 
        object::pair{std::string("envUsages"), std::string("envUsages")}
    };
}

