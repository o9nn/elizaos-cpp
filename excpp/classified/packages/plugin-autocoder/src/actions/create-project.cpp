#include "create-project.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> extractProjectName(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Look for quoted names
    const auto quotedMatch = text.match(/["']([^"']+)["']/);
    if (quotedMatch) {
        return quotedMatch[1];
    }

    // Look for patterns like "called X", "named X"
    const auto namedMatch = text.match(/(?:called|named)\s+(\S+)/i);
    if (namedMatch) {
        return namedMatch[1];
    }

    // Extract from specific patterns
    const auto patterns = [/create\s+(?:a\s+)?(?:new\s+)?(\S+)/i, /(?:build|make)\s+(?:a\s+)?(\S+)/i];

    for (const auto& pattern : patterns)
        const auto match = text.match(pattern);
        if (match) {
            return match[1];
        }
    }

    return nullptr;

}

ProjectType extractProjectType(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lower = text.toLowerCase();

    if (lower.includes('plugin')) {
        return 'plugin';
    }
    if (lower.includes('agent') || lower.includes('bot')) {
        return 'agent';
    }
    if (lower.includes('workflow') || lower.includes('flow')) {
        return 'workflow';
    }
    if (lower.includes('mcp') || lower.includes('model context')) {
        return 'mcp';
    }
    if (lower.includes('app') || lower.includes('application')) {
        return 'full-stack';
    }

    // Default to plugin for ElizaOS
    return 'plugin';

}

void getFormTemplate(const std::string& projectType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (projectType) {
        case 'plugin':
        return {
            title: 'ElizaOS Plugin Project',
            description: "Let's gather information to create your plugin",
            fields: [
            {
                id: 'project_name',
                label: 'Project Name',
                type: 'text',
                placeholder: 'my-awesome-plugin',
                required: true,
                helpText: 'What would you like to name your plugin?',
                },
                {
                    id: 'description',
                    label: 'Description',
                    type: 'textarea',
                    placeholder: 'A plugin that does amazing things...',
                    required: true,
                    helpText: 'Describe what your plugin will do',
                    },
                    {
                        id: 'requirements',
                        label: 'Requirements',
                        type: 'textarea',
                        placeholder: 'List the key features and functionality...',
                        required: true,
                        helpText: 'What specific features or capabilities should this plugin have?',
                        },
                        {
                            id: 'apis',
                            label: 'External APIs (optional)',
                            type: 'textarea',
                            placeholder: 'OpenAI API, Discord API, etc.',
                            required: false,
                            helpText: 'List any external APIs or services this plugin will use',
                            },
                            {
                                id: 'test_scenarios',
                                label: 'Test Scenarios (optional)',
                                type: 'textarea',
                                placeholder: 'User asks for weather, plugin responds with...',
                                required: false,
                                helpText: 'Describe some test scenarios to validate the plugin works correctly',
                                },
                                ],
                                };

                                case 'agent':
                                return {
                                    title: 'ElizaOS Agent Project',
                                    description: "Let's design your agent's personality and capabilities",
                                    fields: [
                                    {
                                        id: 'agent_name',
                                        label: 'Agent Name',
                                        type: 'text',
                                        placeholder: 'My Assistant',
                                        required: true,
                                        helpText: "What's your agent's name?",
                                        },
                                        {
                                            id: 'personality',
                                            label: 'Personality & Bio',
                                            type: 'textarea',
                                            placeholder: 'A helpful, friendly assistant who...',
                                            required: true,
                                            helpText: "Describe your agent's personality and background",
                                            },
                                            {
                                                id: 'capabilities',
                                                label: 'Capabilities',
                                                type: 'textarea',
                                                placeholder: 'Can help with coding, answer questions...',
                                                required: true,
                                                helpText: 'What should your agent be able to do?',
                                                },
                                                {
                                                    id: 'plugins',
                                                    label: 'Plugins to Include',
                                                    type: 'textarea',
                                                    placeholder: 'twitter, discord, knowledge...',
                                                    required: false,
                                                    helpText: 'Which ElizaOS plugins should this agent use?',
                                                    },
                                                    ],
                                                    };

                                                    case 'workflow':
                                                    return {
                                                        title: 'Workflow Project',
                                                        description: 'Define your workflow automation',
                                                        fields: [
                                                        {
                                                            id: 'workflow_name',
                                                            label: 'Workflow Name',
                                                            type: 'text',
                                                            placeholder: 'data-processing-workflow',
                                                            required: true,
                                                            },
                                                            {
                                                                id: 'description',
                                                                label: 'Description',
                                                                type: 'textarea',
                                                                placeholder: 'This workflow processes data and...',
                                                                required: true,
                                                                },
                                                                {
                                                                    id: 'steps',
                                                                    label: 'Workflow Steps',
                                                                    type: 'textarea',
                                                                    placeholder: '1. Fetch data\n2. Process\n3. Store results',
                                                                    required: true,
                                                                    helpText: 'List the steps in your workflow',
                                                                    },
                                                                    {
                                                                        id: 'triggers',
                                                                        label: 'Triggers',
                                                                        type: 'textarea',
                                                                        placeholder: 'On schedule, webhook, manual...',
                                                                        required: false,
                                                                        },
                                                                        ],
                                                                        };

                                                                        default:
                                                                        return {
                                                                            title: 'New Project',
                                                                            description: 'Tell me about your project',
                                                                            fields: [
                                                                            {
                                                                                id: 'project_name',
                                                                                label: 'Project Name',
                                                                                type: 'text',
                                                                                required: true,
                                                                                },
                                                                                {
                                                                                    id: 'description',
                                                                                    label: 'Description',
                                                                                    type: 'textarea',
                                                                                    required: true,
                                                                                    },
                                                                                    {
                                                                                        id: 'requirements',
                                                                                        label: 'Requirements',
                                                                                        type: 'textarea',
                                                                                        required: true,
                                                                                        },
                                                                                        ],
                                                                                        };
                                                                                    }

}

} // namespace elizaos
