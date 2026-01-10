#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-dynamic/utils/plugin-templates.h"

std::function<string(string, string, std::shared_ptr<Record<string, any>>)> generateActionCode = [=](auto name, auto description, auto parameters = undefined) mutable
{
    auto camelCaseName = name->charAt(0)->toLowerCase() + name->slice(1);
    return std::string("import {\
  Action,\
  IAgentRuntime,\
  Memory,\
  State,\
  HandlerCallback,\
  type ActionExample\
} from "@elizaos/core";\
\
export const ") + camelCaseName + std::string("Action: Action = {\
  name: "") + name + std::string("",\
  description: "") + description + std::string("",\
  similes: [\
    // Add similar phrases that might trigger this action\
    "") + name->toLowerCase() + std::string("",\
    "") + description->toLowerCase()->split(std::string(" "))->slice(0, 3)->join(std::string(" ")) + std::string(""\
  ],\
  examples: [\
    [\
      {\
        name: "user",\
        content: {\
          text: "Please ") + name->toLowerCase() + std::string(""\
        }\
      } as ActionExample,\
      {\
        name: "agent", \
        content: {\
          text: "I'll ") + description->toLowerCase() + std::string(" for you."\
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
    return message.content.text.length > 0;\
  },\
  handler: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state?: State,\
    options?: { [key: string]: unknown },\
    callback?: HandlerCallback\
  ): Promise<string> => {\
    try {\
      // TODO: Implement ") + name + std::string(" logic here\
      ") + (parameters) ? any(std::string("\
      // Expected parameters: ") + JSON->stringify(parameters, nullptr, 2) + std::string("\
      ")) : any(string_empty) + std::string("\
      \
      // Placeholder implementation\
      const result = "Successfully executed ") + name + std::string("";\
      \
      if (callback) {\
        await callback({\
          text: result,\
          type: "text"\
        });\
      }\
      \
      return result;\
    } catch (error) {\
      const errorMessage = `Failed to execute ") + name + std::string(": ${error.message}`;\
      if (callback) {\
        await callback({\
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
    return std::string("import {\
  Provider,\
  IAgentRuntime,\
  Memory,\
  State,\
  ProviderResult\
} from "@elizaos/core";\
\
export const ") + camelCaseName + std::string("Provider: Provider = {\
  name: "") + name + std::string("",\
  description: "") + description + std::string("",\
  get: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state: State\
  ): Promise<ProviderResult> => {\
    try {\
      // TODO: Implement ") + name + std::string(" provider logic\
      ") + (dataStructure) ? any(std::string("\
      // Expected data structure: ") + JSON->stringify(dataStructure, nullptr, 2) + std::string("\
      ")) : any(string_empty) + std::string("\
      \
      const data = {\
        // Collect relevant data here\
        timestamp: new Date().toISOString(),\
        source: "") + name + std::string(""\
      };\
      \
      return {\
        text: `") + name + std::string(" data: ${JSON.stringify(data)}`,\
        data: data\
      };\
    } catch (error) {\
      return {\
        text: `") + name + std::string(" provider error: ${error.message}`,\
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
    return std::string("import { Service, IAgentRuntime, logger } from "@elizaos/core";\
\
// Extend the ServiceTypeRegistry for this service\
declare module "@elizaos/core" {\
  interface ServiceTypeRegistry {\
    ") + name->toUpperCase() + std::string(": "") + name->toLowerCase() + std::string("";\
  }\
}\
\
export class ") + className + std::string(" extends Service {\
  static serviceType: "") + name->toLowerCase() + std::string("" = "") + name->toLowerCase() + std::string("";\
  \
  public readonly capabilityDescription: string = "") + description + std::string("";\
  \
  constructor(runtime?: IAgentRuntime) {\
    super(runtime);\
  }\
  \
  async stop(): Promise<void> {\
    logger.info(`Stopping ") + className + std::string("`);\
    // TODO: Clean up resources\
  }\
  \
  static async start(runtime: IAgentRuntime): Promise<") + className + std::string("> {\
    const service = new ") + className + std::string("(runtime);\
    await service.initialize(runtime);\
    return service;\
  }\
  \
  async initialize(runtime: IAgentRuntime): Promise<void> {\
    this.runtime = runtime;\
    logger.info(`Initializing ") + className + std::string("`);\
    // TODO: Initialize service resources\
  }\
  \
  ") + (methods) ? any(methods->map([=](auto method) mutable
    {
        return std::string("\
  async ") + method + std::string("(...args: any[]): Promise<any> {\
    // TODO: Implement ") + method + std::string("\
    logger.info(`") + className + std::string(".") + method + std::string(" called`);\
    return null;\
  }\
  ");
    }
    )->join(std::string("\
"))) : any(string_empty) + std::string("\
  \
  // TODO: Add custom service methods here\
}\
");
};
std::function<string(string, string, array<string>)> generateEvaluatorCode = [=](auto name, auto description, auto triggers = undefined) mutable
{
    auto camelCaseName = name->charAt(0)->toLowerCase() + name->slice(1);
    return std::string("import {\
  Evaluator,\
  IAgentRuntime,\
  Memory,\
  State,\
  logger\
} from "@elizaos/core";\
\
export const ") + camelCaseName + std::string("Evaluator: Evaluator = {\
  name: "") + name + std::string("",\
  description: "") + description + std::string("",\
  similes: [\
    "") + name->toLowerCase() + std::string("",\
    "") + description->toLowerCase()->split(std::string(" "))->slice(0, 3)->join(std::string(" ")) + std::string(""\
  ],\
  examples: [\
    {\
      context: "When evaluating ") + name->toLowerCase() + std::string("",\
      messages: [\
        {\
          name: "user",\
          content: {\
            text: "Analyze this for ") + name->toLowerCase() + std::string(""\
          }\
        }\
      ],\
      expectedOutcome: "Should trigger ") + name + std::string(" evaluation"\
    }\
  ],\
  validate: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state?: State\
  ): Promise<boolean> => {\
    // TODO: Add validation logic for when this evaluator should run\
    ") + (AND((triggers), (triggers->get_length() > 0))) ? any(std::string("\
    // Configured triggers: ") + triggers->join(std::string(", ")) + std::string("\
    ")) : any(string_empty) + std::string("\
    return true;\
  },\
  handler: async (\
    runtime: IAgentRuntime,\
    message: Memory,\
    state?: State\
  ): Promise<string> => {\
    try {\
      logger.info(`Running ") + name + std::string(" evaluator`);\
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
      return `") + name + std::string(" evaluation complete: ${JSON.stringify(result)}`;\
    } catch (error) {\
      logger.error(`") + name + std::string(" evaluator error:`, error);\
      return `") + name + std::string(" evaluation failed: ${error.message}`;\
    }\
  }\
};\
");
};
std::function<string(string, any)> generatePluginIndex = [=](auto pluginName, auto specification) mutable
{
    auto cleanPluginName = pluginName->replace((new RegExp(std::string("^@[^/]+\"))), string_empty)->replace((new RegExp(std::string("[-_]"))), string_empty);
    auto pluginClassName = cleanPluginName->charAt(0)->toUpperCase() + cleanPluginName->slice(1) + std::string("Plugin");
    shared imports = array<string>();
    shared exports = array<string>();
    if (specification["actions"]["length"]) {
        specification["actions"]["forEach"]([=](auto action) mutable
        {
            auto camelCaseName = action["name"]["charAt"](0)["toLowerCase"]() + action["name"]["slice"](1);
            imports->push(std::string("import { ") + camelCaseName + std::string("Action } from './actions/") + action["name"] + std::string("';"));
            exports->push(string_empty + camelCaseName + std::string("Action"));
        }
        );
    }
    if (specification["providers"]["length"]) {
        specification["providers"]["forEach"]([=](auto provider) mutable
        {
            auto camelCaseName = provider["name"]["charAt"](0)["toLowerCase"]() + provider["name"]["slice"](1);
            imports->push(std::string("import { ") + camelCaseName + std::string("Provider } from './providers/") + provider["name"] + std::string("';"));
            exports->push(string_empty + camelCaseName + std::string("Provider"));
        }
        );
    }
    if (specification["services"]["length"]) {
        specification["services"]["forEach"]([=](auto service) mutable
        {
            imports->push(std::string("import { ") + service["name"] + std::string(" } from './services/") + service["name"] + std::string("';"));
            exports->push(string_empty + service["name"] + string_empty);
        }
        );
    }
    if (specification["evaluators"]["length"]) {
        specification["evaluators"]["forEach"]([=](auto evaluator) mutable
        {
            auto camelCaseName = evaluator["name"]["charAt"](0)["toLowerCase"]() + evaluator["name"]["slice"](1);
            imports->push(std::string("import { ") + camelCaseName + std::string("Evaluator } from './evaluators/") + evaluator["name"] + std::string("';"));
            exports->push(string_empty + camelCaseName + std::string("Evaluator"));
        }
        );
    }
    return std::string("import { Plugin } from "@elizaos/core";\
") + imports->join(std::string("\
")) + std::string("\
\
export const ") + pluginClassName + std::string(": Plugin = {\
  name: "") + pluginName + std::string("",\
  description: "") + specification["description"] + std::string("",\
  ") + (specification["actions"]["length"]) ? any(std::string("\
  actions: [\
    ") + specification["actions"]["map"]([=](auto a) mutable
    {
        return string_empty + (a["name"]->charAt(0)->toLowerCase() + a["name"]->slice(1)) + std::string("Action");
    }
    )["join"](std::string(",\
    ")) + std::string("\
  ],")) : any(string_empty) + std::string("\
  ") + (specification["providers"]["length"]) ? any(std::string("\
  providers: [\
    ") + specification["providers"]["map"]([=](auto p) mutable
    {
        return string_empty + (p["name"]->charAt(0)->toLowerCase() + p["name"]->slice(1)) + std::string("Provider");
    }
    )["join"](std::string(",\
    ")) + std::string("\
  ],")) : any(string_empty) + std::string("\
  ") + (specification["services"]["length"]) ? any(std::string("\
  services: [\
    ") + specification["services"]["map"]([=](auto s) mutable
    {
        return string_empty + s["name"] + string_empty;
    }
    )["join"](std::string(",\
    ")) + std::string("\
  ],")) : any(string_empty) + std::string("\
  ") + (specification["evaluators"]["length"]) ? any(std::string("\
  evaluators: [\
    ") + specification["evaluators"]["map"]([=](auto e) mutable
    {
        return string_empty + (e["name"]->charAt(0)->toLowerCase() + e["name"]->slice(1)) + std::string("Evaluator");
    }
    )["join"](std::string(",\
    ")) + std::string("\
  ]")) : any(string_empty) + std::string("\
};\
\
// Export individual components for direct use\
export {\
  ") + exports->join(std::string(",\
  ")) + std::string("\
};\
\
// Default export\
export default ") + pluginClassName + std::string(";\
");
};
std::function<string(string, string)> generateTestCode = [=](auto componentName, auto componentType) mutable
{
    auto camelCaseName = componentName->charAt(0)->toLowerCase() + componentName->slice(1);
    auto typeLower = componentType->toLowerCase();
    return std::string("import { describe, it, expect, beforeEach, vi } from 'vitest';\
import { ") + camelCaseName + string_empty + componentType + std::string(" } from '../") + typeLower + std::string("s/") + componentName + std::string("';\
import { IAgentRuntime, Memory, State } from '@elizaos/core';\
\
// Mock runtime\
const createMockRuntime = (): IAgentRuntime => {\
  return {\
    getSetting: vi.fn(),\
    services: new Map(),\
    providers: new Map(),\
    actions: new Map(),\
    evaluators: new Map()\
  } as any;\
};\
\
// Mock memory\
const createMockMemory = (text: string): Memory => ({\
  id: crypto.randomUUID(),\
  content: { text },\
  userId: 'test-user',\
  roomId: 'test-room',\
  entityId: 'test-entity',\
  createdAt: Date.now()\
} as Memory);\
\
describe('") + componentName + string_empty + componentType + std::string("', () => {\
  let mockRuntime: IAgentRuntime;\
  let mockState: State;\
  \
  beforeEach(() => {\
    mockRuntime = createMockRuntime();\
    mockState = { values: {}, data: {}, text: "" };\
    vi.clearAllMocks();\
  });\
  \
  it('should be properly defined', () => {\
    expect(") + camelCaseName + string_empty + componentType + std::string(").toBeDefined();\
    expect(") + camelCaseName + string_empty + componentType + std::string(".name).toBe('") + componentName + std::string("');\
  });\
  \
  ") + (componentType == std::string("Action")) ? any(std::string("\
  describe('validate', () => {\
    it('should validate valid input', async () => {\
      const message = createMockMemory('test input');\
      const result = await ") + camelCaseName + string_empty + componentType + std::string(".validate(mockRuntime, message, mockState);\
      expect(result).toBe(true);\
    });\
    \
    it('should reject empty input', async () => {\
      const message = createMockMemory('');\
      const result = await ") + camelCaseName + string_empty + componentType + std::string(".validate(mockRuntime, message, mockState);\
      expect(result).toBe(false);\
    });\
  });\
  \
  describe('handler', () => {\
    it('should handle valid request', async () => {\
      const message = createMockMemory('test request');\
      const result = await ") + camelCaseName + string_empty + componentType + std::string(".handler(mockRuntime, message, mockState);\
      expect(result).toContain('Successfully');\
    });\
    \
    it('should handle errors gracefully', async () => {\
      const message = createMockMemory('trigger error');\
      // TODO: Mock error condition\
      const result = await ") + camelCaseName + string_empty + componentType + std::string(".handler(mockRuntime, message, mockState);\
      expect(typeof result).toBe('string');\
    });\
  });\
  ")) : any(string_empty) + std::string("\
  \
  ") + (componentType == std::string("Provider")) ? any(std::string("\
  describe('get', () => {\
    it('should provide data', async () => {\
      const message = createMockMemory('test');\
      const result = await ") + camelCaseName + string_empty + componentType + std::string(".get(mockRuntime, message, mockState);\
      expect(result).toBeDefined();\
      expect(result.text).toBeDefined();\
      expect(result.data).toBeDefined();\
    });\
    \
    it('should handle errors', async () => {\
      // TODO: Mock error condition\
      const message = createMockMemory('test');\
      const result = await ") + camelCaseName + string_empty + componentType + std::string(".get(mockRuntime, message, mockState);\
      expect(result.text).toBeDefined();\
    });\
  });\
  ")) : any(string_empty) + std::string("\
  \
  ") + (componentType == std::string("Evaluator")) ? any(std::string("\
  describe('validate', () => {\
    it('should validate when appropriate', async () => {\
      const message = createMockMemory('test evaluation');\
      const result = await ") + camelCaseName + string_empty + componentType + std::string(".validate(mockRuntime, message, mockState);\
      expect(typeof result).toBe('boolean');\
    });\
  });\
  \
  describe('handler', () => {\
    it('should evaluate messages', async () => {\
      const message = createMockMemory('test evaluation');\
      const result = await ") + camelCaseName + string_empty + componentType + std::string(".handler(mockRuntime, message, mockState);\
      expect(result).toContain('evaluation');\
    });\
  });\
  ")) : any(string_empty) + std::string("\
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
