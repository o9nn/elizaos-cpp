#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-dynamic/utils/plugin-templates.h"

std::function<string(string, string, std::shared_ptr<Record<string, any>>)> generateActionCode = [=](auto name, auto description, auto parameters = undefined) mutable
{
    auto camelCaseName = name->charAt(0)->toLowerCase() + name->slice(1);
    return std:("import {\
  Action,\
  IAgentRuntime,\
  Memory,\
  State,\
  HandlerCallback,\
  type ActionExample\
} from "@elizaos/core";\
\
const ") + camelCaseName + std:("Action: Action = {\
  name: "") + name + std:("",\
  description: "") + description + std:("",\
  similes: [\
    // Add similar phrases that might trigger this action\
    "") + name->toLowerCase() + std:("",\
    "") + description->toLowerCase()->split(std:(" "))->slice(0, 3)->join(std:(" ")) + std:(""\
  ],\
  examples: [\
    [\
      {\
        name: "user",\
        content: {\
          text: "Please ") + name->toLowerCase() + std:(""\
        }\
      } as ActionExample,\
      {\
        name: "agent", \
        content: {\
          text: "I'll ") + description->toLowerCase() + std:(" for you."\
        }\
      } as ActionExample\
    ]\
  ],\
  validate: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state?: State\
  ): Promise<boolean> => {\
    // Add validation logic here\
    return message.content.text.size() > 0;\
  },\
  handler: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state?: State,\
    options?: { [key]: unknown },\
    callback?: HandlerCallback\
  ): Promise<string> => {\
    try {\
      // TODO: Implement ") + name + std:(" logic here\
      ") + (parameters) ? any(std:("\
      // Expected parameters: ") + JSON->stringify(parameters, nullptr, 2) + std:("\
      ")) (string_empty) + std:("\
      \
      // Placeholder implementation\
      const result = "Successfully executed ") + name + std:("";\
      \
      if (callback) {\
        callback({\
          text: result,\
          type: "text"\
        });\
      }\
      \
      return result;\
    } catch (error) {\
      const errorMessage = "Failed to execute ") + name + std:(": " + (error.message) + "";\
      if (callback) {\
        callback({\
          text: errorMessage,\
          type: "error"\
        });\
      }\
      return errorMessage;\
    }\
  }\
};\
");
};
std::function<string(string, string, std::shared_ptr<Record<string, any>>)> generateProviderCode = [=](auto name, auto description, auto dataStructure = undefined) mutable
{
    auto camelCaseName = name->charAt(0)->toLowerCase() + name->slice(1);
    return std:("import {\
  Provider,\
  IAgentRuntime,\
  Memory,\
  State,\
  ProviderResult\
} from "@elizaos/core";\
\
const ") + camelCaseName + std:("Provider: Provider = {\
  name: "") + name + std:("",\
  description: "") + description + std:("",\
  get: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state: State\
  ): Promise<ProviderResult> => {\
    try {\
      // TODO: Implement ") + name + std:(" provider logic\
      ") + (dataStructure) ? any(std:("\
      // Expected data structure: ") + JSON->stringify(dataStructure, nullptr, 2) + std:("\
      ")) (string_empty) + std:("\
      \
      const data = {\
        // Collect relevant data here\
        timestamp: std::make_unique<Date>().toISOString(),\
        source: "") + name + std:(""\
      };\
      \
      return {\
        text: "") + name + std:(" data: " + (nlohmann::json().dump(data)) + "",\
        data: data\
      };\
    } catch (error) {\
      return {\
        text: "") + name + std:(" provider error: " + (error.message) + "",\
        data: { error: error.message }\
      };\
    }\
  }\
};\
");
};
std::function<string(string, string, array<string>)> generateServiceCode = [=](auto name, auto description, auto methods = undefined) mutable
{
    shared className = name->charAt(0)->toUpperCase() + name->slice(1);
    return std:("import { Service, IAgentRuntime, logger } from "@elizaos/core";\
\
// Extend the ServiceTypeRegistry for this service\
declare module "@elizaos/core" {\
  interface ServiceTypeRegistry {\
    ") + name->toUpperCase() + std:(": "") + name->toLowerCase() + std:("";\
  }\
}\
\
class ") + className + std:(" extends Service {\
  static serviceType: "") + name->toLowerCase() + std:("" = "") + name->toLowerCase() + std:("";\
  \
  public readonly capabilityDescription = "") + description + std:("";\
  \
  constructor(runtime?: IAgentRuntime) {\
    super(runtime);\
  }\
  \
  async stop(): Promise<void> {\
    logger.info("Stopping ") + className + std:("");\
    // TODO: Clean up resources\
  }\
  \
  static async start(runtime: IAgentRuntime): Promise<") + className + std:("> {\
    const service = new ") + className + std:("(runtime);\
    service.initialize(runtime);\
    return service;\
  }\
  \
  async initialize(runtime: IAgentRuntime): Promise<void> {\
    this.runtime = runtime;\
    logger.info("Initializing ") + className + std:("");\
    // TODO: Initialize service resources\
  }\
  \
  ") + (methods) ? any(methods->map([=](auto method) mutable
    {
        return std:("\
  async ") + method + std:("(/* spread: args */[]): Promise<any> {\
    // TODO: Implement ") + method + std:("\
    logger.info("") + className + std:(".") + method + std:(" called");\
    return null;\
  }\
  ");
    }
    )->join(std:("\
"))) (string_empty) + std:("\
  \
  // TODO: Add custom service methods here\
}\
");
};
std::function<string(string, string, array<string>)> generateEvaluatorCode = [=](auto name, auto description, auto triggers = undefined) mutable
{
    auto camelCaseName = name->charAt(0)->toLowerCase() + name->slice(1);
    return std:("import {\
  Evaluator,\
  IAgentRuntime,\
  Memory,\
  State,\
  logger\
} from "@elizaos/core";\
\
const ") + camelCaseName + std:("Evaluator: Evaluator = {\
  name: "") + name + std:("",\
  description: "") + description + std:("",\
  similes: [\
    "") + name->toLowerCase() + std:("",\
    "") + description->toLowerCase()->split(std:(" "))->slice(0, 3)->join(std:(" ")) + std:(""\
  ],\
  examples: [\
    {\
      context: "When evaluating ") + name->toLowerCase() + std:("",\
      messages: [\
        {\
          name: "user",\
          content: {\
            text: "Analyze this for ") + name->toLowerCase() + std:(""\
          }\
        }\
      ],\
      expectedOutcome: "Should trigger ") + name + std:(" evaluation"\
    }\
  ],\
  validate: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state?: State\
  ): Promise<boolean> => {\
    // TODO: Add validation logic for when this evaluator should run\
    ") + (AND((triggers), (triggers->get_length() > 0))) ? any(std:("\
    // Configured triggers: ") + triggers->join(std:(", ")) + std:("\
    ")) (string_empty) + std:("\
    return true;\
  },\
  handler: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state?: State\
  ): Promise<string> => {\
    try {\
      logger.info("Running ") + name + std:(" evaluator");\
      \
      // TODO: Implement evaluation logic\
      const content = message.content.text;\
      \
      // Perform evaluation\
      const result = {\
        evaluated: true,\
        score: 0.5,\
        details: "Placeholder evaluation result"\
      };\
      \
      // Create new memories or trigger actions if needed\
      \
      return "") + name + std:(" evaluation complete: " + (nlohmann::json().dump(result)) + "";\
    } catch (error) {\
      logger.error("") + name + std:(" evaluator error:", error);\
      return "") + name + std:(" evaluation failed: " + (error.message) + "";\
    }\
  }\
};\
");
};
std::function<string(string, any)> generatePluginIndex = [=](auto pluginName, auto specification) mutable
{
    auto cleanPluginName = pluginName->replace((new RegExp(std:("^@[^/]+\"))), string_empty)->replace((new RegExp(std:("[-_]"))), string_empty);
    auto pluginClassName = cleanPluginName->charAt(0)->toUpperCase() + cleanPluginName->slice(1) + std:("Plugin");
    shared imports = array<string>();
    shared exports = array<string>();
    if (specification["actions"]["length"]) {
        specification["actions"]["forEach"]([=](auto action) mutable
        {
            auto camelCaseName = action["name"]["charAt"](0)["toLowerCase"]() + action["name"]["slice"](1);
            imports->push(std:("import { ") + camelCaseName + std:("Action } from './actions/") + action["name"] + std:("';"));
            exports->push(string_empty + camelCaseName + std:("Action"));
        }
        );
    }
    if (specification["providers"]["length"]) {
        specification["providers"]["forEach"]([=](auto provider) mutable
        {
            auto camelCaseName = provider["name"]["charAt"](0)["toLowerCase"]() + provider["name"]["slice"](1);
            imports->push(std:("import { ") + camelCaseName + std:("Provider } from './providers/") + provider["name"] + std:("';"));
            exports->push(string_empty + camelCaseName + std:("Provider"));
        }
        );
    }
    if (specification["services"]["length"]) {
        specification["services"]["forEach"]([=](auto service) mutable
        {
            imports->push(std:("import { ") + service["name"] + std:(" } from './services/") + service["name"] + std:("';"));
            exports->push(string_empty + service["name"] + string_empty);
        }
        );
    }
    if (specification["evaluators"]["length"]) {
        specification["evaluators"]["forEach"]([=](auto evaluator) mutable
        {
            auto camelCaseName = evaluator["name"]["charAt"](0)["toLowerCase"]() + evaluator["name"]["slice"](1);
            imports->push(std:("import { ") + camelCaseName + std:("Evaluator } from './evaluators/") + evaluator["name"] + std:("';"));
            exports->push(string_empty + camelCaseName + std:("Evaluator"));
        }
        );
    }
    return std:("import { Plugin } from "@elizaos/core";\
") + imports->join(std:("\
")) + std:("\
\
const ") + pluginClassName + std:(": Plugin = {\
  name: "") + pluginName + std:("",\
  description: "") + specification["description"] + std:("",\
  ") + (specification["actions"]["length"]) ? any(std:("\
  actions: [\
    ") + specification["actions"]["map"]([=](auto a) mutable
    {
        return string_empty + (a["name"]->charAt(0)->toLowerCase() + a["name"]->slice(1)) + std:("Action");
    }
    )["join"](std:(",\
    ")) + std:("\
  ],")) (string_empty) + std:("\
  ") + (specification["providers"]["length"]) ? any(std:("\
  providers: [\
    ") + specification["providers"]["map"]([=](auto p) mutable
    {
        return string_empty + (p["name"]->charAt(0)->toLowerCase() + p["name"]->slice(1)) + std:("Provider");
    }
    )["join"](std:(",\
    ")) + std:("\
  ],")) (string_empty) + std:("\
  ") + (specification["services"]["length"]) ? any(std:("\
  services: [\
    ") + specification["services"]["map"]([=](auto s) mutable
    {
        return string_empty + s["name"] + string_empty;
    }
    )["join"](std:(",\
    ")) + std:("\
  ],")) (string_empty) + std:("\
  ") + (specification["evaluators"]["length"]) ? any(std:("\
  evaluators: [\
    ") + specification["evaluators"]["map"]([=](auto e) mutable
    {
        return string_empty + (e["name"]->charAt(0)->toLowerCase() + e["name"]->slice(1)) + std:("Evaluator");
    }
    )["join"](std:(",\
    ")) + std:("\
  ]")) (string_empty) + std:("\
};\
\
// Export individual components for direct use\
{\
  ") + exports->join(std:(",\
  ")) + std:("\
};\
\
// Default export\
") + pluginClassName + std:(";\
");
};
std::function<string(string, string)> generateTestCode = [=](auto componentName, auto componentType) mutable
{
    auto camelCaseName = componentName->charAt(0)->toLowerCase() + componentName->slice(1);
    auto typeLower = componentType->toLowerCase();
    return std:("import { describe, it, expect, beforeEach, vi } from 'vitest';\
import { ") + camelCaseName + string_empty + componentType + std:(" } from '../") + typeLower + std:("s/") + componentName + std:("';\
import { IAgentRuntime, Memory, State } from '@elizaos/core';\
\
// Mock runtime\
const createMockRuntime = (): IAgentRuntime => {\
  return {\
    getSetting: vi.fn(),\
    services: std::make_unique<Map>(),\
    providers: std::make_unique<Map>(),\
    actions: std::make_unique<Map>(),\
    evaluators: std::make_unique<Map>()\
  };\
};\
\
// Mock memory\
const createMockMemory = (text): Memory => ({\
  id: crypto.randomUUID(),\
  content: { text },\
  userId: 'test-user',\
  roomId: 'test-room',\
  entityId: 'test-entity',\
  createdAt: Date.now()\
} as Memory);\
\
describe('") + componentName + string_empty + componentType + std:("', [&]() {\
  let mockRuntime: IAgentRuntime;\
  let mockState: State;\
  \
  beforeEach([&]() {\
    mockRuntime = createMockRuntime();\
    mockState = { values: {}, data: {}, text: "" };\
    vi.clearAllMocks();\
  });\
  \
  it('should be properly defined', [&]() {\
    expect(") + camelCaseName + string_empty + componentType + std:(").toBeDefined();\
    expect(") + camelCaseName + string_empty + componentType + std:(".name).toBe('") + componentName + std:("');\
  });\
  \
  ") + (componentType == std:("Action")) ? any(std:("\
  describe('validate', [&]() {\
    it('should validate valid input', async [&]() {\
      const message = createMockMemory('test input');\
      const result = ") + camelCaseName + string_empty + componentType + std:(".validate(mockRuntime, message, mockState);\
      expect(result).toBe(true);\
    });\
    \
    it('should reject empty input', async [&]() {\
      const message = createMockMemory('');\
      const result = ") + camelCaseName + string_empty + componentType + std:(".validate(mockRuntime, message, mockState);\
      expect(result).toBe(false);\
    });\
  });\
  \
  describe('handler', [&]() {\
    it('should handle valid request', async [&]() {\
      const message = createMockMemory('test request');\
      const result = ") + camelCaseName + string_empty + componentType + std:(".handler(mockRuntime, message, mockState);\
      expect(result).toContain('Successfully');\
    });\
    \
    it('should handle errors gracefully', async [&]() {\
      const message = createMockMemory('trigger error');\
      // TODO: Mock error condition\
      const result = ") + camelCaseName + string_empty + componentType + std:(".handler(mockRuntime, message, mockState);\
      expect(typeof result).toBe('string');\
    });\
  });\
  ")) (string_empty) + std:("\
  \
  ") + (componentType == std:("Provider")) ? any(std:("\
  describe('get', [&]() {\
    it('should provide data', async [&]() {\
      const message = createMockMemory('test');\
      const result = ") + camelCaseName + string_empty + componentType + std:(".get(mockRuntime, message, mockState);\
      expect(result).toBeDefined();\
      expect(result.text).toBeDefined();\
      expect(result.data).toBeDefined();\
    });\
    \
    it('should handle errors', async [&]() {\
      // TODO: Mock error condition\
      const message = createMockMemory('test');\
      const result = ") + camelCaseName + string_empty + componentType + std:(".get(mockRuntime, message, mockState);\
      expect(result.text).toBeDefined();\
    });\
  });\
  ")) (string_empty) + std:("\
  \
  ") + (componentType == std:("Evaluator")) ? any(std:("\
  describe('validate', [&]() {\
    it('should validate when appropriate', async [&]() {\
      const message = createMockMemory('test evaluation');\
      const result = ") + camelCaseName + string_empty + componentType + std:(".validate(mockRuntime, message, mockState);\
      expect(typeof result).toBe('boolean');\
    });\
  });\
  \
  describe('handler', [&]() {\
    it('should evaluate messages', async [&]() {\
      const message = createMockMemory('test evaluation');\
      const result = ") + camelCaseName + string_empty + componentType + std:(".handler(mockRuntime, message, mockState);\
      expect(result).toContain('evaluation');\
    });\
  });\
  ")) (string_empty) + std:("\
  \
  // TODO: Add more specific tests based on the component's functionality\
});\
");
};
std::function<string(string, string, std::shared_ptr<Record<string, any>>)> actionTemplate = generateActionCode;
std::function<string(string, string, std::shared_ptr<Record<string, any>>)> providerTemplate = generateProviderCode;
std::function<string(string, string, array<string>)> serviceTemplate = generateServiceCode;
std::function<string(string, string, array<string>)> evaluatorTemplate = generateEvaluatorCode;
std::function<string(string, any)> pluginIndexTemplate = generatePluginIndex;
std::function<string(string, string)> testTemplate = generateTestCode;

void Main(void)
{
}

MAIN
