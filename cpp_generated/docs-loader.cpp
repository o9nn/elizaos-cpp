#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-knowledge/src/docs-loader.h"

string getKnowledgePath()
{
    auto envPath = process->env->KNOWLEDGE_PATH;
    if (envPath) {
        auto resolvedPath = path->resolve(envPath);
        if (!fs::existsSync(resolvedPath)) {
            logger->warn(std::string("Knowledge path from environment variable does not exist: ") + resolvedPath + string_empty);
            logger->warn(std::string("Please create the directory or update KNOWLEDGE_PATH environment variable"));
        }
        return resolvedPath;
    }
    auto defaultPath = path->join(process->cwd(), std::string("docs"));
    if (!fs::existsSync(defaultPath)) {
        logger->info(std::string("Default docs folder does not exist at: ") + defaultPath + string_empty);
        logger->info(std::string("To use the knowledge plugin, either:"));
        logger->info(std::string("1. Create a "docs" folder in your project root"));
        logger->info(std::string("2. Set KNOWLEDGE_PATH environment variable to your documents folder"));
    }
    return defaultPath;
};


std::shared_ptr<Promise<object>> loadDocsFromPath(std::shared_ptr<KnowledgeService> service, std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> worldId)
{
    auto docsPath = getKnowledgePath();
    if (!fs::existsSync(docsPath)) {
        logger->warn(std::string("Knowledge path does not exist: ") + docsPath + string_empty);
        return object{
            object::pair{std::string("total"), 0}, 
            object::pair{std::string("successful"), 0}, 
            object::pair{std::string("failed"), 0}
        };
    }
    logger->info(std::string("Loading documents from: ") + docsPath + string_empty);
    auto files = getAllFiles(docsPath);
    if (files->get_length() == 0) {
        logger->info(std::string("No files found in knowledge path"));
        return object{
            object::pair{std::string("total"), 0}, 
            object::pair{std::string("successful"), 0}, 
            object::pair{std::string("failed"), 0}
        };
    }
    logger->info(std::string("Found ") + files->get_length() + std::string(" files to process"));
    auto successful = 0;
    auto failed = 0;
    for (auto& filePath : files)
    {
        try
        {
            auto fileName = path->basename(filePath);
            auto fileExt = path->extname(filePath)->toLowerCase();
            if (fileName->startsWith(std::string("."))) {
                continue;
            }
            auto contentType = getContentType(fileExt);
            if (!contentType) {
                logger->debug(std::string("Skipping unsupported file type: ") + filePath + string_empty);
                continue;
            }
            auto fileBuffer = fs::readFileSync(filePath);
            auto isBinary = isBinaryContentType(contentType, fileName);
            auto content = (isBinary) ? fileBuffer->toString(std::string("base64")) : fileBuffer->toString(std::string("utf-8"));
            auto knowledgeOptions = object{
                object::pair{std::string("clientDocumentId"), as<std::shared_ptr<UUID>>(string_empty)}, 
                object::pair{std::string("contentType"), std::string("contentType")}, 
                object::pair{std::string("originalFilename"), fileName}, 
                object::pair{std::string("worldId"), OR((worldId), (agentId))}, 
                object::pair{std::string("content"), std::string("content")}, 
                object::pair{std::string("roomId"), agentId}, 
                object::pair{std::string("entityId"), agentId}
            };
            logger->debug(std::string("Processing document: ") + fileName + string_empty);
            auto result = std::async([=]() { service->addKnowledge(knowledgeOptions); });
            logger->info(std::string("✅ "") + fileName + std::string("": ") + result->fragmentCount + std::string(" fragments created"));
            successful++;
        }
        catch (const any& error)
        {
            logger->error(std::string("Failed to process file ") + filePath + std::string(":"), error);
            failed++;
        }
    }
    logger->info(std::string("Document loading complete: ") + successful + std::string(" successful, ") + failed + std::string(" failed out of ") + files->get_length() + std::string(" total"));
    return object{
        object::pair{std::string("total"), files->get_length()}, 
        object::pair{std::string("successful"), std::string("successful")}, 
        object::pair{std::string("failed"), std::string("failed")}
    };
};


