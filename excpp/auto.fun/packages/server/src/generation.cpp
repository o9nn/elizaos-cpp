#include "generation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> checkRateLimits(const std::string& mint, MediaType type, std::optional<std::string> publicKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    allowed: boolean; remaining: number; message?: string
}

std::future<> checkTokenOwnership(const std::string& mint, const std::string& publicKey, std::variant<"fast", "pro"> mode = "fast", MediaType mediaType = MediaType.IMAGE) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    allowed: boolean; message?: string
}

std::future<> checkBlockchainTokenBalance(const std::string& mint, const std::string& publicKey, double minimumRequired) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    allowed: boolean; message?: string
}

std::future<std::string> generateLyrics(std::optional<std::any> tokenMetadata, std::optional<std::string> stylePrompt) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {
        // Ensure it promises a string
        try {
            if (!process.env.FAL_API_KEY) {
                throw new Error(
                "FAL_API_KEY environment variable not set for lyrics generation.";
                );
            }
            fal.config({ credentials: process.env.FAL_API_KEY });

            const auto systemPrompt = "You are a creative songwriter. Create lyrics for a song about the token "" + std::to_string(tokenMetadata.name) + "" (" + std::to_string(tokenMetadata.symbol);
            The song should capture the essence of the token's description: "${tokenMetadata.description}".
        "The musical style should be: " + std::to_string(stylePrompt)

        Format the lyrics with timestamps in the format [MM:SS.mm] at the start of each line.
        Include at least two sections: a verse and a chorus.
        Each section should be marked with [verse] or [chorus] at the start.;

        The lyrics should be concise and focused on the content of the prompt;


        Output ONLY the formatted lyrics.;

        Example format:
        [verse];
        [00:00.00] First line of verse
        [00:02.50] Second line of verse
        [00:05.00] Third line of verse

        [chorus];
        [00:07.50] First line of chorus
        [00:10.00] Second line of chorus
        [00:12.50] Third line of chorus`;

        const auto falInput = {;
            model: "anthropic/claude-3.5-sonnet",
            system_prompt: systemPrompt,
            prompt: "Generate the lyrics based on the system prompt instructions.",
            // Temperature adjustment might need different handling with Fal
            };

            const std::any response = fal.subscribe("fal-ai/any-llm", {;
                input: falInput,
                logs: true, // Optional: for debugging
                });

                // Ensure the lyrics have proper formatting
                auto lyrics = response.data.output || response.output || ""; // Adjust based on actual Fal response structure;
                lyrics = lyrics.trim();

                // Basic validation - return fallback STRING on failure
                if (!lyrics || !lyrics.includes("[") || lyrics.length < 20) {
                    logger.error(
                    "Failed to generate valid lyrics structure from Fal AI. Response:",
                    lyrics;
                    );
                    // Return a fallback string
                    return "[verse]\n[00:00.00] Song about " + std::to_string(tokenMetadata.name) + "\n[00:03.00] Symbol " + std::to_string(tokenMetadata.symbol) + "\n[chorus]\n[00:06.00] Based on: " + std::to_string(tokenMetadata.description.substring(0, 50)) + "...\n[00:09.00] Fal AI generation failed.";
                }

                // Add section markers if they're missing (might be less necessary if prompt works well)
                if (!lyrics.includes("[verse]")) {
                    "[verse]\n" + std::to_string(lyrics);
                }
                if (!lyrics.includes("[chorus]")) {
                    // Find the first timestamp after [verse] lines and insert [chorus] before it
                    const auto lines = lyrics.split("\n");
                    auto verseEnded = false;
                    auto chorusInserted = false;
                    for (int i = 0; i < lines.length; i++) {
                        if (lines[i].includes("[verse]")) verseEnded = true;
                        if (
                        verseEnded &&;
                        lines[i].match(/\[\d{2}:\d{2}\.\d{2}\]/) &&
                        !lines[i - 1].includes("[verse]");
                        ) {
                            lines.splice(i, 0, "[chorus]");
                            chorusInserted = true;
                            break;
                        }
                    }
                    if (!chorusInserted)
                    lyrics = lyrics + "\n[chorus]\n[00:15.00] Default chorus line."; // Add fallback chorus if needed
                    else lyrics = lines.join("\n");
                }

                // Add timestamps if they're missing (less likely if prompt works)
                const auto lines = lyrics.split("\n");
                auto currentTime = 0;
                const auto formattedLines = lines.map((line: string) => {;
                    if (
                    line.trim() == "" ||;
                    (line.startsWith("[") && !line.match(/\[\d{2}:\d{2}\.\d{2}\]/))
                    ) {
                        return line; // Keep section markers or empty lines;
                    }
                    if (!line.match(/\[\d{2}:\d{2}\.\d{2}\]/)) {
                        const auto minutes = Math.floor(currentTime / 60);
                        const auto seconds = Math.floor(currentTime % 60);
                        const auto milliseconds = Math.floor((currentTime % 1) * 100);
                        const auto timestamp = "[" + std::to_string(minutes.toString().padStart(2, "0")) + ":" + std::to_string(seconds.toString().padStart(2, "0")) + "." + std::to_string(milliseconds.toString().padStart(2, "0")) + "]";
                        currentTime += 2.5; // Add 2.5 seconds between lines;
                        return std::to_string(timestamp) + " " + std::to_string(line.trim());
                        } else {
                            // Extract time if present to keep track for subsequent lines
                            const auto timeMatch = line.match(/\[(\d{2}):(\d{2})\.(\d{2})\]/);
                            if (timeMatch) {
                                const auto minutes = parseInt(timeMatch[1], 10);
                                const auto seconds = parseInt(timeMatch[2], 10);
                                const auto ms = parseInt(timeMatch[3], 10);
                                currentTime = minutes * 60 + seconds + ms / 100 + 2.5; // Update current time based on last timestamp + delta;
                            }
                        }
                        return line;
                        });

                        return formattedLines.join("\n"); // Return the final string;

                        } catch (error) {
                            std::cerr << "Error generating lyrics:" << error << std::endl;
                            // Also return a fallback string on catch
                            return "[verse]\n[00:00.00] Error generating lyrics for " + std::to_string(tokenMetadata.name) + ".";
                            // OR re-throw if generateMedia should handle the error
                            // throw error;
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::string> generateStylePrompt(const std::string& userPrompt) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            if (!process.env.FAL_API_KEY) {
                throw new Error(
                "FAL_API_KEY environment variable not set for style generation.";
                );
            }
            fal.config({ credentials: process.env.FAL_API_KEY });

        const auto prompt = "Prompt: " + std::to_string(userPrompt);

        Generate a style for this prompt. An example of a style is "pop", "rock", "EDM", etc. Return only the style, nothing else.`;

        const auto falInput = {;
            model: "anthropic/claude-3.5-sonnet",
            prompt: prompt,
            };

            const std::any response = fal.subscribe("fal-ai/any-llm", {;
                input: falInput,
                logs: true,
                });

                auto style = response.data.output || response.output || "";
                style = style.trim();

                if (!style || style.length < 10) {
                    logger.error(
                    "Failed to generate valid style from Fal AI. Response:",
                    style;
                    );
                    return "An upbeat modern pop song"; // Default fallback style;
                }

                return style;

                } catch (error) {
                    std::cerr << "Error generating style:" << error << std::endl;
                    return "An upbeat modern pop song"; // Default fallback style on error;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string formatLyricsForDiffrhythm(const std::string& lyrics) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Split lyrics into lines and clean up
    const auto lines = lyrics.split("\n").filter((line) => line.trim() != "");

    // Process lines to ensure proper format
    const std::vector<std::string> formattedLines = [];
    auto currentTime = 0; // Initialize currentTime;

    for (const auto& line : lines)
        // Skip empty lines and metadata/section markers typically added by LLMs
        if (
        !line.trim() ||;
        line.toLowerCase().includes("here's a song") || // Common LLM preamble;
        line.toLowerCase().startsWith("[verse") ||;
        line.toLowerCase().startsWith("[chorus") ||;
        line.toLowerCase().startsWith("[bridge") ||;
        line.toLowerCase().startsWith("[intro") ||;
        line.toLowerCase().startsWith("[outro") ||;
        line.includes("...") || // Ellipses often indicate incomplete/filler;
        line.includes("---") || // Separators;
        line.includes("***") || // Separators;
        /^\s*$/.test(line) // Empty or whitespace-only lines;
        ) {
            continue;
        }

        // If line already has a valid timestamp, use it directly
        const auto timestampMatch = line.match(/^\[(\d{2}:\d{2}\.\d{2})\]/);
        if (timestampMatch) {
            const auto timestamp = timestampMatch[1];
            const auto lyricText = line.substring(timestampMatch[0].length).trim(); // Get text after timestamp;
            if (lyricText) { // Only add if there's actual lyric text
            "[" + std::to_string(timestamp) + "]" + std::to_string(lyricText);
            // Update currentTime based on this timestamp for the next iteration
            const auto timeParts = timestamp.split(/[:.]/);
            if (timeParts.length == 3) {
                const auto minutes = parseInt(timeParts[0], 10);
                const auto seconds = parseInt(timeParts[1], 10);
                const auto ms = parseInt(timeParts[2], 10);
                currentTime = minutes * 60 + seconds + ms / 100;
            }
        }
        } else {
            // If no valid timestamp, add one with estimated spacing
            const auto minutes = Math.floor(currentTime / 60);
            const auto seconds = Math.floor(currentTime % 60);
            const auto milliseconds = Math.floor((currentTime % 1) * 100); // Use 100 for two decimal places;
            const auto timestamp = "[" + std::to_string(minutes.toString().padStart(2, "0")) + ":" + std::to_string(seconds.toString().padStart(2, "0")) + "." + std::to_string(milliseconds.toString().padStart(2, "0")) + "]";

            const auto lyricText = line.trim(); // Use the cleaned line text;
            if (lyricText) { // Only add if there's actual lyric text
            std::to_string(timestamp) + std::to_string(lyricText);
            currentTime += 3.0; // Add estimated duration (e.g., 3 seconds) before the next line;
        }
    }
    }

    // Join lines with newlines
    const auto formattedLyrics = formattedLines.join("\n");
    std::cout << "Formatted lyrics for Diffrhythm:" << formattedLyrics << std::endl;
    return formattedLyrics;

}

