#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/autodoc/src/AIService/utils/DocumentOrganizer.h"

std::shared_ptr<OrganizedDocs> DocumentOrganizer::organizeDocumentation(array<std::shared_ptr<ASTQueueItem>> docs)
{
    return docs->reduce([=](auto acc, auto doc) mutable
    {
        static switch_type __switch675_1360 = {
            { any(std::string("ClassDeclaration")), 1 },
            { any(std::string("MethodDefinition")), 2 },
            { any(std::string("TSMethodSignature")), 3 },
            { any(std::string("TSInterfaceDeclaration")), 4 },
            { any(std::string("TSTypeAliasDeclaration")), 5 },
            { any(std::string("FunctionDeclaration")), 6 },
            { any(std::string("VariableDeclaration")), 7 }
        };
        switch (__switch675_1360[doc->nodeType])
        {
        case 1:
            acc->classes->push(doc);
            break;
        case 2:
        case 3:
            acc->methods->push(doc);
            break;
        case 4:
            acc->interfaces->push(doc);
            break;
        case 5:
            acc->types->push(doc);
            break;
        case 6:
            acc->functions->push(doc);
            break;
        case 7:
            acc->variables->push(doc);
            break;
        }
        return acc;
    }
    , object{
        object::pair{std::string("classes"), array<any>()}, 
        object::pair{std::string("methods"), array<any>()}, 
        object::pair{std::string("interfaces"), array<any>()}, 
        object::pair{std::string("types"), array<any>()}, 
        object::pair{std::string("functions"), array<any>()}, 
        object::pair{std::string("variables"), array<any>()}
    });
}

array<std::shared_ptr<FileDocsGroup>> DocumentOrganizer::groupDocsByFile(std::shared_ptr<OrganizedDocs> docs)
{
    shared filePaths = std::make_shared<Set<string>>();
    (array<std::shared_ptr<ASTQueueItem>>{ docs->classes, docs->methods, docs->interfaces, docs->types, docs->functions, docs->variables })->forEach([=](auto item) mutable
    {
        return filePaths->add(item->filePath);
    }
    );
    return Array->from(filePaths)->map([=](auto filePath) mutable
    {
        return object{
            object::pair{std::string("filePath"), std::string("filePath")}, 
            object::pair{std::string("classes"), docs->classes->filter([=](auto c) mutable
            {
                return c->filePath == filePath;
            }
            )}, 
            object::pair{std::string("methods"), docs->methods->filter([=](auto m) mutable
            {
                return m->filePath == filePath;
            }
            )}, 
            object::pair{std::string("interfaces"), docs->interfaces->filter([=](auto i) mutable
            {
                return i->filePath == filePath;
            }
            )}, 
            object::pair{std::string("types"), docs->types->filter([=](auto t) mutable
            {
                return t->filePath == filePath;
            }
            )}, 
            object::pair{std::string("functions"), docs->functions->filter([=](auto f) mutable
            {
                return f->filePath == filePath;
            }
            )}, 
            object::pair{std::string("variables"), docs->variables->filter([=](auto v) mutable
            {
                return v->filePath == filePath;
            }
            )}
        };
    }
    );
}