array<string> getAllFiles(string dirPath, array<string> files)
{
    try
    {
        auto entries = fs::readdirSync(dirPath, object{
            object::pair{std::string("withFileTypes"), true}
        });
        for (auto& entry : entries)
        {
            auto fullPath = path->join(dirPath, entry->name);
            if (entry->isDirectory()) {
                if (!(array<string>{ std::string("node_modules"), std::string(".git"), std::string(".vscode"), std::string("dist"), std::string("build") })->includes(entry->name)) {
                    getAllFiles(fullPath, files);
                }
            } else if (entry->isFile()) {
                files->push(fullPath);
            }
        }
    }
    catch (const any& error)
    {
        logger->error(std::string("Error reading directory ") + dirPath + std::string(":"), error);
    }
    return files;
};


any getContentType(string extension)
{
    auto contentTypes = object{
        object::pair{std::string(".txt"), std::string("text/plain")}, 
        object::pair{std::string(".md"), std::string("text/markdown")}, 
        object::pair{std::string(".markdown"), std::string("text/markdown")}, 
        object::pair{std::string(".tson"), std::string("text/plain")}, 
        object::pair{std::string(".xml"), std::string("application/xml")}, 
        object::pair{std::string(".csv"), std::string("text/csv")}, 
        object::pair{std::string(".tsv"), std::string("text/tab-separated-values")}, 
        object::pair{std::string(".log"), std::string("text/plain")}, 
        object::pair{std::string(".html"), std::string("text/html")}, 
        object::pair{std::string(".htm"), std::string("text/html")}, 
        object::pair{std::string(".css"), std::string("text/css")}, 
        object::pair{std::string(".scss"), std::string("text/x-scss")}, 
        object::pair{std::string(".sass"), std::string("text/x-sass")}, 
        object::pair{std::string(".less"), std::string("text/x-less")}, 
        object::pair{std::string(".js"), std::string("text/javascript")}, 
        object::pair{std::string(".jsx"), std::string("text/javascript")}, 
        object::pair{std::string(".ts"), std::string("text/typescript")}, 
        object::pair{std::string(".tsx"), std::string("text/typescript")}, 
        object::pair{std::string(".mjs"), std::string("text/javascript")}, 
        object::pair{std::string(".cjs"), std::string("text/javascript")}, 
        object::pair{std::string(".vue"), std::string("text/x-vue")}, 
        object::pair{std::string(".svelte"), std::string("text/x-svelte")}, 
        object::pair{std::string(".astro"), std::string("text/x-astro")}, 
        object::pair{std::string(".py"), std::string("text/x-python")}, 
        object::pair{std::string(".pyw"), std::string("text/x-python")}, 
        object::pair{std::string(".pyi"), std::string("text/x-python")}, 
        object::pair{std::string(".java"), std::string("text/x-java")}, 
        object::pair{std::string(".kt"), std::string("text/x-kotlin")}, 
        object::pair{std::string(".kts"), std::string("text/x-kotlin")}, 
        object::pair{std::string(".scala"), std::string("text/x-scala")}, 
        object::pair{std::string(".c"), std::string("text/x-c")}, 
        object::pair{std::string(".cpp"), std::string("text/x-c++")}, 
        object::pair{std::string(".cc"), std::string("text/x-c++")}, 
        object::pair{std::string(".cxx"), std::string("text/x-c++")}, 
        object::pair{std::string(".h"), std::string("text/x-c")}, 
        object::pair{std::string(".hpp"), std::string("text/x-c++")}, 
        object::pair{std::string(".cs"), std::string("text/x-csharp")}, 
        object::pair{std::string(".php"), std::string("text/x-php")}, 
        object::pair{std::string(".rb"), std::string("text/x-ruby")}, 
        object::pair{std::string(".go"), std::string("text/x-go")}, 
        object::pair{std::string(".rs"), std::string("text/x-rust")}, 
        object::pair{std::string(".swift"), std::string("text/x-swift")}, 
        object::pair{std::string(".r"), std::string("text/x-r")}, 
        object::pair{std::string(".R"), std::string("text/x-r")}, 
        object::pair{std::string(".m"), std::string("text/x-objectivec")}, 
        object::pair{std::string(".mm"), std::string("text/x-objectivec")}, 
        object::pair{std::string(".clj"), std::string("text/x-clojure")}, 
        object::pair{std::string(".cljs"), std::string("text/x-clojure")}, 
        object::pair{std::string(".ex"), std::string("text/x-elixir")}, 
        object::pair{std::string(".exs"), std::string("text/x-elixir")}, 
        object::pair{std::string(".lua"), std::string("text/x-lua")}, 
        object::pair{std::string(".pl"), std::string("text/x-perl")}, 
        object::pair{std::string(".pm"), std::string("text/x-perl")}, 
        object::pair{std::string(".dart"), std::string("text/x-dart")}, 
        object::pair{std::string(".hs"), std::string("text/x-haskell")}, 
        object::pair{std::string(".elm"), std::string("text/x-elm")}, 
        object::pair{std::string(".ml"), std::string("text/x-ocaml")}, 
        object::pair{std::string(".fs"), std::string("text/x-fsharp")}, 
        object::pair{std::string(".fsx"), std::string("text/x-fsharp")}, 
        object::pair{std::string(".vb"), std::string("text/x-vb")}, 
        object::pair{std::string(".pas"), std::string("text/x-pascal")}, 
        object::pair{std::string(".d"), std::string("text/x-d")}, 
        object::pair{std::string(".nim"), std::string("text/x-nim")}, 
        object::pair{std::string(".zig"), std::string("text/x-zig")}, 
        object::pair{std::string(".jl"), std::string("text/x-julia")}, 
        object::pair{std::string(".tcl"), std::string("text/x-tcl")}, 
        object::pair{std::string(".awk"), std::string("text/x-awk")}, 
        object::pair{std::string(".sed"), std::string("text/x-sed")}, 
        object::pair{std::string(".sh"), std::string("text/x-sh")}, 
        object::pair{std::string(".bash"), std::string("text/x-sh")}, 
        object::pair{std::string(".zsh"), std::string("text/x-sh")}, 
        object::pair{std::string(".fish"), std::string("text/x-fish")}, 
        object::pair{std::string(".ps1"), std::string("text/x-powershell")}, 
        object::pair{std::string(".bat"), std::string("text/x-batch")}, 
        object::pair{std::string(".cmd"), std::string("text/x-batch")}, 
        object::pair{std::string(".json"), std::string("application/json")}, 
        object::pair{std::string(".yaml"), std::string("text/x-yaml")}, 
        object::pair{std::string(".yml"), std::string("text/x-yaml")}, 
        object::pair{std::string(".toml"), std::string("text/x-toml")}, 
        object::pair{std::string(".ini"), std::string("text/x-ini")}, 
        object::pair{std::string(".cfg"), std::string("text/x-ini")}, 
        object::pair{std::string(".conf"), std::string("text/x-ini")}, 
        object::pair{std::string(".env"), std::string("text/plain")}, 
        object::pair{std::string(".gitignore"), std::string("text/plain")}, 
        object::pair{std::string(".dockerignore"), std::string("text/plain")}, 
        object::pair{std::string(".editorconfig"), std::string("text/plain")}, 
        object::pair{std::string(".properties"), std::string("text/x-properties")}, 
        object::pair{std::string(".sql"), std::string("text/x-sql")}, 
        object::pair{std::string(".pdf"), std::string("application/pdf")}, 
        object::pair{std::string(".doc"), std::string("application/msword")}, 
        object::pair{std::string(".docx"), std::string("application/vnd.openxmlformats-officedocument.wordprocessingml.document")}
    };
    return OR((const_(contentTypes)[extension]), (nullptr));
};


