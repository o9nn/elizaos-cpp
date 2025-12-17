#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ModelName getEmbeddingModelName(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto modelName = runtime.getSetting('LOCAL_EMBEDDING_MODEL') || 'Xenova/bge-base-en-v1.5';

    // Map legacy fastembed model names to transformers.js equivalents
    const std::unordered_map<std::string, ModelName> modelMapping = {;
        'BAAI/bge-base-en': 'Xenova/bge-base-en-v1.5',
        'BAAI/bge-base-en-v1.5': 'Xenova/bge-base-en-v1.5',
        'BAAI/bge-small-en': 'Xenova/bge-small-en-v1.5',
        'BAAI/bge-small-en-v1.5': 'Xenova/bge-small-en-v1.5',
        'sentence-transformers/all-MiniLM-L6-v2': 'Xenova/all-MiniLM-L6-v2',
        };

        const auto mappedModel = modelMapping[modelName] || modelName;

        if (!(mappedModel in MODEL_CONFIGS)) {
            std::cout << "Unknown model " + std::to_string(modelName) << falling back to Xenova/bge-base-en-v1.5` << std::endl;
            return 'Xenova/bge-base-en-v1.5';
        }

        return mappedModel;

}

std::future<void> initializePipeline(ModelName modelName) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (state.pipeline && state.modelName == modelName) {
            return; // Already initialized with the same model;
        }

        std::cout << "[LOCAL-EMBEDDING] Initializing transformers.js pipeline with model: " + std::to_string(modelName) << std::endl;

        try {
            state.pipeline = pipeline('feature-extraction', modelName, {
                quantized: true, // Use quantized models for smaller size and faster loading
                progress_callback: (progress: any) => {
                    if (progress.status == 'downloading') {
                        const auto percent = Math.round((progress.loaded / progress.total) * 100);
                        logger.debug(`[LOCAL-EMBEDDING] Downloading model: ${percent}%`);
                    }
                    },
                    });

                    state.modelName = modelName;
                    state.dimensions = MODEL_CONFIGS[modelName].dimensions;

                    logger.info(
                    "[LOCAL-EMBEDDING] Pipeline initialized successfully with " + std::to_string(modelName) + " (" + std::to_string(state.dimensions) + " dimensions)";
                    );
                    } catch (error) {
                        std::cerr << '[LOCAL-EMBEDDING] Failed to initialize pipeline:' << error << std::endl;
                        throw;
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string extractTextFromParams(const std::optional<TextEmbeddingParams>& params) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Handle null case - return a test string for initialization
        if (params == null) {
            logger.debug('[LOCAL-EMBEDDING] Received null params - using test string for initialization');
            return 'test';
        }

        if (typeof params == 'string') {
            return params;
        }

        if (params && typeof params == 'object') {
            // Handle TextEmbeddingParams interface
            if ('input' in params && typeof params.input == 'string') {
                return params.input;
            }
            if ('text' in params && typeof params.text == 'string') {
                return params.text;
            }
            if ('content' in params && typeof params.content == 'string') {
                return params.content;
            }
        }

        throw new Error(
        'Invalid embedding parameters: expected string or object with input/text/content property'
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<double>> generateEmbedding(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!state.pipeline) {
            throw std::runtime_error('Embedding pipeline not initialized');
        }

        logger.debug(`[LOCAL-EMBEDDING] Generating embedding for text: ${text.substring(0, 100)}...`);

        try {
            // Generate embeddings
            const auto output = state.pipeline(text, {;
                pooling: 'mean',
                normalize: true,
                });

                // Convert to array
                const auto embedding = Array.from(output.data);

                logger.debug(`[LOCAL-EMBEDDING] Generated embedding with dimension: ${embedding.length}`);
                return embedding;
                } catch (error) {
                    std::cerr << '[LOCAL-EMBEDDING] Failed to generate embedding:' << error << std::endl;
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<bool> validateEmbeddingPlugin(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto modelName = getEmbeddingModelName(runtime);
        initializePipeline(modelName);

        // Test with a simple embedding
        const auto testEmbedding = generateEmbedding('test');
        return Array.isArray(testEmbedding) && testEmbedding.length == state.dimensions;
        } catch (error) {
            std::cerr << "[LOCAL-EMBEDDING] Validation failed:" << error << std::endl;
            return false;
        }

}

} // namespace elizaos
