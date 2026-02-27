#include "commands.hpp"
#include <string>

namespace global {
    namespace Cypress {
    }
}

void Main(void)
{
    Cypress->Commands->add(std::string("uploadFile"), [=](auto selector, auto fileName, auto fileContent, auto mimeType = std::string("text/plain")) mutable
    {
        cy->get(selector)->selectFile(object{
            object::pair{std::string("contents"), Cypress->Buffer->from(fileContent)}, 
            object::pair{std::string("fileName"), std::string("fileName")}, 
            object::pair{std::string("mimeType"), std::string("mimeType")}
        }, object{
            object::pair{std::string("force"), true}
        });
    }
    );
    Cypress->Commands->add(std::string("waitForApi"), [=](auto alias, auto timeout = 10000) mutable
    {
        cy->wait(alias, object{
            object::pair{std::string("timeout"), std::string("timeout")}
        });
    }
    );
    Cypress->Commands->add(std::string("visitKnowledgePanel"), [=]() mutable
    {
        cy->visit(std::string("/plugins/knowledge/display"));
        cy->get(std::string("[data-testid="knowledge-panel"]"), object{
            object::pair{std::string("timeout"), 10000}
        })->should(std::string("be.visible"));
    }
    );
    Cypress->Commands->add(std::string("uploadKnowledgeFile"), [=](auto fileName, auto content, auto mimeType = std::string("text/plain")) mutable
    {
        auto blob = std::make_shared<Blob>(array<string>{ content }, object{
            object::pair{std::string("type"), mimeType}
        });
        auto file = std::make_shared<File>(array<std::shared_ptr<Blob>>{ blob }, fileName, object{
            object::pair{std::string("type"), mimeType}
        });
        cy->get(std::string("[data-testid="file-upload-input"]"))->selectFile(object{
            object::pair{std::string("contents"), Cypress->Buffer->from(content)}, 
            object::pair{std::string("fileName"), std::string("fileName")}, 
            object::pair{std::string("mimeType"), std::string("mimeType")}
        }, object{
            object::pair{std::string("force"), true}
        });
        cy->get(std::string("[data-testid="upload-success"]"), object{
            object::pair{std::string("timeout"), 10000}
        })->should(std::string("be.visible"));
    }
    );
    Cypress->Commands->add(std::string("searchKnowledge"), [=](auto query) mutable
    {
        cy->get(std::string("[data-testid="knowledge-search-input"]"))->clear()->type(query);
        cy->get(std::string("[data-testid="knowledge-search-button"]"))->click();
        cy->get(std::string("[data-testid="search-results"]"), object{
            object::pair{std::string("timeout"), 5000}
        })->should(std::string("be.visible"));
    }
    );
    Cypress->Commands->add(std::string("deleteDocument"), [=](auto title) mutable
    {
        cy->contains(std::string("[data-testid="document-item"]"), title)->find(std::string("[data-testid="delete-button"]"))->click();
        cy->get(std::string("[data-testid="confirm-delete"]"))->click();
        cy->contains(std::string("[data-testid="document-item"]"), title)->should(std::string("not.exist"));
    }
    );
}

MAIN
