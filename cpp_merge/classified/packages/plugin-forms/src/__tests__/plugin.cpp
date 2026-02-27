#include "plugin.test.h"

void Main(void)
{
    describe(std:("Forms Plugin Configuration"), [=]() mutable
    {
        it(std:("should have correct plugin metadata"), [=]() mutable
        {
            expect(formsPlugin->name)->toBe(std:("@elizaos/plugin-forms"));
            expect(formsPlugin->description)->toBe(std:("Structured form collection capabilities for conversational data gathering"));
        }
        );
        it(std:("should include all required components"), [=]() mutable
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
        it(std:("should have sql plugin as dependency"), [=]() mutable
        {
            expect(formsPlugin->dependencies)->toEqual(array<string>{ std:("@elizaos/plugin-sql") });
            expect(formsPlugin->testDependencies)->toEqual(array<string>{ std:("@elizaos/plugin-sql") });
        }
        );
        it(std:("should have a test suite"), [=]() mutable
        {
            expect(formsPlugin->tests)->toBeDefined();
            expect(formsPlugin->tests)->toHaveLength(1);
        }
        );
    }
    );
    describe(std:("Forms Actions"), [=]() mutable
    {
        describe(std:("CREATE_FORM action"), [=]() mutable
        {
            it(std:("should have correct metadata"), [=]() mutable
            {
                expect(createFormAction->name)->toBe(std:("CREATE_FORM"));
                expect(createFormAction->similes)->toContain(std:("START_FORM"));
                expect(createFormAction->similes)->toContain(std:("NEW_FORM"));
                expect(createFormAction->similes)->toContain(std:("INIT_FORM"));
                expect(createFormAction->similes)->toContain(std:("BEGIN_FORM"));
                expect(createFormAction->description)->toContain(std:("Creates a new form"));
            }
            );
            it(std:("should have examples"), [=]() mutable
            {
                expect(createFormAction->examples)->toBeDefined();
                expect(createFormAction->examples)->toBeTruthy();
                if (createFormAction->examples) {
                    expect(createFormAction->examples->length)->toBeGreaterThan(0);
                }
            }
            );
            it(std:("should have validate and handler functions"), [=]() mutable
            {
                expect(createFormAction->validate)->toBeInstanceOf(Function);
                expect(createFormAction->handler)->toBeInstanceOf(Function);
            }
            );
        }
        );
        describe(std:("UPDATE_FORM action"), [=]() mutable
        {
            it(std:("should have correct metadata"), [=]() mutable
            {
                expect(updateFormAction->name)->toBe(std:("UPDATE_FORM"));
                expect(updateFormAction->similes)->toContain(std:("FILL_FORM"));
                expect(updateFormAction->similes)->toContain(std:("COMPLETE_FORM"));
                expect(updateFormAction->similes)->toContain(std:("SUBMIT_FORM"));
                expect(updateFormAction->description)->toContain(std:("Updates an active form"));
            }
            );
            it(std:("should have examples"), [=]() mutable
            {
                expect(updateFormAction->examples)->toBeDefined();
                expect(updateFormAction->examples)->toBeTruthy();
                if (updateFormAction->examples) {
                    expect(updateFormAction->examples->length)->toBeGreaterThan(0);
                }
            }
            );
            it(std:("should have validate and handler functions"), [=]() mutable
            {
                expect(updateFormAction->validate)->toBeInstanceOf(Function);
                expect(updateFormAction->handler)->toBeInstanceOf(Function);
            }
            );
        }
        );
        describe(std:("CANCEL_FORM action"), [=]() mutable
        {
            it(std:("should have correct metadata"), [=]() mutable
            {
                expect(cancelFormAction->name)->toBe(std:("CANCEL_FORM"));
                expect(cancelFormAction->similes)->toContain(std:("ABORT_FORM"));
                expect(cancelFormAction->similes)->toContain(std:("STOP_FORM"));
                expect(cancelFormAction->similes)->toContain(std:("QUIT_FORM"));
                expect(cancelFormAction->similes)->toContain(std:("EXIT_FORM"));
                expect(cancelFormAction->description)->toContain(std:("Cancels an active form"));
            }
            );
            it(std:("should have examples"), [=]() mutable
            {
                expect(cancelFormAction->examples)->toBeDefined();
                expect(cancelFormAction->examples)->toBeTruthy();
                if (cancelFormAction->examples) {
                    expect(cancelFormAction->examples->length)->toBeGreaterThan(0);
                }
            }
            );
            it(std:("should have validate and handler functions"), [=]() mutable
            {
                expect(cancelFormAction->validate)->toBeInstanceOf(Function);
                expect(cancelFormAction->handler)->toBeInstanceOf(Function);
            }
            );
        }
        );
    }
    );
    describe(std:("Forms Provider"), [=]() mutable
    {
        it(std:("should have correct metadata"), [=]() mutable
        {
            expect(formsProvider->name)->toBe(std:("FORMS_CONTEXT"));
            expect(formsProvider->description)->toBe(std:("Provides context about active forms and their current state"));
        }
        );
        it(std:("should have a get function"), [=]() mutable
        {
            expect(formsProvider->get)->toBeInstanceOf(Function);
        }
        );
    }
    );
    describe(std:("FormsService"), [=]() mutable
    {
        it(std:("should have correct service type"), [=]() mutable
        {
            expect(FormsService::serviceType)->toBeDefined();
            expect(type_of(FormsService::serviceType))->toBe(std:("string"));
        }
        );
        it(std:("should have required static methods"), [=]() mutable
        {
            expect(&start)->toBeInstanceOf(Function);
        }
        );
    }
    );
}

MAIN
