#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-plugin-starter/src/plugins/calculator/eval-basic.h"

std::shared_ptr<Evaluator> calculationEvaluator = object{
    object::pair{std:("name"), std:("VALIDATE_CALCULATION")}, 
    object::pair{std:("similes"), array<string>{ std:("CHECK_MATH"), std:("VERIFY_CALCULATION"), std:("MATH_CHECK") }}, 
    object::pair{std:("alwaysRun"), false}, 
    object::pair{std:("description"), std:("Validates mathematical calculations and checks for computational accuracy")}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        auto mathPattern = (new RegExp(std:("[\d+\-*/()")));
        return mathPattern->test(message["content"]["text"]);
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message) mutable
    {
        auto result = parseFloat(message["content"]["text"]);
        return object{
            object::pair{std:("isValid"), !isNaN(result)}, 
            object::pair{std:("calculatedValue"), result}
        };
    }
    }, 
    object::pair{std:("examples"), array<object>{ object{
        object::pair{std:("context"), std:("{{user1}} is using a calculator function")}, 
        object::pair{std:("messages"), array<object>{ object{
            object::pair{std:("user"), std:("{{user1}}")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("What is 25 * 4?")}, 
                object::pair{std:("action"), std:("CALCULATE")}
            }}
        }, object{
            object::pair{std:("user"), std:("Calculator")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("The result is 100")}, 
                object::pair{std:("action"), std:("RESPOND")}
            }}
        } }}, 
        object::pair{std:("outcome"), std:("{\
                "isValid": true,\
                "calculatedValue": 100\
            }")}
    }, object{
        object::pair{std:("context"), std:("{{user1}} receives an invalid calculation")}, 
        object::pair{std:("messages"), array<object>{ object{
            object::pair{std:("user"), std:("{{user1}}")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("What is 10 / 0?")}, 
                object::pair{std:("action"), std:("CALCULATE")}
            }}
        }, object{
            object::pair{std:("user"), std:("Calculator")}, 
            object::pair{std:("content"), object{
                object::pair{std:("text"), std:("Cannot divide by zero")}, 
                object::pair{std:("action"), std:("RESPOND")}
            }}
        } }}, 
        object::pair{std:("outcome"), std:("{\
                "isValid": false,\
                "calculatedValue": null\
            }")}
    } }}
};

void Main(void)
{
}

MAIN
