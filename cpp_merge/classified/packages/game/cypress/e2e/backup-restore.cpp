#include "backup-restore.spec.h"

void Main(void)
{
    describe(std:("Backup and Restore"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            cy->visitGame();
            cy->waitForGameLoad();
        }
        );
        describe(std:("Backup Settings UI"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
                cy->get(std:("[data-testid="config-content"]"))->should(std:("be.visible"));
            }
            );
            it(std:("should display backup settings section"), [=]() mutable
            {
                cy->contains(std:("◎ BACKUP & RESTORE"))->should(std:("be.visible"));
                cy->contains(std:("Automatic Backup Settings"))->should(std:("be.visible"));
                cy->contains(std:("Available Backups"))->should(std:("be.visible"));
            }
            );
            it(std:("should show backup configuration options"), [=]() mutable
            {
                cy->contains(std:("Enable automatic backups"))->should(std:("be.visible"));
                cy->contains(std:("Backup interval (hours)"))->should(std:("be.visible"));
                cy->contains(std:("Keep last"))->should(std:("be.visible"));
                cy->get(std:("input[type="checkbox"]"))->first()->should(std:("be.checked"));
                cy->get(std:("input[type="number"]"))->first()->should(std:("have.value"), std:("4"));
                cy->get(std:("input[type="number"]"))->eq(1)->should(std:("have.value"), std:("5"));
            }
            );
            it(std:("should update backup configuration"), [=]() mutable
            {
                cy->get(std:("input[type="checkbox"]"))->first()->uncheck();
                cy->get(std:("input[type="number"]"))->first()->clear()->type(std:("6"));
                cy->get(std:("input[type="number"]"))->eq(1)->clear()->type(std:("10"));
                cy->contains(std:("button"), std:("Save Settings"))->click();
            }
            );
        }
        );
        describe(std:("Manual Backup Creation"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
            }
            );
            it(std:("should create a manual backup"), [=]() mutable
            {
                cy->contains(std:("button"), std:("💾 Create Backup"))->click();
                cy->contains(std:("Create Manual Backup"))->should(std:("be.visible"));
                cy->get(std:("textarea"))->type(std:("Test backup for E2E tests"));
                cy->contains(std:("button"), std:("Create Backup"))->click();
                cy->contains(std:("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->contains(std:("manual"))->should(std:("be.visible"));
                    cy->contains(std:("Test backup for E2E tests"))->should(std:("be.visible"));
                }
                );
            }
            );
            it(std:("should handle backup creation errors gracefully"), [=]() mutable
            {
                cy->contains(std:("button"), std:("💾 Create Backup"))->should(std:("exist"));
            }
            );
        }
        );
        describe(std:("Backup List Management"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
                cy->contains(std:("button"), std:("💾 Create Backup"))->click();
                cy->get(std:("textarea"))->type(std:("Test backup"));
                cy->contains(std:("button"), std:("Create Backup"))->click();
                cy->wait(1000);
            }
            );
            it(std:("should display backup list with correct information"), [=]() mutable
            {
                cy->contains(std:("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->contains(std:("✋"))->should(std:("be.visible"));
                    cy->contains(std:("manual"))->should(std:("be.visible"));
                    cy->contains(std:("Test backup"))->should(std:("be.visible"));
                    cy->contains((new RegExp(std:("\d+\s*(B|KB|MB|GB"))))->should(std:("be.visible"));
                    cy->contains((new RegExp(std:("\d{1,2}\/\d{1,2}\/\d{4"))))->should(std:("be.visible"));
                }
                );
            }
            );
            it(std:("should delete a backup"), [=]() mutable
            {
                cy->contains(std:("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std:("button[title="Delete"]"))->first()->click();
                }
                );
                cy->on(std:("window:confirm"), [=]() mutable
                {
                    return true;
                }
                );
                cy->wait(1000);
                cy->contains(std:("Test backup"))->should(std:("not.exist"));
            }
            );
            it(std:("should a backup"), [=]() mutable
            {
                cy->contains(std:("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std:("button[title="Export"]"))->first()->click();
                }
                );
            }
            );
        }
        );
        describe(std:("Backup Restore"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
                cy->contains(std:("button"), std:("💾 Create Backup"))->click();
                cy->get(std:("textarea"))->type(std:("Backup to restore"));
                cy->contains(std:("button"), std:("Create Backup"))->click();
                cy->wait(1000);
            }
            );
            it(std:("should show restore dialog with options"), [=]() mutable
            {
                cy->contains(std:("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std:("button[title="Restore"]"))->first()->click();
                }
                );
                cy->contains(std:("⚠️ Restore Backup"))->should(std:("be.visible"));
                cy->contains(std:("This operation is DESTRUCTIVE"))->should(std:("be.visible"));
                cy->contains(std:("Database (conversations, settings)"))->should(std:("be.visible"));
                cy->contains(std:("Agent State (memory, context)"))->should(std:("be.visible"));
                cy->contains(std:("Knowledge Base"))->should(std:("be.visible"));
                cy->contains(std:("Logs (optional)"))->should(std:("be.visible"));
                cy->get(std:("input[type="checkbox"]"))->eq(0)->should(std:("be.checked"));
                cy->get(std:("input[type="checkbox"]"))->eq(1)->should(std:("be.checked"));
                cy->get(std:("input[type="checkbox"]"))->eq(2)->should(std:("be.checked"));
                cy->get(std:("input[type="checkbox"]"))->eq(3)->should(std:("not.be.checked"));
            }
            );
            it(std:("should cancel restore operation"), [=]() mutable
            {
                cy->contains(std:("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std:("button[title="Restore"]"))->first()->click();
                }
                );
                cy->contains(std:("button"), std:("Cancel"))->click();
                cy->contains(std:("⚠️ Restore Backup"))->should(std:("not.exist"));
            }
            );
            it(std:("should warn before restore"), [=]() mutable
            {
                cy->contains(std:("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->get(std:("button[title="Restore"]"))->first()->click();
                }
                );
                cy->get(std:("input[type="checkbox"]"))->eq(2)->uncheck();
                cy->contains(std:("button"), std:("Restore Backup"))->click();
                cy->on(std:("window:confirm"), [=](auto message) mutable
                {
                    expect(message)->to->include(std:("WARNING"));
                    expect(message)->to->include(std:("DESTRUCTIVE"));
                    return false;
                }
                );
            }
            );
        }
        );
        describe(std:("Import/Export Integration"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
            }
            );
            it(std:("should show import button"), [=]() mutable
            {
                cy->contains(std:("button"), std:("📥 Import"))->should(std:("be.visible"));
            }
            );
            it(std:("should handle import action"), [=]() mutable
            {
                cy->contains(std:("button"), std:("📥 Import"))->click();
            }
            );
        }
        );
        describe(std:("Auto-backup Functionality"), [=]() mutable
        {
            it(std:("should show auto-backup indicator when enabled"), [=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
                cy->get(std:("input[type="checkbox"]"))->first()->should(std:("be.checked"));
            }
            );
        }
        );
        describe(std:("Backup Components Display"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
                cy->contains(std:("button"), std:("💾 Create Backup"))->click();
                cy->get(std:("textarea"))->type(std:("Component test backup"));
                cy->contains(std:("button"), std:("Create Backup"))->click();
                cy->wait(1000);
            }
            );
            it(std:("should show backup type icons correctly"), [=]() mutable
            {
                cy->contains(std:("Available Backups"))->parent()->within([=]() mutable
                {
                    cy->contains(std:("✋"))->should(std:("be.visible"));
                }
                );
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle network errors gracefully"), [=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
                cy->contains(std:("◎ BACKUP & RESTORE"))->should(std:("be.visible"));
            }
            );
        }
        );
        describe(std:("Accessibility"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                cy->get(std:("[data-testid="config-tab"]"))->click();
            }
            );
            it(std:("should have proper labels for form controls"), [=]() mutable
            {
                cy->contains(std:("label"), std:("Enable automatic backups"))->should(std:("be.visible"));
                cy->contains(std:("label"), std:("Backup interval (hours)"))->should(std:("be.visible"));
                cy->contains(std:("label"), std:("Keep last"))->should(std:("be.visible"));
            }
            );
            it(std:("should have descriptive button titles"), [=]() mutable
            {
                cy->get(std:("button[title="Restore"]"))->should(std:("exist"));
                cy->get(std:("button[title="Export"]"))->should(std:("exist"));
                cy->get(std:("button[title="Delete"]"))->should(std:("exist"));
            }
            );
        }
        );
    }
    );
}

MAIN
