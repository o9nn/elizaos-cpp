#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-plugin-starter/src/plugins/calculator/eval-basic.h"

std::shared_ptr<Evaluator> calculationEvaluator = object{
    object::pair{std::string("name"), std::string("VALIDATE_CALCULATION")}, 
    object::pair{std::string("similes"), array<string>{ std::string("CHECK_MATH"), std::string("VERIFY_CALCULATION"), std::string("MATH_CHECK") }}, 
    object::pair{std::string("alwaysRun"), false}, 
    object::pair{std::string("description"), std::string("Validates mathematical calculations and checks for computational accuracy")}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        auto mathPattern = (new RegExp(std::string("[\d+\-*/()")));
        return mathPattern->test(message["content"]["text"]);
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message) mutable
    {
        auto result = parseFloat(message["content"]["text"]);
        return object{
            object::pair{std::string("isValid"), !isNaN(result)}, 
            object::pair{std::string("calculatedValue"), result}
        };
    }
    }, 
    object::pair{std::string("examples"), array<object>{ object{
        object::pair{std::string("context"), std::string("{{user1}} is using a calculator function")}, 
        object::pair{std::string("messages"), array<object>{ object{
            object::pair{std::string("user"), std::string("{{user1}}")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("What is 25 * 4?")}, 
                object::pair{std::string("action"), std::string("CALCULATE")}
            }}
        }, object{
            object::pair{std::string("user"), std::string("Calculator")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("The result is 100")}, 
                object::pair{std::string("action"), std::string("RESPOND")}
            }}
        } }}, 
        object::pair{std::string("outcome"), std::string("{\
                "isValid": true,\
                "calculatedValue": 100\
            }")}
    }, object{
        object::pair{std::string("context"), std::string("{{user1}} receives an invalid calculation")}, 
        object::pair{std::string("messages"), array<object>{ object{
            object::pair{std::string("user"), std::string("{{user1}}")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("What is 10 / 0?")}, 
                object::pair{std::string("action"), std::string("CALCULATE")}
            }}
        }, object{
            object::pair{std::string("user"), std::string("Calculator")}, 
            object::pair{std::string("content"), object{
                object::pair{std::string("text"), std::string("Cannot divide by zero")}, 
                object::pair{std::string("action"), std::string("RESPOND")}
            }}
        } }}, 
        object::pair{std::string("outcome"), std::string("{\
                "isValid": false,\
                "calculatedValue": null\
            }")}
    } }}
};

void Main(void)
{
}

MAIN
