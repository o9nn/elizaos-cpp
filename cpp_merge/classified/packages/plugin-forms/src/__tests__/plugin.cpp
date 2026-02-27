#include "plugin.test.h"
#include <string>

void Main(void)
{
    describe(std::string("Forms Plugin Configuration"), [=]() mutable
    {
        it(std::string("should have correct plugin metadata"), [=]() mutable
        {
            expect(formsPlugin->name)->toBe(std::string("@elizaos/plugin-forms"));
            expect(formsPlugin->description)->toBe(std::string("Structured form collection capabilities for conversational data gathering"));
        }
        );
        it(std::string("should include all required components"), [=]() mutable
        {
            expect(formsPlugin->services)->toContain(FormsService);
            expect(formsPlugin->services)->toHaveLength(1);
            expect(formsPlugin->actions)->toContain(createFormAction);
            expect(formsPlugin->actions)->toContain(updateFormAction);
            expect(formsPlugin->actions)->toContain(cancelFormAction);
            expect(formsPlugin->actions)->toHaveLength(3);
            expect(formsPlugin->providers)->toContain(formsProvider);
            expect(formsPlugin->providers)->toHaveLength(1);
            expect(formsPlugin->evaluators)->toEqual(array<any>());
        }
        );
        it(std::string("should have sql plugin as dependency"), [=]() mutable
        {
            expect(formsPlugin->dependencies)->toEqual(array<string>{ std::string("@elizaos/plugin-sql") });
            expect(formsPlugin->testDependencies)->toEqual(array<string>{ std::string("@elizaos/plugin-sql") });
        }
        );
        it(std::string("should have a test suite"), [=]() mutable
        {
            expect(formsPlugin->tests)->toBeDefined();
            expect(formsPlugin->tests)->toHaveLength(1);
        }
        );
    }
    );
    describe(std::string("Forms Actions"), [=]() mutable
    {
        describe(std::string("CREATE_FORM action"), [=]() mutable
        {
            it(std::string("should have correct metadata"), [=]() mutable
            {
                expect(createFormAction->name)->toBe(std::string("CREATE_FORM"));
                expect(createFormAction->similes)->toContain(std::string("START_FORM"));
                expect(createFormAction->similes)->toContain(std::string("NEW_FORM"));
                expect(createFormAction->similes)->toContain(std::string("INIT_FORM"));
                expect(createFormAction->similes)->toContain(std::string("BEGIN_FORM"));
                expect(createFormAction->description)->toContain(std::string("Creates a new form"));
            }
            );
            it(std::string("should have examples"), [=]() mutable
            {
                expect(createFormAction->examples)->toBeDefined();
                expect(createFormAction->examples)->toBeTruthy();
                if (createFormAction->examples) {
                    expect(createFormAction->examples->length)->toBeGreaterThan(0);
                }
            }
            );
            it(std::string("should have validate and handler functions"), [=]() mutable
            {
                expect(createFormAction->validate)->toBeInstanceOf(Function);
                expect(createFormAction->handler)->toBeInstanceOf(Function);
            }
            );
        }
        );
        describe(std::string("UPDATE_FORM action"), [=]() mutable
        {
            it(std::string("should have correct metadata"), [=]() mutable
            {
                expect(updateFormAction->name)->toBe(std::string("UPDATE_FORM"));
                expect(updateFormAction->similes)->toContain(std::string("FILL_FORM"));
                expect(updateFormAction->similes)->toContain(std::string("COMPLETE_FORM"));
                expect(updateFormAction->similes)->toContain(std::string("SUBMIT_FORM"));
                expect(updateFormAction->description)->toContain(std::string("Updates an active form"));
            }
            );
            it(std::string("should have examples"), [=]() mutable
            {
                expect(updateFormAction->examples)->toBeDefined();
                expect(updateFormAction->examples)->toBeTruthy();
                if (updateFormAction->examples) {
                    expect(updateFormAction->examples->length)->toBeGreaterThan(0);
                }
            }
            );
            it(std::string("should have validate and handler functions"), [=]() mutable
            {
                expect(updateFormAction->validate)->toBeInstanceOf(Function);
                expect(updateFormAction->handler)->toBeInstanceOf(Function);
            }
            );
        }
        );
        describe(std::string("CANCEL_FORM action"), [=]() mutable
        {
            it(std::string("should have correct metadata"), [=]() mutable
            {
                expect(cancelFormAction->name)->toBe(std::string("CANCEL_FORM"));
                expect(cancelFormAction->similes)->toContain(std::string("ABORT_FORM"));
                expect(cancelFormAction->similes)->toContain(std::string("STOP_FORM"));
                expect(cancelFormAction->similes)->toContain(std::string("QUIT_FORM"));
                expect(cancelFormAction->similes)->toContain(std::string("EXIT_FORM"));
                expect(cancelFormAction->description)->toContain(std::string("Cancels an active form"));
            }
            );
            it(std::string("should have examples"), [=]() mutable
            {
                expect(cancelFormAction->examples)->toBeDefined();
                expect(cancelFormAction->examples)->toBeTruthy();
                if (cancelFormAction->examples) {
                    expect(cancelFormAction->examples->length)->toBeGreaterThan(0);
                }
            }
            );
            it(std::string("should have validate and handler functions"), [=]() mutable
            {
                expect(cancelFormAction->validate)->toBeInstanceOf(Function);
                expect(cancelFormAction->handler)->toBeInstanceOf(Function);
            }
            );
        }
        );
    }
    );
    describe(std::string("Forms Provider"), [=]() mutable
    {
        it(std::string("should have correct metadata"), [=]() mutable
        {
            expect(formsProvider->name)->toBe(std::string("FORMS_CONTEXT"));
            expect(formsProvider->description)->toBe(std::string("Provides context about active forms and their current state"));
        }
        );
        it(std::string("should have a get function"), [=]() mutable
        {
            expect(formsProvider->get)->toBeInstanceOf(Function);
        }
        );
    }
    );
    describe(std::string("FormsService"), [=]() mutable
    {
        it(std::string("should have correct service type"), [=]() mutable
        {
            expect(FormsService::serviceType)->toBeDefined();
            expect(type_of(FormsService::serviceType))->toBe(std::string("string"));
        }
        );
        it(std::string("should have required static methods"), [=]() mutable
        {
            expect(&start)->toBeInstanceOf(Function);
        }
        );
    }
    );
}

MAIN
