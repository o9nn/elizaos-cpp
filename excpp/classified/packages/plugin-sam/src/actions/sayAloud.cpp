#include "sayAloud.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractTextToSpeak(const std::string& messageText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Clean the input
    const auto text = messageText.toLowerCase().trim();

    // Patterns to extract quoted text
    const auto quotedPatterns = [;
    /say ["']([^"']+)["']/,
    /speak ["']([^"']+)["']/,
    /read ["']([^"']+)["']/,
    /announce ["']([^"']+)["']/,
    /["']([^"']+)["']/,
    ];

    // Try to find quoted text first
    for (const auto& pattern : quotedPatterns)
        const auto match = text.match(pattern);
        if (match) {
            return match[1];
        }
    }

    // Patterns to extract text after keywords
    const auto afterKeywordPatterns = [;
    /(?:say|speak|read)\s+(?:aloud\s+)?(?:this\s+)?:?\s*(.+)$/,
    /(?:can you|please)\s+(?:say|speak|read)\s+(?:aloud\s+)?(.+)$/,
    /(?:i want to hear|let me hear)\s+(.+)$/,
    /(?:read this|say this|speak this)\s*:?\s*(.+)$/,
    ];

    // Try to find text after keywords
    for (const auto& pattern : afterKeywordPatterns)
        const auto match = text.match(pattern);
        if (match) {
            auto extractedText = match[1].trim();

            // Clean up common trailing words
            extractedText = extractedText;
            .replace(/\s+out loud$/, '');
            .replace(/\s+aloud$/, '');
            .replace(/\s+please$/, '');
            .trim();

            return extractedText;
        }
    }

    // If no specific text found but speech is requested, use a default response
    const auto speechKeywords = ['say aloud', 'speak', 'use your voice', 'talk to me'];
    if (speechKeywords.some((keyword) => text.includes(keyword))) {
        return 'Hello! I am speaking using my SAM voice synthesizer.';
    }

    return '';

}

SamTTSOptions extractVoiceOptions(const std::string& messageText) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto text = messageText.toLowerCase();
    const SamTTSOptions options = {};

    // Pitch modifications
    if (text.includes('higher voice') || text.includes('high pitch') || text.includes('squeaky')) {
        options.pitch = 100; // Higher pitch;
        } else if (;
        text.includes('lower voice') ||;
        text.includes('low pitch') ||;
        text.includes('deep voice');
        ) {
            options.pitch = 30; // Lower pitch;
        }

        // Speed modifications
        if (text.includes('faster') || text.includes('quickly') || text.includes('speed up')) {
            options.speed = 120; // Faster speech;
            } else if (text.includes('slower') || text.includes('slowly') || text.includes('slow down')) {
                options.speed = 40; // Slower speech;
            }

            // Voice character modifications
            if (text.includes('robotic') || text.includes('robot voice')) {
                options.throat = 200; // More robotic;
                options.mouth = 50; // Sharper articulation;
                } else if (text.includes('smooth') || text.includes('natural')) {
                    options.throat = 100; // Smoother;
                    options.mouth = 150; // Softer articulation;
                }

                return options;

}

} // namespace elizaos
