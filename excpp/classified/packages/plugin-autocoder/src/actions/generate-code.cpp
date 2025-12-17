#include "generate-code.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> extractProjectType(IAgentRuntime runtime, Memory message, std::optional<State> _state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto text = message.content.text.toLowerCase() || '';

    if (text.includes('plugin')) {
        return 'plugin';
    }
    if (text.includes('agent') || text.includes('bot')) {
        return 'agent';
    }
    if (text.includes('workflow')) {
        return 'workflow';
    }
    if (text.includes('mcp')) {
        return 'mcp';
    }
    if (text.includes('app') || text.includes('application')) {
        return 'full-stack';
    }

    // Check form data
    const auto data = message.content.data as {;
        action?: string;
        projectId?: string;
        projectType?: string;
        formId?: string;
        formData?: any;
        };
        if (data.projectType) {
            return data.projectType;
        }

        // Check recent memories for context
        const auto recentMemories = runtime.getMemories({;
            roomId: message.roomId,
            count: 10,
            unique: false,
            tableName: 'messages',
            });

            for (const auto& memory : recentMemories)
                const auto memText = memory.content.text.toLowerCase() || '';
                if (memText.includes('plugin')) {
                    return 'plugin';
                }
                if (memText.includes('agent')) {
                    return 'agent';
                }
                if (memText.includes('workflow')) {
                    return 'workflow';
                }
            }

            return 'plugin'; // default;

}

std::vector<std::string> extractRequirements(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> requirements = [];

    // Look for bullet points or numbered lists
    const auto lines = text.split('\n');
    lines.forEach((line) => {
        const auto trimmed = line.trim();
        if (trimmed.match(/^[-*•]/) || trimmed.match(/^\d+[.)]/)) {
            requirements.push(trimmed.replace(/^[-*•]\s*/, '').replace(/^\d+[.)]\s*/, ''));
        }
        });

        // If no list found, extract from sentences
        if (requirements.length == 0) {
            const auto sentences = text.match(/[^.!?]+[.!?]+/g) || [];
            sentences.forEach((sentence) => {
                if (sentence.includes('should') || sentence.includes('must') || sentence.includes('need')) {
                    requirements.push(sentence.trim());
                }
                });
            }

            // If still no requirements, use the whole text
            if (requirements.length == 0 && text.length > 20) {
                requirements.push(text);
            }

            return requirements;

}

std::vector<std::string> extractAPIs(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> apis = [];
    const auto apiPatterns = [;
    /(\w+)\s+API/gi,
    /API[:\s]+(\w+)/gi,
    /uses?\s+(\w+)/gi,
    /integrates?\s+with\s+(\w+)/gi,
    ];

    apiPatterns.forEach((pattern) => {
        const auto matches = text.matchAll(pattern);
        for (const auto& match : matches)
            if (match[1]) {
                apis.push(match[1]);
            }
        }
        });

        return [...new Set(apis)]; // Remove duplicates;

}

std::optional<std::string> extractProjectName(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Look for quoted names
    const auto quotedMatch = text.match(/["']([^"']+)["']/);
    if (quotedMatch) {
        return quotedMatch[1];
    }

    // Look for "called" or "named" patterns
    const auto namedMatch = text.match(/(?:called|named)\s+(\S+)/i);
    if (namedMatch) {
        return namedMatch[1];
    }

    return nullptr;

}

std::string getNextStepPrompt(const std::any& form, const std::string& completedStepId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Determine next step in form based on what was just completed
    const auto steps = form.steps || [];
    const auto currentStepIndex = steps.findIndex((s: any) => s.id == completedStepId);

    if (currentStepIndex >= 0 && currentStepIndex < steps.length - 1) {
        const auto nextStep = steps[currentStepIndex + 1];
        return "Great! Now let's " + std::to_string(nextStep.title) + ". " + std::to_string(nextStep.description || '');
    }

    return 'Perfect! I have all the information I need. Let me start generating your code...';

}

std::any extractProjectData(const std::any& formData) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Extract and structure project data from form
    const std::any data = {;
        projectName: formData.projectName || formData.project_name || 'unnamed-project',
        description: formData.description || '',
        requirements: [],
        apis: [],
        targetType: formData.projectType || 'plugin',
        };

        // Extract requirements
        if (formData.requirements) {
            data.requirements = formData.requirements.split('\n').filter((r: string) => r.trim());
        }

        // Extract APIs
        if (formData.apis) {
            data.apis = formData.apis;
            .split(/[,\n]/);
            .map((a: string) => a.trim())
            .filter(Boolean);
        }

        // Extract test scenarios
        if (formData.test_scenarios) {
            data.testScenarios = formData.test_scenarios.split('\n').filter((s: string) => s.trim());
        }

        // GitHub settings
        data.createGithubRepo = formData.create_github == true || formData.create_github == 'true';
        data.privateRepo = formData.private_repo == true || formData.private_repo == 'true';

        // Agent-specific data
        if (formData.agent_name) {
            data.projectName = formData.agent_name;
            data.targetType = 'agent';
        }

        if (formData.personality) {
            "Agent personality: " + std::to_string(formData.personality)
        }

        if (formData.capabilities) {
            "Agent capabilities: " + std::to_string(formData.capabilities)
        }

        if (formData.plugins) {
            const auto plugins = formData.plugins;
            .split(/[,\n]/);
            .map((p: string) => p.trim())
            .filter(Boolean);
            "Include plugins: " + std::to_string(plugins.join(', '))
        }

        return data;

}

} // namespace elizaos
