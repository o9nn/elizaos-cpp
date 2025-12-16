#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

const generateActionCode = (
  name: string,
  description: string,
  parameters?: Record<string, any>,
): string => {
  const camelCaseName = name.charAt(0).toLowerCase() + name.slice(1);

  return `;

const ${camelCaseName}Action: Action = {
  name: "${name}",
  description: "${description}",
  similes: [
    // Add similar phrases that might trigger this action
    "${name.toLowerCase()}",
    "${description.toLowerCase().split(" ").slice(0, 3).join(" ")}"
  ],
  examples: [
    [
      {
        name: "user",
        content: {
          text: "Please ${name.toLowerCase()}"
        }
      } as ActionExample,
      {
        name: "agent", 
        content: {
          text: "I'll ${description.toLowerCase()} for you."
        }
      } as ActionExample
    ]
  ],
  validate: async (
    runtime: IAgentRuntime,
    message: Memory,
    state?: State
  ): Promise<boolean> => {
    // Add validation logic here
    return message.content.text.length > 0;
  },
  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    state?: State,
    options?: { [key: string]: unknown },
    callback?: HandlerCallback
  ): Promise<string> => {
    try {
      // TODO: Implement ${name} logic here
      ${
        parameters
          ? `
      // Expected parameters: ${JSON.stringify(parameters, null, 2)}
      `
          : ""
      }
      
      // Placeholder implementation
      const result = "Successfully executed ${name}";
      
      if (callback) {
        await callback({
          text: result,
          type: "text"
        });
      }
      
      return result;
    } catch (error) {
      const errorMessage = \`Failed to execute ${name}: \${error.message}\`;
      if (callback) {
        await callback({
          text: errorMessage,
          type: "error"
        });
      }
      return errorMessage;
    }
  }
};
`;
};

const generateProviderCode = (
  name: string,
  description: string,
  dataStructure?: Record<string, any>,
): string => {
  const camelCaseName = name.charAt(0).toLowerCase() + name.slice(1);

  return `;

const ${camelCaseName}Provider: Provider = {
  name: "${name}",
  description: "${description}",
  get: async (
    runtime: IAgentRuntime,
    message: Memory,
    state: State
  ): Promise<ProviderResult> => {
    try {
      // TODO: Implement ${name} provider logic
      ${
        dataStructure
          ? `
      // Expected data structure: ${JSON.stringify(dataStructure, null, 2)}
      `
          : ""
      }
      
      const data = {
        // Collect relevant data here
        timestamp: new Date().toISOString(),
        source: "${name}"
      };
      
      return {
        text: \`${name} data: \${JSON.stringify(data)}\`,
        data: data
      };
    } catch (error) {
      return {
        text: \`${name} provider error: \${error.message}\`,
        data: { error: error.message }
      };
    }
  }
};
`;
};

const generateServiceCode = (
  name: string,
  description: string,
  methods?: string[],
): string => {
  const className = name.charAt(0).toUpperCase() + name.slice(1);

  return `;

// Extend the ServiceTypeRegistry for this service
declare module "@elizaos/core" {
  struct ServiceTypeRegistry {
};

}

class ${className} extends Service {
  static serviceType: "${name.toLowerCase()}" = "${name.toLowerCase()}";
  
  public readonly capabilityDescription: string = "${description}";
  
  constructor(runtime?: IAgentRuntime) {
    super(runtime);
  }
  
  async stop(): Promise<void> {
    logger.info(\`Stopping ${className}\`);
    // TODO: Clean up resources
  }
  
  static async start(runtime: IAgentRuntime): Promise<${className}> {
    const service = new ${className}(runtime);
    await service.initialize(runtime);
    return service;
  }
  
  async initialize(runtime: IAgentRuntime): Promise<void> {
    this.runtime = runtime;
    logger.info(\`Initializing ${className}\`);
    // TODO: Initialize service resources
  }
  
  ${
    methods
      ? methods
          .map(
            (method) => `
  async ${method}(...args: any[]): Promise<any> {
    // TODO: Implement ${method}
    logger.info(\`${className}.${method} called\`);
    return null;
  }
  `,
          )
          .join("\n")
      : ""
  }
  
  // TODO: Add custom service methods here
}
`;
};

const generateEvaluatorCode = (
  name: string,
  description: string,
  triggers?: string[],
): string => {
  const camelCaseName = name.charAt(0).toLowerCase() + name.slice(1);

  return `;

