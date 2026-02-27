#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-todo/cypress/support/commands.h"

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std:("uploadFile"), [=](auto selector, auto fileName, auto fileContent, auto mimeType = std:("text/plain")) mutable
    {
        cy->get(selector)->selectFile(object{
            object::pair{std:("contents"), Cypress->Buffer->from(fileContent)}, 
            object::pair{std:("fileName"), std:("fileName")}, 
            object::pair{std:("mimeType"), std:("mimeType")}
        }, object{
            object::pair{std:("force"), true}
        });
    }
    );
    Cypress->Commands->add(std:("waitForApi"), [=](auto alias, auto timeout = 10000) mutable
    {
        cy->wait(alias, object{
            object::pair{std:("timeout"), std:("timeout")}
        });
    }
    );
    Cypress->Commands->add(std:("visitKnowledgePanel"), [=]() mutable
    {
        cy->visit(std:("/plugins/knowledge/display"));
        cy->get(std:("[data-testid="knowledge-panel"]"), object{
            object::pair{std:("timeout"), 10000}
        })->should(std:("be.visible"));
    }
    );
    Cypress->Commands->add(std:("uploadKnowledgeFile"), [=](auto fileName, auto content, auto mimeType = std:("text/plain")) mutable
    {
        auto blob = std::make_shared<Blob>(array<string>{ content }, object{
            object::pair{std:("type"), mimeType}
        });
        auto file = std::make_shared<File>(array<std::shared_ptr<Blob>>{ blob }, fileName, object{
            object::pair{std:("type"), mimeType}
        });
        cy->get(std:("[data-testid="file-upload-input"]"))->selectFile(object{
            object::pair{std:("contents"), Cypress->Buffer->from(content)}, 
            object::pair{std:("fileName"), std:("fileName")}, 
            object::pair{std:("mimeType"), std:("mimeType")}
        }, object{
            object::pair{std:("force"), true}
        });
        cy->get(std:("[data-testid="upload-success"]"), object{
            object::pair{std:("timeout"), 10000}
        })->should(std:("be.visible"));
    }
    );
    Cypress->Commands->add(std:("searchKnowledge"), [=](auto query) mutable
    {
        cy->get(std:("[data-testid="knowledge-search-input"]"))->clear()->type(query);
        cy->get(std:("[data-testid="knowledge-search-button"]"))->click();
        cy->get(std:("[data-testid="search-results"]"), object{
            object::pair{std:("timeout"), 5000}
        })->should(std:("be.visible"));
    }
    );
    Cypress->Commands->add(std:("deleteDocument"), [=](auto title) mutable
    {
        cy->contains(std:("[data-testid="document-item"]"), title)->find(std:("[data-testid="delete-button"]"))->click();
        cy->get(std:("[data-testid="confirm-delete"]"))->click();
        cy->contains(std:("[data-testid="document-item"]"), title)->should(std:("not.exist"));
    }
    );
}

MAIN
