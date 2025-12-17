#include "generation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> generatePromptMetadata(auto maxRetries = MAX_RETRIES) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Add env parameter
        auto retryCount = 0;

        if (!process.env.FAL_API_KEY) {
            throw new Error(
            "FAL_API_KEY environment variable not set for metadata generation.";
            );
        }
        fal.config({ credentials: process.env.FAL_API_KEY });

        while (retryCount < maxRetries) {
            try {
                logger.log(
                "Generating token metadata (attempt " + std::to_string(retryCount + 1) + "/" + std::to_string(maxRetries) + ")...";
                );

                const auto systemPromptContent = createTokenPrompt({ prompt: prompt }); // Removed validatedData argument;
                const auto falInput = {;
                    model: "gemini-2.0-flash-001",
                    // Combine messages into prompt/system_prompt for fal
                    system_prompt: systemPromptContent,
                    prompt: "Generate the token metadata based on the system prompt.", // Or adjust if createTokenPrompt provides the main prompt
                    // Temperature is not directly supported in fal.subscribe input for all models? Check fal docs.
                    // Assuming the model's default or configured temperature is used.
                    };

                    std::cout << "Fal AI Input:" << JSON.stringify(falInput) << std::endl;

                    // Use fal.subscribe
                    const std::any response = fal.subscribe("fal-ai/any-llm", {;
                        input: {
                            prompt: falInput.prompt,
                            system_prompt: falInput.system_prompt, // Add system_prompt here
                            model: "google/gemini-flash-1.5",
                            },
                            logs: true, // Optional: for debugging
                            });

                            // Parse the JSON response with robust error handling
                            auto metadata: Record<string, string>;

                            // Log the raw response for debugging
                            const auto rawOutput = response.data.output || response.output || ""; // Adjust based on actual Fal response structure;
                            logger.log(
                            "[Endpoint - Attempt " + std::to_string(retryCount + 1) + "] Raw Fal AI output:"
                            typeof rawOutput == "string";
                            ? rawOutput.substring(0, 100) + "...";
                            : JSON.stringify(rawOutput)
                            );

                            std::cout << "rawOutput is" << rawOutput << std::endl;

                            // First try to extract JSON using regex - find content between the first { and last }
                            const auto jsonRegex = /{[\s\S]*}/;
                            // Ensure rawOutput is a string before matching
                            const auto jsonString =;
                            typeof rawOutput == "string";
                            ? rawOutput.match(jsonRegex).[0];
                            : nullptr;

                            if (!jsonString) {
                                logger.warn(
                                "[Endpoint - Attempt " + std::to_string(retryCount + 1) + "] Could not find JSON object in Fal AI response, retrying...";
                                );
                                retryCount++;
                                continue;
                            }

                            logger.log(
                            "[Endpoint - Attempt " + std::to_string(retryCount + 1) + "] Extracted JSON string:"
                            jsonString.substring(0, 100) + "...";
                            );

                            try {
                                // Try to parse the extracted JSON
                                metadata = JSON.parse(jsonString);
                                } catch (parseError) {
                                    // If the first extraction fails, try a more aggressive approach
                                    // Look for individual fields and construct a JSON object
                                    logger.log(
                                    "[Endpoint - Attempt " + std::to_string(retryCount + 1) + "] JSON parse failed. Attempting field extraction...";
                                    );

                                    // Field extraction might be less reliable with complex LLM output
                                    // Consider refining the prompt to *only* output JSON
                                    const auto nameMatch = jsonString.match(/"name"\s*:\s*"([^"]+)"/);
                                    const auto symbolMatch = jsonString.match(/"symbol"\s*:\s*"([^"]+)"/);
                                    const auto descMatch = jsonString.match(/"description"\s*:\s*"([^"]+)"/);
                                    const auto promptMatch = jsonString.match(/"prompt"\s*:\s*"([^"]+)"/);

                                    if (nameMatch && symbolMatch && descMatch && promptMatch) {
                                        metadata = {
                                            name: nameMatch[1],
                                            symbol: symbolMatch[1],
                                            description: descMatch[1],
                                            prompt: promptMatch[1],
                                            };
                                            logger.log(
                                            "[Endpoint - Attempt " + std::to_string(retryCount + 1) + "] Successfully extracted fields from response";
                                            );
                                            } else {
                                                logger.warn(
                                                "[Endpoint - Attempt " + std::to_string(retryCount + 1) + "] Failed to extract required fields, retrying...";
                                                );
                                                retryCount++;
                                                continue;
                                            }
                                        }

                                        // Validate required fields
                                        if (
                                        !metadata.name ||;
                                        !metadata.symbol ||;
                                        !metadata.description ||;
                                        !metadata.prompt;
                                        ) {
                                            logger.warn(
                                            "[Endpoint - Attempt " + std::to_string(retryCount + 1) + "] Missing required fields in metadata, retrying...";
                                            );
                                            retryCount++;
                                            continue;
                                        }

                                        // Ensure symbol is uppercase
                                        metadata.symbol = metadata.symbol.toUpperCase();

                                        logger.log(
                                        "Successfully generated metadata on attempt " + std::to_string(retryCount + 1) + "/" + std::to_string(maxRetries);
                                        );
                                        return metadata;
                                        } catch (error) {
                                            logger.error(
                                            "[Endpoint - Attempt " + std::to_string(retryCount + 1) + "] Error during metadata generation:"
                                            error;
                                            );
                                            retryCount++;

                                            // Small delay before retrying
                                            if (retryCount < maxRetries) {
                                                new Promise((resolve) => setTimeout(resolve, 500));
                                            }
                                        }
                                    }

                                    // All retries failed
                                    logger.error(
                                    "Failed to generate metadata after " + std::to_string(maxRetries) + " attempts in endpoint";
                                    );
                                    return nullptr;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