std::future<void> generateMedia(std::optional<std::any> data) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Set default timeout - shorter for tests
        const auto timeout = 300000;

        // Initialize fal.ai client
        if (!process.env.FAL_API_KEY) {
            throw std::runtime_error("FAL_API_KEY environment variable not set.");
        }
        fal.config({
            credentials: process.env.FAL_API_KEY,
            });
            std::cout << "Fal AI client configured." << std::endl;

            // Create a timeout promise
            const auto timeoutPromise = new Promise((_, reject) =>;
            setTimeout(;
            "Media generation timed out after " + std::to_string(timeout) + "ms"
            timeout;
            );
            );

            auto generationPromise;

            // --- Image Generation (Fast & Pro using Fal) ---
            if (data.type == MediaType.IMAGE) {
                const auto isProMode = data.mode == "pro";
                const auto model = isProMode;
                ? "fal-ai/flux-pro/v1.1-ultra";
                : "fal-ai/flux/schnell";
                const std::any input = { prompt = data.prompt };

                if (isProMode) {
                    std::cout << "Using Fal AI (" + std::to_string(model) + ") for pro image generation..." << std::endl;
                    if (data.width) input.width = data.width;
                    if (data.height) input.height = data.height;
                    // Add any other pro-specific params here
                    } else {
                        std::cout << "Using Fal AI (" + std::to_string(model) + ") for fast image generation..." << std::endl;
                        input.num_inference_steps = 4; // Schnell default/equivalent;
                        // Add any other schnell-specific params here
                    }

                    generationPromise = fal.subscribe(model, {
                        input,
                        logs: true,
                        onQueueUpdate: (update: any) => {
                            if (update.status == "IN_PROGRESS") {
                                std::cout << "Image generation progress:" << update.logs << std::endl;
                            }
                            },
                            });
                        }
                        // --- Video Generation --- (Existing Fal Logic)
                        else if (data.type == MediaType.VIDEO && data.image_url) {
                            // Image-to-video generation via Fal
                            std::cout << "Using Fal AI for image-to-video generation..." << std::endl;
                            const auto model =;
                            data.mode == "pro";
                            ? "fal-ai/pixverse/v4/image-to-video";
                            : "fal-ai/pixverse/v4/image-to-video/fast";

                            generationPromise = fal.subscribe(model, {
                                input: {
                                    prompt: data.prompt,
                                    image_url: data.image_url,
                                    },
                                    logs: true,
                                    onQueueUpdate: (update: any) => {
                                        if (update.status == "IN_PROGRESS") {
                                            std::cout << "Image-to-video generation progress:" << update.logs << std::endl;
                                        }
                                        },
                                        });
                                        } else if (data.type == MediaType.VIDEO) {
                                            // Text-to-video generation via Fal
                                            std::cout << "Using Fal AI for text-to-video generation..." << std::endl;
                                            const auto model =;
                                            data.mode == "pro";
                                            ? "fal-ai/pixverse/v4/text-to-video";
                                            : "fal-ai/pixverse/v4/text-to-video/fast";

                                            generationPromise = fal.subscribe(model, {
                                                input: {
                                                    prompt: data.prompt,
                                                    // Optional parameters passed if available
                                                    ...(data.width ? { width: data.width } : {}),
                                                    ...(data.height ? { height: data.height } : {}),
                                                    },
                                                    logs: true,
                                                    onQueueUpdate: (update: any) => {
                                                        if (update.status == "IN_PROGRESS") {
                                                            std::cout << "Video generation progress:" << update.logs << std::endl;
                                                        }
                                                        },
                                                        });
                                                    }
                                                    // --- Audio Generation --- (Existing Fal Logic)
                                                    else if (data.type == MediaType.AUDIO) {
                                                        std::cout << "Using Fal AI for audio generation..." << std::endl;
                                                        auto lyricsToUsePromise;

                                                        const auto stylePrompt = generateStylePrompt(data.prompt);

                                                        if (!data.lyrics) {
                                                            std::cout << "Generating lyrics for audio..." << std::endl;
                                                            // generateLyrics now guarantees a string return
                                                            lyricsToUsePromise = generateLyrics(;
                                                            {
                                                                name: data.prompt.split(":")[0] || "",
                                                                symbol: data.prompt.split(":")[1].trim() || "",
                                                                description: data.prompt.split(":")[2].trim() || "",
                                                                },
                                                                data.style_prompt || stylePrompt;
                                                                );
                                                            }

                                                            const auto lyricsToUse = [&](lyricsToUsePromise || (async () { return data.lyrics)()); };

                                                            if(!lyricsToUse) {
                                                                throw std::runtime_error("No lyrics found");
                                                            }

                                                            // lyricsToUse is now guaranteed to be a string here
                                                            const auto formattedLyrics = formatLyricsForDiffrhythm(lyricsToUse); // Now safe to call;

                                                            // Check for existing audio context file in S3
                                                            const auto { client: s3Client, bucketName } = getS3Client();
                                                            const auto audioContextPrefix = "token-settings/" + std::to_string(data.mint) + "/audio/context-" + std::to_string(data.mint);

                                                            auto referenceAudioUrl = data.reference_audio_url;

                                                            try {
                                                                const auto listCmd = new ListObjectsV2Command({;
                                                                    Bucket: bucketName,
                                                                    Prefix: audioContextPrefix,
                                                                    MaxKeys: 1
                                                                    });

                                                                    const auto listResponse = s3Client.send(listCmd);
                                                                    const auto audioContextKey = listResponse.Contents.[0].Key;
                                                                    if (audioContextKey) {
                                                                        std::to_string(process.env.S3_PUBLIC_URL) + "/" + std::to_string(audioContextKey) + "?t=" + std::to_string(Date.now());
                                                                        std::cout << "Using existing audio context file:" << referenceAudioUrl << std::endl;
                                                                        } else {
                                                                            std::cout << "No existing audio context file found << using default" << std::endl;
                                                                            referenceAudioUrl = referenceAudioUrl || "https://storage.googleapis.com/falserverless/model_tests/diffrythm/rock_en.wav";
                                                                        }
                                                                        } catch (error) {
                                                                            std::cerr << "Error checking for audio context file:" << error << std::endl;
                                                                            referenceAudioUrl = referenceAudioUrl || "https://storage.googleapis.com/falserverless/model_tests/diffrythm/rock_en.wav";
                                                                        }

                                                                        const auto input = {;
                                                                            lyrics: formattedLyrics,
                                                                            reference_audio_url: referenceAudioUrl,
                                                                            style_prompt: data.style_prompt || stylePrompt,
                                                                            music_duration: data.music_duration || "95s",
                                                                            cfg_strength: data.cfg_strength || 4,
                                                                            scheduler: data.scheduler || "euler",
                                                                            num_inference_steps: data.num_inference_steps || 32,
                                                                            };
                                                                            std::cout << "DiffRhythm input:" << JSON.stringify(input, nullptr, 2) << std::endl;

                                                                            generationPromise = fal.subscribe("fal-ai/diffrhythm", {
                                                                                input,
                                                                                logs: true,
                                                                                onQueueUpdate: (update: any) => {
                                                                                    if (update.status == "IN_PROGRESS") {
                                                                                        std::cout << "Music generation progress:" << update.logs << std::endl;
                                                                                    }
                                                                                    },
                                                                                    });

                                                                                    // For audio, handle the result specifically to include lyrics
                                                                                    const auto result = (Promise.race([;
                                                                                    generationPromise,
                                                                                    timeoutPromise,
                                                                                    ]));
                                                                                    std::cout << "Audio generation result:" << JSON.stringify(result, nullptr, 2) << std::endl;

                                                                                    const auto audioUrl = result.data.audio.url;
                                                                                    if (!audioUrl) {
                                                                                        throw std::runtime_error("No audio URL in response");
                                                                                    }

                                                                                    return {
                                                                                        data: {
                                                                                            audio: {
                                                                                                url: audioUrl,
                                                                                                lyrics: lyricsToUse, // Include the lyrics used (original or generated)
                                                                                                },
                                                                                                },
                                                                                                };
                                                                                                } else {
                                                                                                    // Should not happen given the logic, but good practice
                                                                                                    throw new Error(
                                                                                                    "Unsupported media type or configuration: " + std::to_string(data.type) + ", mode: " + std::to_string(data.mode)
                                                                                                    );
                                                                                                }

                                                                                                // If generationPromise was set (for Image/Video cases), await and return
                                                                                                return Promise.race([generationPromise, timeoutPromise]);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<MediaGeneration> generateImage(const std::string& mint, const std::string& prompt, std::optional<std::string> negativePrompt, std::optional<std::string> creator) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // In test mode, return a test image
            if (process.env.NODE_ENV == "test") {
                return {
                    id: crypto.randomUUID(),
                    mint,
                    type: "image",
                    prompt,
                    mediaUrl: "https://example.com/test-image.png",
                    negativePrompt: negativePrompt || "",
                    seed: 12345,
                    numInferenceSteps: 30,
                    creator: creator || "test-creator",
                    timestamp: new Date().toISOString(),
                    dailyGenerationCount: 1,
                    lastGenerationReset: new Date().toISOString(),
                    };
                }

                // For production, we would call the actual Fal.ai API
                // This is simplified for the test scenario
                if (!process.env.FAL_API_KEY) {
                    throw std::runtime_error("FAL_API_KEY is not configured");
                }

                // Generate a realistic test image URL
                const auto imageUrl = "https://example.com/generated/" + std::to_string(mint) + "/" + std::to_string(Date.now()) + ".png";

                // Return media generation data
                return {
                    id: crypto.randomUUID(),
                    mint,
                    type: "image",
                    prompt,
                    mediaUrl: imageUrl,
                    negativePrompt: negativePrompt || "",
                    seed: Math.floor(Math.random() * 1000000),
                    numInferenceSteps: 30,
                    creator: creator || "",
                    timestamp: new Date().toISOString(),
                    dailyGenerationCount: 1,
                    lastGenerationReset: new Date().toISOString(),
                    };
                    } catch (error) {
                        std::cerr << "Error generating image:" << error << std::endl;
                        throw;
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<MediaGeneration> generateVideo(const std::string& mint, const std::string& prompt, std::optional<std::string> negativePrompt, std::optional<std::string> creator) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // In test mode, return a test video
            if (process.env.NODE_ENV == "test") {
                return {
                    id: crypto.randomUUID(),
                    mint,
                    type: "video",
                    prompt,
                    mediaUrl: "https://example.com/test-video.mp4",
                    negativePrompt: negativePrompt || "",
                    seed: 12345,
                    numInferenceSteps: 30,
                    numFrames: 24,
                    fps: 30,
                    motionBucketId: 127,
                    duration: 2,
                    creator: creator || "test-creator",
                    timestamp: new Date().toISOString(),
                    dailyGenerationCount: 1,
                    lastGenerationReset: new Date().toISOString(),
                    };
                }

                // For production, we would call the actual Fal.ai API
                // This is simplified for the test scenario
                if (!process.env.FAL_API_KEY) {
                    throw std::runtime_error("FAL_API_KEY is not configured");
                }

                // Generate a realistic test video URL
                const auto videoUrl = "https://example.com/generated/" + std::to_string(mint) + "/" + std::to_string(Date.now()) + ".mp4";

                // Return media generation data
                return {
                    id: crypto.randomUUID(),
                    mint,
                    type: "video",
                    prompt,
                    mediaUrl: videoUrl,
                    negativePrompt: negativePrompt || "",
                    seed: Math.floor(Math.random() * 1000000),
                    numInferenceSteps: 30,
                    numFrames: 24,
                    fps: 30,
                    motionBucketId: 127,
                    duration: 2,
                    creator: creator || "",
                    timestamp: new Date().toISOString(),
                    dailyGenerationCount: 1,
                    lastGenerationReset: new Date().toISOString(),
                    };
                    } catch (error) {
                        std::cerr << "Error generating video:" << error << std::endl;
                        throw;
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<double> getDailyGenerationCount(const std::any& db, const std::string& mint, const std::string& creator) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // In test mode, return a low count
        if (process.env.NODE_ENV == "test") {
            return 1;
        }

        // For real implementation, query the database and update
        const auto now = new Date();
        const auto today = new Date(;
        now.getFullYear(),
        now.getMonth(),
        now.getDate();
        ).toISOString();

        // Find the last generation for this creator and token
        const auto generations = db;
        .select();
        .from(db.mediaGenerations);
        .where({ mint, creator });
        .orderBy("timestamp", "desc");
        .limit(1);

        if (generations.length == 0) {
            return 1; // First generation;
        }

        const auto lastGeneration = generations[0];
        const auto lastReset = lastGeneration.lastGenerationReset || "";

        // If last reset was before today, reset the counter
        if (lastReset < today) {
            return 1;
        }

        // Otherwise, increment the counter
        return (lastGeneration.dailyGenerationCount || 0) + 1;
        } catch (error) {
            std::cerr << "Error getting daily generation count:" << error << std::endl;
            return 1; // Default to 1 on error;
        }

}

