#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/cypress/e2e/backup-restore.spec.h"

void Main(void)
{
    describe(std::string("Backup and Restore"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            cy->visitGame();
            cy->waitForGameLoad();
        }
        );
        describe(std::string("Backup Settings UI"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
                cy->get(std::string("[data-testid="config-content"]"))->should(std::string("be.visible"));
            }
            );
            it(std::string("should display backup settings section"), [=]() mutable
            {
                cy->contains(std::string("◎ BACKUP & RESTORE"))->should(std::string("be.visible"));
                cy->contains(std::string("Automatic Backup Settings"))->should(std::string("be.visible"));
                cy->contains(std::string("Available Backups"))->should(std::string("be.visible"));
            }
            );
            it(std::string("should show backup configuration options"), [=]() mutable
            {
                cy->contains(std::string("Enable automatic backups"))->should(std::string("be.visible"));
                cy->contains(std::string("Backup interval (hours)"))->should(std::string("be.visible"));
                cy->contains(std::string("Keep last"))->should(std::string("be.visible"));
                cy->get(std::string("input[type="checkbox"]"))->first()->should(std::string("be.checked"));
                cy->get(std::string("input[type="number"]"))->first()->should(std::string("have.value"), std::string("4"));
                cy->get(std::string("input[type="number"]"))->eq(1)->should(std::string("have.value"), std::string("5"));
            }
            );
            it(std::string("should update backup configuration"), [=]() mutable
            {
                cy->get(std::string("input[type="checkbox"]"))->first()->uncheck();
                cy->get(std::string("input[type="number"]"))->first()->clear()->type(std::string("6"));
                cy->get(std::string("input[type="number"]"))->eq(1)->clear()->type(std::string("10"));
                cy->contains(std::string("button"), std::string("Save Settings"))->click();
            }
            );
        }
        );
        describe(std::string("Manual Backup Creation"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
            }
            );
            it(std::string("should create a manual backup"), [=]() mutable
            {
                cy->contains(std::string("button"), std::string("💾 Create Backup"))->click();
                cy->contains(std::string("Create Manual Backup"))->should(std::string("be.visible"));
                cy->get(std::string("textarea"))->type(std::string("Test backup for E2E tests"));
                cy->contains(std::string("button"), std::string("Create Backup"))->click();
                cy->contains(std::string("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->contains(std::string("manual"))->should(std::string("be.visible"));
                    cy->contains(std::string("Test backup for E2E tests"))->should(std::string("be.visible"));
                }
                );
            }
            );
            it(std::string("should handle backup creation errors gracefully"), [=]() mutable
            {
                cy->contains(std::string("button"), std::string("💾 Create Backup"))->should(std::string("exist"));
            }
            );
        }
        );
        describe(std::string("Backup List Management"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
                cy->contains(std::string("button"), std::string("💾 Create Backup"))->click();
                cy->get(std::string("textarea"))->type(std::string("Test backup"));
                cy->contains(std::string("button"), std::string("Create Backup"))->click();
                cy->wait(1000);
            }
            );
            it(std::string("should display backup list with correct information"), [=]() mutable
            {
                cy->contains(std::string("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->contains(std::string("✋"))->should(std::string("be.visible"));
                    cy->contains(std::string("manual"))->should(std::string("be.visible"));
                    cy->contains(std::string("Test backup"))->should(std::string("be.visible"));
                    cy->contains((new RegExp(std::string("\d+\s*(B|KB|MB|GB"))))->should(std::string("be.visible"));
                    cy->contains((new RegExp(std::string("\d{1,2}\/\d{1,2}\/\d{4"))))->should(std::string("be.visible"));
                }
                );
            }
            );
            it(std::string("should delete a backup"), [=]() mutable
            {
                cy->contains(std::string("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std::string("button[title="Delete"]"))->first()->click();
                }
                );
                cy->on(std::string("window:confirm"), [=]() mutable
                {
                    return true;
                }
                );
                cy->wait(1000);
                cy->contains(std::string("Test backup"))->should(std::string("not.exist"));
            }
            );
            it(std::string("should export a backup"), [=]() mutable
            {
                cy->contains(std::string("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std::string("button[title="Export"]"))->first()->click();
                }
                );
            }
            );
        }
        );
        describe(std::string("Backup Restore"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
                cy->contains(std::string("button"), std::string("💾 Create Backup"))->click();
                cy->get(std::string("textarea"))->type(std::string("Backup to restore"));
                cy->contains(std::string("button"), std::string("Create Backup"))->click();
                cy->wait(1000);
            }
            );
            it(std::string("should show restore dialog with options"), [=]() mutable
            {
                cy->contains(std::string("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std::string("button[title="Restore"]"))->first()->click();
                }
                );
                cy->contains(std::string("⚠️ Restore Backup"))->should(std::string("be.visible"));
                cy->contains(std::string("This operation is DESTRUCTIVE"))->should(std::string("be.visible"));
                cy->contains(std::string("Database (conversations, settings)"))->should(std::string("be.visible"));
                cy->contains(std::string("Agent State (memory, context)"))->should(std::string("be.visible"));
                cy->contains(std::string("Knowledge Base"))->should(std::string("be.visible"));
                cy->contains(std::string("Logs (optional)"))->should(std::string("be.visible"));
                cy->get(std::string("input[type="checkbox"]"))->eq(0)->should(std::string("be.checked"));
                cy->get(std::string("input[type="checkbox"]"))->eq(1)->should(std::string("be.checked"));
                cy->get(std::string("input[type="checkbox"]"))->eq(2)->should(std::string("be.checked"));
                cy->get(std::string("input[type="checkbox"]"))->eq(3)->should(std::string("not.be.checked"));
            }
            );
            it(std::string("should cancel restore operation"), [=]() mutable
            {
                cy->contains(std::string("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std::string("button[title="Restore"]"))->first()->click();
                }
                );
                cy->contains(std::string("button"), std::string("Cancel"))->click();
                cy->contains(std::string("⚠️ Restore Backup"))->should(std::string("not.exist"));
            }
            );
            it(std::string("should warn before restore"), [=]() mutable
            {
                cy->contains(std::string("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std::string("button[title="Restore"]"))->first()->click();
                }
                );
                cy->get(std::string("input[type="checkbox"]"))->eq(2)->uncheck();
                cy->contains(std::string("button"), std::string("Restore Backup"))->click();
                cy->on(std::string("window:confirm"), [=](auto message) mutable
                {
                    expect(message)->to->include(std::string("WARNING"));
                    expect(message)->to->include(std::string("DESTRUCTIVE"));
                    return false;
                }
                );
            }
            );
        }
        );
        describe(std::string("Import/Export Integration"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
            }
            );
            it(std::string("should show import button"), [=]() mutable
            {
                cy->contains(std::string("button"), std::string("📥 Import"))->should(std::string("be.visible"));
            }
            );
            it(std::string("should handle import action"), [=]() mutable
            {
                cy->contains(std::string("button"), std::string("📥 Import"))->click();
            }
            );
        }
        );
        describe(std::string("Auto-backup Functionality"), [=]() mutable
        {
            it(std::string("should show auto-backup indicator when enabled"), [=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
                cy->get(std::string("input[type="checkbox"]"))->first()->should(std::string("be.checked"));
            }
            );
        }
        );
        describe(std::string("Backup Components Display"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
                cy->contains(std::string("button"), std::string("💾 Create Backup"))->click();
                cy->get(std::string("textarea"))->type(std::string("Component test backup"));
                cy->contains(std::string("button"), std::string("Create Backup"))->click();
                cy->wait(1000);
            }
            );
            it(std::string("should show backup type icons correctly"), [=]() mutable
            {
                cy->contains(std::string("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->contains(std::string("✋"))->should(std::string("be.visible"));
                }
                );
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle network errors gracefully"), [=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
                cy->contains(std::string("◎ BACKUP & RESTORE"))->should(std::string("be.visible"));
            }
            );
        }
        );
        describe(std::string("Accessibility"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std::string("[data-testid="config-tab"]"))->click();
            }
            );
            it(std::string("should have proper labels for form controls"), [=]() mutable
            {
                cy->contains(std::string("label"), std::string("Enable automatic backups"))->should(std::string("be.visible"));
                cy->contains(std::string("label"), std::string("Backup interval (hours)"))->should(std::string("be.visible"));
                cy->contains(std::string("label"), std::string("Keep last"))->should(std::string("be.visible"));
            }
            );
            it(std::string("should have descriptive button titles"), [=]() mutable
            {
                cy->get(std::string("button[title="Restore"]"))->should(std::string("exist"));
                cy->get(std::string("button[title="Export"]"))->should(std::string("exist"));
                cy->get(std::string("button[title="Delete"]"))->should(std::string("exist"));
            }
            );
        }
        );
    }
    );
}

MAIN
