#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-knowledge/src/docs-loader.h"

string getKnowledgePath()
{
    auto envPath = process->env->KNOWLEDGE_PATH;
    if (envPath) {
        auto resolvedPath = path->resolve(envPath);
        if (!fs::existsSync(resolvedPath)) {
            logger->warn(std:("Knowledge path from environment variable does not exist: ") + resolvedPath + string_empty);
            logger->warn(std:("Please create the directory or update KNOWLEDGE_PATH environment variable"));
        }
        return resolvedPath;
    }
    auto defaultPath = path->join(process->cwd(), std:("docs"));
    if (!fs::existsSync(defaultPath)) {
        logger->info(std:("Default docs folder does not exist at: ") + defaultPath + string_empty);
        logger->info(std:("To use the knowledge plugin, either:"));
        logger->info(std:("1. Create a "docs" folder in your project root"));
        logger->info(std:("2. Set KNOWLEDGE_PATH environment variable to your documents folder"));
    }
    return defaultPath;
};


std::shared_ptr<Promise<object>> loadDocsFromPath(std::shared_ptr<KnowledgeService> service, std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId)
{
    auto docsPath = getKnowledgePath();
    if (!fs::existsSync(docsPath)) {
        logger->warn(std:("Knowledge path does not exist: ") + docsPath + string_empty);
        return object{
            object::pair{std:("total"), 0}, 
            object::pair{std:("successful"), 0}, 
            object::pair{std:("failed"), 0}
        };
    }
    logger->info(std:("Loading documents from: ") + docsPath + string_empty);
    auto files = getAllFiles(docsPath);
    if (files->get_length() == 0) {
        logger->info(std:("No files found in knowledge path"));
        return object{
            object::pair{std:("total"), 0}, 
            object::pair{std:("successful"), 0}, 
            object::pair{std:("failed"), 0}
        };
    }
    logger->info(std:("Found ") + files->get_length() + std:(" files to process"));
    auto successful = 0;
    auto failed = 0;
    for (auto& filePath : files)
    {
        try
        {
            auto fileName = path->basename(filePath);
            auto fileExt = path->extname(filePath)->toLowerCase();
            if (fileName->startsWith(std:("."))) {
                continue;
            }
            auto contentType = getContentType(fileExt);
            if (!contentType) {
                logger->debug(std:("Skipping unsupported file type: ") + filePath + string_empty);
                continue;
            }
            auto fileBuffer = fs::readFileSync(filePath);
            auto isBinary = isBinaryContentType(contentType, fileName);
            auto content = (isBinary) ? fileBuffer->toString(std:("base64")) : fileBuffer->toString(std:("utf-8"));
            auto knowledgeOptions = object{
                object::pair{std:("clientDocumentId"), as<std::shared_ptr<UUID>>(string_empty)}, 
                object::pair{std:("contentType"), std:("contentType")}, 
                object::pair{std:("originalFilename"), fileName}, 
                object::pair{std:("worldId"), OR((worldId), (agentId))}, 
                object::pair{std:("content"), std:("content")}, 
                object::pair{std:("roomId"), agentId}, 
                object::pair{std:("entityId"), agentId}
            };
            logger->debug(std:("Processing document: ") + fileName + string_empty);
            auto result = std::async([=]() { service->addKnowledge(knowledgeOptions); });
            logger->info(std:("✅ "") + fileName + std:("": ") + result->fragmentCount + std:(" fragments created"));
            successful++;
        }
        catch (const any& error)
        {
            logger->error(std:("Failed to process file ") + filePath + std:(":"), error);
            failed++;
        }
    }
    logger->info(std:("Document loading complete: ") + successful + std:(" successful, ") + failed + std:(" failed out of ") + files->get_length() + std:(" total"));
    return object{
        object::pair{std:("total"), files->get_length()}, 
        object::pair{std:("successful"), std:("successful")}, 
        object::pair{std:("failed"), std:("failed")}
    };
};


array<string> getAllFiles(string dirPath, array<string> files)
{
    try
    {
        auto entries = fs::readdirSync(dirPath, object{
            object::pair{std:("withFileTypes"), true}
        });
        for (auto& entry : entries)
        {
            auto fullPath = path->join(dirPath, entry->name);
            if (entry->isDirectory()) {
                if (!(array<string>{ std:("node_modules"), std:(".git"), std:(".vscode"), std:("dist"), std:("build") })->includes(entry->name)) {
                    getAllFiles(fullPath, files);
                }
            } else if (entry->isFile()) {
                files->push(fullPath);
            }
        }
    }
    catch (const any& error)
    {
        logger->error(std:("Error reading directory ") + dirPath + std:(":"), error);
    }
    return files;
};


