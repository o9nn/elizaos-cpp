#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/autodoc/src/AIService/utils/DocumentOrganizer.h"

std::shared_ptr<OrganizedDocs> DocumentOrganizer::organizeDocumentation(array<std::shared_ptr<ASTQueueItem>> docs)
{
    return docs->reduce([=](auto acc, auto doc) mutable
    {
        static switch_type __switch675_1360 = {
            { any(std:("ClassDeclaration")), 1 },
            { any(std:("MethodDefinition")), 2 },
            { any(std:("TSMethodSignature")), 3 },
            { any(std:("TSInterfaceDeclaration")), 4 },
            { any(std:("TSTypeAliasDeclaration")), 5 },
            { any(std:("FunctionDeclaration")), 6 },
            { any(std:("VariableDeclaration")), 7 }
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
        object::pair{std:("classes"), array<any>()}, 
        object::pair{std:("methods"), array<any>()}, 
        object::pair{std:("interfaces"), array<any>()}, 
        object::pair{std:("types"), array<any>()}, 
        object::pair{std:("functions"), array<any>()}, 
        object::pair{std:("variables"), array<any>()}
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
            object::pair{std:("filePath"), std:("filePath")}, 
            object::pair{std:("classes"), docs->classes->filter([=](auto c) mutable
            {
                return c->filePath == filePath;
            }
            )}, 
            object::pair{std:("methods"), docs->methods->filter([=](auto m) mutable
            {
                return m->filePath == filePath;
            }
            )}, 
            object::pair{std:("interfaces"), docs->interfaces->filter([=](auto i) mutable
            {
                return i->filePath == filePath;
            }
            )}, 
            object::pair{std:("types"), docs->types->filter([=](auto t) mutable
            {
                return t->filePath == filePath;
            }
            )}, 
            object::pair{std:("functions"), docs->functions->filter([=](auto f) mutable
            {
                return f->filePath == filePath;
            }
            )}, 
            object::pair{std:("variables"), docs->variables->filter([=](auto v) mutable
            {
                return v->filePath == filePath;
            }
            )}
        };
    }
    );
}