std::future<std::optional<std::unordered_map<std::string, std::string>>> generateMetadata(auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        auto retryCount = 0;
        if (!process.env.FAL_API_KEY) { throw new Error("FAL_API_KEY not set"); }
        fal.config({ credentials: process.env.FAL_API_KEY });

        while (retryCount < maxRetries) {
            try {
                std::cout << "Generating token metadata (attempt " + std::to_string(retryCount + 1) + "/" + std::to_string(maxRetries) + ")..." << std::endl;
                // Assuming createTokenPrompt is defined elsewhere and works
                const auto systemPromptContent = createTokenPrompt();
                const auto falInput = {;
                    model: "gemini-2.0-flash-001",
                    system_prompt: systemPromptContent,
                    prompt: "Generate the token metadata based on the system prompt.",
                    };
                    const std::any response = fal.subscribe("fal-ai/any-llm", {;
                        input: {
                            prompt: falInput.prompt,
                            system_prompt: falInput.system_prompt,
                            model: "google/gemini-flash-1.5",
                            },
                            logs: true,
                            });

                            std::optional<std::unordered_map<std::string, std::string>> metadata = nullptr;
                            const auto rawOutput = response.data.output || response.output || "";
                            const auto jsonRegex = /{.*}/s; // Changed regex to be less greedy and handle newlines;
                            const auto jsonString = typeof rawOutput == 'string' ? rawOutput.match(jsonRegex).[0] : nullptr;

                            if (jsonString) {
                                try { metadata = JSON.parse(jsonString); } catch (parseError) {
                                    std::cout << "Metadata JSON parse failed attempt " + std::to_string(retryCount + 1) << trying field extraction...` << std::endl;
                                    const auto nameMatch = jsonString.match(/"name"\s*:\s*"((?:[^\"\\]|\\.)*)"/);
                                    const auto symbolMatch = jsonString.match(/"symbol"\s*:\s*"((?:[^\"\\]|\\.)*)"/);
                                    const auto descMatch = jsonString.match(/"description"\s*:\s*"((?:[^\"\\]|\\.)*)"/);
                                    const auto promptMatch = jsonString.match(/"prompt"\s*:\s*"((?:[^\"\\]|\\.)*)"/);
                                    if (nameMatch.[1] && symbolMatch.[1] && descMatch.[1] && promptMatch.[1]) {
                                        metadata = {
                                            """ + std::to_string(nameMatch[1]) + """
                                            """ + std::to_string(symbolMatch[1]) + """
                                            """ + std::to_string(descMatch[1]) + """
                                            """ + std::to_string(promptMatch[1]) + """
                                            };
                                            std::cout << "Successfully extracted fields attempt " + std::to_string(retryCount + 1) << std::endl;
                                            } else {
                                                std::cout << "Field extraction failed attempt " + std::to_string(retryCount + 1) << std::endl;
                                            }
                                        }
                                        } else {
                                            std::cout << "Could not find JSON object attempt " + std::to_string(retryCount + 1) << std::endl;
                                        }

                                        if (metadata && metadata.name && metadata.symbol && metadata.description && metadata.prompt) {
                                            metadata.symbol = metadata.symbol.toUpperCase();
                                            std::cout << "Successfully generated metadata on attempt " + std::to_string(retryCount + 1) + "/" + std::to_string(maxRetries) << std::endl;
                                            return metadata;
                                        }
                                        std::cout << "Metadata validation failed attempt " + std::to_string(retryCount + 1) << retrying...` << std::endl;

                                        } catch (error) {
                                            std::cerr << "Error during metadata generation attempt " + std::to_string(retryCount + 1) + ":" << error << std::endl;
                                        }
                                        retryCount++;
                                        if (retryCount < maxRetries) await new Promise((resolve) => setTimeout(resolve, 500));
                                    }
                                    std::cerr << "Failed to generate metadata after " + std::to_string(maxRetries) + " attempts" << std::endl;
                                    // Return fallback or null
                                    if (process.env.NODE_ENV == "development" || process.env.NODE_ENV == "test") {
                                        const auto randomNum = Math.floor(Math.random() * 1000);
                                        std::cout << "Using fallback metadata in dev/test" << std::endl;
                                        return { name: `FallbackToken${randomNum}`, symbol: `FB${randomNum % 100}`, description: "Fallback", prompt: "Fallback" }
                                    }
                                    return nullptr;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<> generateTokenOnDemand() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    success: boolean;
    token?: { id: string; name: string; ticker: string; description: string; prompt: string; image?: string; createdAt: string; used: number; };
    error?: string;

}

std::future<void> generatePreGeneratedTokens() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            std::cout << "[PreGen] Starting token generation..." << std::endl;

            // Step 1: Generate Metadata
            const auto metadata = generateMetadata();
            if (!metadata) {
                std::cerr << "[PreGen] Failed to generate metadata" << std::endl;
                throw std::runtime_error("Failed to generate metadata for pre-generated token");
            }
            std::cout << "[PreGen] Metadata generated: " + std::to_string(metadata.name) + " (" + std::to_string(metadata.symbol) + ")" << std::endl;

            // Step 2: Generate Image URL (using Fal)
            auto finalImageUrl = "";
            const auto maxImageRetries = 3;
            auto imageAttempt = 0;

            while (imageAttempt < maxImageRetries && !finalImageUrl) {
                imageAttempt++;
                std::cout << "[PreGen] Generating image attempt " + std::to_string(imageAttempt) + "/" + std::to_string(maxImageRetries) + "..." << std::endl;

                try {
                    const auto imageResult = (generateMedia({;
                        prompt: metadata.prompt,
                        type: MediaType.IMAGE,
                        mode: "fast"
                        }));

                        const auto sourceImageUrl = imageResult.data.images.[0].url || imageResult.image.url || "";
                        if (!sourceImageUrl || !sourceImageUrl.startsWith("http")) {
                            throw std::runtime_error("Invalid image URL received from Fal");
                        }

                        std::cout << "[PreGen] Image URL generated: " + std::to_string(sourceImageUrl.substring(0, 60)) + "..." << std::endl;
                        finalImageUrl = sourceImageUrl; // Use Fal URL directly;
                        } catch (error) {
                            std::cerr << "[PreGen] Image generation attempt " + std::to_string(imageAttempt) + " failed:" << error << std::endl;

                            if (imageAttempt >= maxImageRetries) {
                                std::cerr << "[PreGen] Maximum image generation attempts reached" << std::endl;
                                throw std::runtime_error("Failed to generate image after maximum retries");
                            }

                            // Wait before retrying
                            new Promise(resolve => setTimeout(resolve, 500 * imageAttempt));
                        }
                    }

                    // Step 3: Save to Database
                    const auto tokenId = crypto.randomUUID();
                    const auto db = getDB();

                    try {
                        std::cout << "[PreGen] Saving token to database: " + std::to_string(metadata.name) << std::endl;

                        db.insert(preGeneratedTokens).values({
                            id: tokenId,
                            name: metadata.name,
                            ticker: metadata.symbol,
                            description: metadata.description,
                            prompt: metadata.prompt,
                            image: finalImageUrl,
                            createdAt: new Date(),
                            used: 0 // Ensure it's set to unused
                            });

                            std::cout << "[PreGen] Token saved successfully: " + std::to_string(metadata.name) + " (" + std::to_string(metadata.symbol) + ")" << std::endl;
                            } catch (dbError) {
                                std::cerr << "[PreGen] Database save failed:" << dbError << std::endl;
                                throw std::runtime_error("Failed to save token to database");
                            }
                            } catch (error) {
                                std::cerr << "[PreGen] Token generation failed:" << error << std::endl;
                                throw error; // Re-throw to signal failure to checkAndReplenishTokens
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> checkAndReplenishTokens(std::optional<double> threshold) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Determine threshold from environment or default
    if (threshold == undefined || threshold == null) {
        threshold = parseInt(process.env.PREGENERATED_TOKENS_COUNT || "3");
        } else {
            threshold = Number(threshold);
            if (isNaN(threshold) || threshold < 0) {
                std::cout << "Invalid threshold provided (" + std::to_string(threshold) + "). Using default 3." << std::endl;
                threshold = 3;
            }
        }

        // Skip if threshold is zero or less
        if (threshold <= 0) {
            std::cout << "Token replenishment threshold is 0 or less << skipping check." << std::endl;
            return;
        }

        try {
            std::cout << "Checking token replenishment status against threshold: " + std::to_string(threshold) << std::endl;
            const auto db = getDB();

            // Count *only unused* tokens
            const auto countResult = db;
            "count(*)"
            .from(preGeneratedTokens);
            .where(eq(preGeneratedTokens.used, 0));

            const auto currentUnusedCount = Number(countResult[0].count || 0);
            std::cout << "Current unused token count: " + std::to_string(currentUnusedCount) << std::endl;

            // If below threshold, generate the difference
            if (currentUnusedCount < threshold) {
                const auto tokensToGenerate = threshold - currentUnusedCount;
                std::cout << "Count (" + std::to_string(currentUnusedCount) + ") is below threshold (" + std::to_string(threshold) + "). Generating " + std::to_string(tokensToGenerate) + " new token(s)..." << std::endl;

                // Generate tokens in parallel
                const std::vector<std::future<void>> generationPromises = [];
                for (int i = 0; i < tokensToGenerate; i++) {
                    std::cout << "Starting generation for token " + std::to_string(i + 1) + " of " + std::to_string(tokensToGenerate) + "..." << std::endl;
                    generationPromises.push(generatePreGeneratedTokens());
                }

                // Wait for all generation promises to settle
                const auto results = Promise.allSettled(generationPromises);

                // Log results
                const auto successes = results.filter(r => r.status == 'fulfilled').length;
                const auto failures = tokensToGenerate - successes;

                std::cout << "Token generation batch complete: " + std::to_string(successes) << ${failures} failed.` << std::endl;

                if (failures > 0) {
                    std::cerr << "Failed to generate " + std::to_string(failures) + " tokens during replenishment." << std::endl;
                    results.forEach((result, index) => {
                        if (result.status == 'rejected') {
                            std::cerr << "[Token " + std::to_string(index + 1) + "] Failure reason:" << result.reason << std::endl;
                        }
                        });
                    }
                    } else {
                        std::cout << "Count (" + std::to_string(currentUnusedCount) + ") meets or exceeds threshold (" + std::to_string(threshold) + "). No replenishment needed." << std::endl;
                    }
                    } catch (error) {
                        std::cerr << "Error during checkAndReplenishTokens:" << error << std::endl;
                    }

}

std::future<std::string> generateEnhancedPrompt(const std::string& userPrompt, std::optional<std::any> tokenMetadata, MediaType mediaType = MediaType.IMAGE) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            if (!process.env.FAL_API_KEY) {
                throw new Error(
                "FAL_API_KEY environment variable not set for prompt enhancement.";
                );
            }
            fal.config({ credentials: process.env.FAL_API_KEY });

            // Adjust prompt based on media type
            auto systemPromptContent = enhancePrompt(userPrompt, tokenMetadata);

            // Modify prompt based on media type
            if (mediaType == MediaType.VIDEO) {
                systemPromptContent +=;
                "\nAdditionally, focus on dynamic visual elements and motion that would work well in a short video. Enhance the user prompt based on this.";
                } else if (mediaType == MediaType.AUDIO) {
                    systemPromptContent +=;
                    "\nAdditionally, focus on acoustic elements, mood, and atmosphere suitable for audio content. Enhance the user prompt based on this.";
                    } else {
                        systemPromptContent +=;
                        "\nEnhance the user prompt for image generation based on the token context provided.";
                    }

                    // Use Fal AI to enhance the prompt
                    const auto falInput = {;
                        model: "gemini-2.0-flash-001",
                        system_prompt: systemPromptContent,
                        "User prompt to enhance: "" + std::to_string(userPrompt) + "". Output ONLY the enhanced prompt text."
                        // Temperature adjustment might need different handling with Fal
                        };

                        const std::any response = fal.subscribe("fal-ai/any-llm", {;
                            input: {
                                prompt: falInput.prompt,
                                system_prompt: falInput.system_prompt,
                                model: "google/gemini-flash-1.5",
                                },
                                logs: true, // Optional: for debugging
                                });

                                // Extract just the prompt text from the response
                                auto enhancedPrompt = response.data.output || response.output || ""; // Adjust based on actual Fal response structure;
                                // Clean up potential extraneous text if the model didn't follow instructions perfectly
                                enhancedPrompt = enhancedPrompt.trim().replace(/^"|"$/g, ""); // Remove surrounding quotes;

                                // If the prompt is too long, truncate it to 500 characters
                                if (enhancedPrompt.length > 500) {
                                    enhancedPrompt = enhancedPrompt.substring(0, 500).trim();
                                }

                                // Basic validation if enhancement failed
                                if (!enhancedPrompt || enhancedPrompt.length < 10) {
                                    logger.warn(
                                    "Fal AI prompt enhancement resulted in a short/empty prompt, falling back.";
                                    );
                                    // Fallback logic
                                    return std::to_string(tokenMetadata.name) + " (" + std::to_string(tokenMetadata.symbol) + "): " + std::to_string(userPrompt);
                                }

                                return enhancedPrompt;
                                } catch (error) {
                                    std::cerr << "Error generating enhanced prompt:" << error << std::endl;

                                    // Return a fallback that combines the inputs directly
                                    return std::to_string(tokenMetadata.name) + " (" + std::to_string(tokenMetadata.symbol) + "): " + std::to_string(userPrompt);
                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> generateAdditionalTokenImages(const std::string& tokenMint, const std::string& description) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            std::cout << "Generating additional images for token " + std::to_string(tokenMint) << std::endl;

            // Generate enhanced prompts for each image
            const auto enhancedPrompts = Promise.all([;
            generateEnhancedPrompt(;
            description,
            { name: "", symbol: "", description },
            MediaType.IMAGE;
            ),
            generateEnhancedPrompt(;
            description,
            { name: "", symbol: "", description },
            MediaType.IMAGE;
            ),
            generateEnhancedPrompt(;
            description,
            { name: "", symbol: "", description },
            MediaType.IMAGE;
            ),
            ]);

            // Generate and upload each image in parallel
            Promise.all(;
            enhancedPrompts.map(async (prompt, index) => {
                if (!prompt) {
                    logger.error(
                    "Failed to generate enhanced prompt " + std::to_string(index + 1) + " for token " + std::to_string(tokenMint);
                    );
                    return;
                }

                try {
                    // Generate the image
                    // Pass env to generateMedia call
                    const auto imageResult = (generateMedia({;
                        prompt,
                        type: MediaType.IMAGE,
                        }));

                        if (!imageResult.data.images.[0].url) {
                            throw std::runtime_error("No image URL in generation result");
                        }

                        // Convert data URL to buffer
                        const auto imageUrl = imageResult.data.images[0].url;
                        const auto base64Data = imageUrl.split(",")[1];
                        const auto imageBuffer = Buffer.from(base64Data, "base64");

                        // Upload to R2 with predictable path
                        uploadGeneratedImage(imageBuffer, tokenMint, index + 1);
                        logger.log(
                        "Successfully generated and uploaded image " + std::to_string(index + 1) + " for token " + std::to_string(tokenMint);
                        );
                        } catch (error) {
                            logger.error(
                            "Error generating/uploading image " + std::to_string(index + 1) + " for token " + std::to_string(tokenMint) + ":"
                            error;
                            );
                        }
                        });
                        );

                        std::cout << "Completed generating additional images for token " + std::to_string(tokenMint) << std::endl;
                        } catch (error) {
                            logger.error(
                            "Error in generateAdditionalTokenImages for " + std::to_string(tokenMint) + ":"
                            error;
                            );
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