any getContentType(string extension)
{
    auto contentTypes = object{
        object::pair{std:(".txt"), std:("text/plain")}, 
        object::pair{std:(".md"), std:("text/markdown")}, 
        object::pair{std:(".markdown"), std:("text/markdown")}, 
        object::pair{std:(".tson"), std:("text/plain")}, 
        object::pair{std:(".xml"), std:("application/xml")}, 
        object::pair{std:(".csv"), std:("text/csv")}, 
        object::pair{std:(".tsv"), std:("text/tab-separated-values")}, 
        object::pair{std:(".log"), std:("text/plain")}, 
        object::pair{std:(".html"), std:("text/html")}, 
        object::pair{std:(".htm"), std:("text/html")}, 
        object::pair{std:(".css"), std:("text/css")}, 
        object::pair{std:(".scss"), std:("text/x-scss")}, 
        object::pair{std:(".sass"), std:("text/x-sass")}, 
        object::pair{std:(".less"), std:("text/x-less")}, 
        object::pair{std:(".js"), std:("text/javascript")}, 
        object::pair{std:(".jsx"), std:("text/javascript")}, 
        object::pair{std:(".ts"), std:("text/typescript")}, 
        object::pair{std:(".tsx"), std:("text/typescript")}, 
        object::pair{std:(".mjs"), std:("text/javascript")}, 
        object::pair{std:(".cjs"), std:("text/javascript")}, 
        object::pair{std:(".vue"), std:("text/x-vue")}, 
        object::pair{std:(".svelte"), std:("text/x-svelte")}, 
        object::pair{std:(".astro"), std:("text/x-astro")}, 
        object::pair{std:(".py"), std:("text/x-python")}, 
        object::pair{std:(".pyw"), std:("text/x-python")}, 
        object::pair{std:(".pyi"), std:("text/x-python")}, 
        object::pair{std:(".java"), std:("text/x-java")}, 
        object::pair{std:(".kt"), std:("text/x-kotlin")}, 
        object::pair{std:(".kts"), std:("text/x-kotlin")}, 
        object::pair{std:(".scala"), std:("text/x-scala")}, 
        object::pair{std:(".c"), std:("text/x-c")}, 
        object::pair{std:(".cpp"), std:("text/x-c++")}, 
        object::pair{std:(".cc"), std:("text/x-c++")}, 
        object::pair{std:(".cxx"), std:("text/x-c++")}, 
        object::pair{std:(".h"), std:("text/x-c")}, 
        object::pair{std:(".hpp"), std:("text/x-c++")}, 
        object::pair{std:(".cs"), std:("text/x-csharp")}, 
        object::pair{std:(".php"), std:("text/x-php")}, 
        object::pair{std:(".rb"), std:("text/x-ruby")}, 
        object::pair{std:(".go"), std:("text/x-go")}, 
        object::pair{std:(".rs"), std:("text/x-rust")}, 
        object::pair{std:(".swift"), std:("text/x-swift")}, 
        object::pair{std:(".r"), std:("text/x-r")}, 
        object::pair{std:(".R"), std:("text/x-r")}, 
        object::pair{std:(".m"), std:("text/x-objectivec")}, 
        object::pair{std:(".mm"), std:("text/x-objectivec")}, 
        object::pair{std:(".clj"), std:("text/x-clojure")}, 
        object::pair{std:(".cljs"), std:("text/x-clojure")}, 
        object::pair{std:(".ex"), std:("text/x-elixir")}, 
        object::pair{std:(".exs"), std:("text/x-elixir")}, 
        object::pair{std:(".lua"), std:("text/x-lua")}, 
        object::pair{std:(".pl"), std:("text/x-perl")}, 
        object::pair{std:(".pm"), std:("text/x-perl")}, 
        object::pair{std:(".dart"), std:("text/x-dart")}, 
        object::pair{std:(".hs"), std:("text/x-haskell")}, 
        object::pair{std:(".elm"), std:("text/x-elm")}, 
        object::pair{std:(".ml"), std:("text/x-ocaml")}, 
        object::pair{std:(".fs"), std:("text/x-fsharp")}, 
        object::pair{std:(".fsx"), std:("text/x-fsharp")}, 
        object::pair{std:(".vb"), std:("text/x-vb")}, 
        object::pair{std:(".pas"), std:("text/x-pascal")}, 
        object::pair{std:(".d"), std:("text/x-d")}, 
        object::pair{std:(".nim"), std:("text/x-nim")}, 
        object::pair{std:(".zig"), std:("text/x-zig")}, 
        object::pair{std:(".jl"), std:("text/x-julia")}, 
        object::pair{std:(".tcl"), std:("text/x-tcl")}, 
        object::pair{std:(".awk"), std:("text/x-awk")}, 
        object::pair{std:(".sed"), std:("text/x-sed")}, 
        object::pair{std:(".sh"), std:("text/x-sh")}, 
        object::pair{std:(".bash"), std:("text/x-sh")}, 
        object::pair{std:(".zsh"), std:("text/x-sh")}, 
        object::pair{std:(".fish"), std:("text/x-fish")}, 
        object::pair{std:(".ps1"), std:("text/x-powershell")}, 
        object::pair{std:(".bat"), std:("text/x-batch")}, 
        object::pair{std:(".cmd"), std:("text/x-batch")}, 
        object::pair{std:(".json"), std:("application/json")}, 
        object::pair{std:(".yaml"), std:("text/x-yaml")}, 
        object::pair{std:(".yml"), std:("text/x-yaml")}, 
        object::pair{std:(".toml"), std:("text/x-toml")}, 
        object::pair{std:(".ini"), std:("text/x-ini")}, 
        object::pair{std:(".cfg"), std:("text/x-ini")}, 
        object::pair{std:(".conf"), std:("text/x-ini")}, 
        object::pair{std:(".env"), std:("text/plain")}, 
        object::pair{std:(".gitignore"), std:("text/plain")}, 
        object::pair{std:(".dockerignore"), std:("text/plain")}, 
        object::pair{std:(".editorconfig"), std:("text/plain")}, 
        object::pair{std:(".properties"), std:("text/x-properties")}, 
        object::pair{std:(".sql"), std:("text/x-sql")}, 
        object::pair{std:(".pdf"), std:("application/pdf")}, 
        object::pair{std:(".doc"), std:("application/msword")}, 
        object::pair{std:(".docx"), std:("application/vnd.openxmlformats-officedocument.wordprocessingml.document")}
    };
    return OR((const_(contentTypes)[extension]), (nullptr));
};