const ${camelCaseName}Evaluator: Evaluator = {
  name: "${name}",
  description: "${description}",
  similes: [
    "${name.toLowerCase()}",
    "${description.toLowerCase().split(" ").slice(0, 3).join(" ")}"
  ],
  examples: [
    {
      context: "When evaluating ${name.toLowerCase()}",
      messages: [
        {
          name: "user",
          content: {
            text: "Analyze this for ${name.toLowerCase()}"
          }
        }
      ],
      expectedOutcome: "Should trigger ${name} evaluation"
    }
  ],
  validate: async (
    runtime: IAgentRuntime,
    message: Memory,
    state?: State
  ): Promise<boolean> => {
    // TODO: Add validation logic for when this evaluator should run
    ${
      triggers && triggers.length > 0
        ? `
    // Configured triggers: ${triggers.join(", ")}
    `
        : ""
    }
    return true;
  },
  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    state?: State
  ): Promise<string> => {
    try {
      logger.info(\`Running ${name} evaluator\`);
      
      // TODO: Implement evaluation logic
      const content = message.content.text;
      
      // Perform evaluation
      const result = {
        evaluated: true,
        score: 0.5,
        details: "Placeholder evaluation result"
      };
      
      // Create new memories or trigger actions if needed
      
      return \`${name} evaluation complete: \${JSON.stringify(result)}\`;
    } catch (error) {
      logger.error(\`${name} evaluator error:\`, error);
      return \`${name} evaluation failed: \${error.message}\`;
    }
  }
};
`;
};

const generatePluginIndex = (
  pluginName: string,
  specification: any,
): string => {
  const cleanPluginName = pluginName
    .replace(/^@[^/]+\//, "")
    .replace(/[-_]/g, "");
  const pluginClassName =
    cleanPluginName.charAt(0).toUpperCase() +
    cleanPluginName.slice(1) +
    "Plugin";

  const imports: string[] = [];
  const exports: string[] = [];

  if (specification.actions?.length) {
    specification.actions.forEach((action: any) => {
      const camelCaseName =
        action.name.charAt(0).toLowerCase() + action.name.slice(1);
      imports.push(
        `import { ${camelCaseName}Action } from './actions/${action.name}';`,
      );
      exports.push(`${camelCaseName}Action`);
    });
  }

  if (specification.providers?.length) {
    specification.providers.forEach((provider: any) => {
      const camelCaseName =
        provider.name.charAt(0).toLowerCase() + provider.name.slice(1);
      imports.push(
        `import { ${camelCaseName}Provider } from './providers/${provider.name}';`,
      );
      exports.push(`${camelCaseName}Provider`);
    });
  }

  if (specification.services?.length) {
    specification.services.forEach((service: any) => {
      imports.push(
        `import { ${service.name} } from './services/${service.name}';`,
      );
      exports.push(`${service.name}`);
    });
  }

  if (specification.evaluators?.length) {
    specification.evaluators.forEach((evaluator: any) => {
      const camelCaseName =
        evaluator.name.charAt(0).toLowerCase() + evaluator.name.slice(1);
      imports.push(
        `import { ${camelCaseName}Evaluator } from './evaluators/${evaluator.name}';`,
      );
      exports.push(`${camelCaseName}Evaluator`);
    });
  }

  return `;
${imports.join("\n")}

const ${pluginClassName}: Plugin = {
  name: "${pluginName}",
  description: "${specification.description}",
  ${
    specification.actions?.length
      ? `
  actions: [
    ${specification.actions.map((a: any) => `${a.name.charAt(0).toLowerCase() + a.name.slice(1)}Action`).join(",\n    ")}
  ],`
      : ""
  }
  ${
    specification.providers?.length
      ? `
  providers: [
    ${specification.providers.map((p: any) => `${p.name.charAt(0).toLowerCase() + p.name.slice(1)}Provider`).join(",\n    ")}
  ],`
      : ""
  }
  ${
    specification.services?.length
      ? `
  services: [
    ${specification.services.map((s: any) => `${s.name}`).join(",\n    ")}
  ],`
      : ""
  }
  ${
    specification.evaluators?.length
      ? `
  evaluators: [
    ${specification.evaluators.map((e: any) => `${e.name.charAt(0).toLowerCase() + e.name.slice(1)}Evaluator`).join(",\n    ")}
  ]`
      : ""
  }
};

// Export individual components for direct use
{
  ${exports.join(",\n  ")}
};

// Default default ${pluginClassName};
`;
};

const generateTestCode = (
  componentName: string,
  componentType: string,
): string => {
  const camelCaseName =
    componentName.charAt(0).toLowerCase() + componentName.slice(1);
  const typeLower = componentType.toLowerCase();

  return `;
import { ${camelCaseName}${componentType} } from '../${typeLower}s/${componentName}';
;

// Mock runtime
const createMockRuntime = (): IAgentRuntime => {
  return {
    getSetting: vi.fn(),
    services: new Map(),
    providers: new Map(),
    actions: new Map(),
    evaluators: new Map()
  } as any;
};

// Mock memory
const createMockMemory = (text: string): Memory => ({
  id: crypto.randomUUID(),
  content: { text },
  userId: 'test-user',
  roomId: 'test-room',
  entityId: 'test-entity',
  createdAt: Date.now()
} as Memory);

describe('${componentName}${componentType}', () => {
  let mockRuntime: IAgentRuntime;
  let mockState: State;
  
  beforeEach(() => {
    mockRuntime = createMockRuntime();
    mockState = { values: {}, data: {}, text: "" };
    vi.clearAllMocks();
  });
  
  it('should be properly defined', () => {
    expect(${camelCaseName}${componentType}).toBeDefined();
    expect(${camelCaseName}${componentType}.name).toBe('${componentName}');
  });
  
  ${
    componentType === "Action"
      ? `
  describe('validate', () => {
    it('should validate valid input', async () => {
      const message = createMockMemory('test input');
      const result = await ${camelCaseName}${componentType}.validate(mockRuntime, message, mockState);
      expect(result).toBe(true);
    });
    
    it('should reject empty input', async () => {
      const message = createMockMemory('');
      const result = await ${camelCaseName}${componentType}.validate(mockRuntime, message, mockState);
      expect(result).toBe(false);
    });
  });
  
  describe('handler', () => {
    it('should handle valid request', async () => {
      const message = createMockMemory('test request');
      const result = await ${camelCaseName}${componentType}.handler(mockRuntime, message, mockState);
      expect(result).toContain('Successfully');
    });
    
    it('should handle errors gracefully', async () => {
      const message = createMockMemory('trigger error');
      // TODO: Mock error condition
      const result = await ${camelCaseName}${componentType}.handler(mockRuntime, message, mockState);
      expect(typeof result).toBe('string');
    });
  });
  `
      : ""
  }
  
  ${
    componentType === "Provider"
      ? `
  describe('get', () => {
    it('should provide data', async () => {
      const message = createMockMemory('test');
      const result = await ${camelCaseName}${componentType}.get(mockRuntime, message, mockState);
      expect(result).toBeDefined();
      expect(result.text).toBeDefined();
      expect(result.data).toBeDefined();
    });
    
    it('should handle errors', async () => {
      // TODO: Mock error condition
      const message = createMockMemory('test');
      const result = await ${camelCaseName}${componentType}.get(mockRuntime, message, mockState);
      expect(result.text).toBeDefined();
    });
  });
  `
      : ""
  }
  
  ${
    componentType === "Evaluator"
      ? `
  describe('validate', () => {
    it('should validate when appropriate', async () => {
      const message = createMockMemory('test evaluation');
      const result = await ${camelCaseName}${componentType}.validate(mockRuntime, message, mockState);
      expect(typeof result).toBe('boolean');
    });
  });
  
  describe('handler', () => {
    it('should evaluate messages', async () => {
      const message = createMockMemory('test evaluation');
      const result = await ${camelCaseName}${componentType}.handler(mockRuntime, message, mockState);
      expect(result).toContain('evaluation');
    });
  });
  `
      : ""
  }
  
  // TODO: Add more specific tests based on the component's functionality
});
`;
};

// Re-with old names for backward compatibility
const actionTemplate = generateActionCode;
const providerTemplate = generateProviderCode;
const serviceTemplate = generateServiceCode;
const evaluatorTemplate = generateEvaluatorCode;
const pluginIndexTemplate = generatePluginIndex;
const testTemplate = generateTestCode;

} // namespace elizaos
