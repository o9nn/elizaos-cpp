#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/tests/test-flake8-utils.test.h"

void Main(void)
{
    describe(std::string("Flake8 Utils"), [=]() mutable
    {
        describe(std::string("Flake8Error parsing"), [=]() mutable
        {
            it(std::string("should parse flake8 error line correctly"), [=]() mutable
            {
                auto line = std::string("existing_lint error.py:12:41: E999 SyntaxError: invalid syntax");
                auto error = Flake8ErrorClass::fromLine(line);
                expect(error->file)->toBe(std::string("existing_lint error.py"));
                expect(error->line)->toBe(12);
                expect(error->column)->toBe(41);
                expect(error->message)->toBe(std::string("E999 SyntaxError: invalid syntax"));
            }
            );
            it(std::string("should handle file paths with spaces"), [=]() mutable
            {
                auto line = std::string("my file.py:5:10: E301 expected 1 blank line");
                auto error = Flake8ErrorClass::fromLine(line);
                expect(error->file)->toBe(std::string("my file.py"));
                expect(error->line)->toBe(5);
                expect(error->column)->toBe(10);
                expect(error->message)->toBe(std::string("E301 expected 1 blank line"));
            }
            );
            it(std::string("should throw on invalid line format"), [=]() mutable
            {
                shared invalidLine = std::string("not a valid flake8 line");
                expect([=]() mutable
                {
                    return Flake8ErrorClass::fromLine(invalidLine);
                }
                )->toThrow();
            }
            );
        }
        );
        describe(std::string("formatFlake8Output"), [=]() mutable
        {
            it(std::string("should return empty string when no new errors"), [=]() mutable
            {
                auto result = formatFlake8Output(std::string("a:12:41: e"), std::string("a:12:41: e"), std::tuple<double, double>{ 50, 51 }, 10, false);
                expect(result)->toBe(string_empty);
            }
            );
            it(std::string("should return empty string when error outside replacement window"), [=]() mutable
            {
                auto result = formatFlake8Output(std::string("a:12:41: e"), std::string("a:13:41: e"), std::tuple<double, double>{ 1, 2 }, 1, false);
                expect(result)->toBe(string_empty);
            }
            );
            it(std::string("should return empty string without line numbers"), [=]() mutable
            {
                auto result = formatFlake8Output(std::string("a:12:41: e"), std::string("a:13:41: e"), std::tuple<double, double>{ 1, 2 }, 1, false);
                expect(result)->toBe(string_empty);
            }
            );
            it(std::string("should format new error without line numbers"), [=]() mutable
            {
                auto result = formatFlake8Output(std::string("a:12:41: e"), std::string("a:13:41: e"), std::tuple<double, double>{ 12, 13 }, 10, false);
                expect(result)->toBe(std::string("- e"));
            }
            );
            it(std::string("should format new error with line numbers"), [=]() mutable
            {
                auto result = formatFlake8Output(std::string("a:12:41: e"), std::string("a:13:41: e"), std::tuple<double, double>{ 12, 13 }, 10, true);
                expect(result)->toBe(std::string("- line 12 col 41: e"));
            }
            );
            it(std::string("should handle multiple errors"), [=]() mutable
            {
                auto currentErrors = std::string("file.py:10:5: E301 expected 1 blank line\
file.py:15:10: W292 no newline at end of file\
file.py:20:1: E302 expected 2 blank lines");
                auto previousErrors = std::string("file.py:10:5: E301 expected 1 blank line");
                auto result = formatFlake8Output(currentErrors, previousErrors, std::tuple<double, double>{ 14, 16 }, 3, true);
                auto lines = result->split(std::string("\
"));
                expect(lines->get_length())->toBeGreaterThan(0);
                expect(result)->toContain(std::string("E302"));
            }
            );
            it(std::string("should adjust line numbers after replacement"), [=]() mutable
            {
                auto currentErrors = std::string("file.py:25:1: E302 expected 2 blank lines");
                auto previousErrors = std::string("file.py:20:1: E302 expected 2 blank lines");
                auto result = formatFlake8Output(currentErrors, previousErrors, std::tuple<double, double>{ 10, 12 }, 8, true);
                expect(result)->toBe(string_empty);
            }
            );
        }
        );
    }
    );
}

MAIN
