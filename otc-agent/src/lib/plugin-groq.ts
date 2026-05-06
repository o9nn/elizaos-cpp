import { createGroq } from "@ai-sdk/groq";
import type {
  ModelTypeName,
  ObjectGenerationParams,
  Plugin,
} from "@elizaos/core";
import {
  type DetokenizeTextParams,
  type GenerateTextParams,
  ModelType,
  type TokenizeTextParams,
  logger,
} from "@elizaos/core";

/**
 * Retrieves the Groq API base URL, using runtime settings or environment variables if available.
 *
 * @returns The resolved Groq API base URL.
 */
function getBaseURL(runtime: any): string {
  return (
    runtime.getSetting("GROQ_BASE_URL") || "https://api.groq.com/openai/v1"
  );
}
import { generateObject, generateText } from "ai";
import { type TiktokenModel, encodingForModel } from "js-tiktoken";

// Removed unused Runtime interface

/**
 * Returns the appropriate Groq model name string for the specified model type.
 *
 * If environment variables for model names are set, they are used; otherwise, defaults are returned.
 *
 * @param model - The model type for which to retrieve the model name.
 * @returns The model name string corresponding to the given {@link model}.
 *
 * @remark If an error occurs, returns the default model name 'llama-3.1-8b-instant'.
 */
function findModelName(model: ModelTypeName): TiktokenModel {
  const name =
    model === ModelType.TEXT_SMALL
      ? (process.env.SMALL_GROQ_MODEL ?? "llama-3.1-8b-instant")
      : (process.env.LARGE_GROQ_MODEL ?? "llama-3.3-70b-versatile");
  return name as TiktokenModel;
}

async function tokenizeText(model: ModelTypeName, prompt: string) {
  const encoding = encodingForModel(findModelName(model));
  const tokens = encoding.encode(prompt);
  return tokens;
}

/**
 * Detokenize a sequence of tokens back into text using the specified model.
 *
 * @param {ModelTypeName} model - The type of model to use for detokenization.
 * @param {number[]} tokens - The sequence of tokens to detokenize.
 * @returns {string} The detokenized text.
 */
async function detokenizeText(model: ModelTypeName, tokens: number[]) {
  const modelName = findModelName(model);
  const encoding = encodingForModel(modelName);
  return encoding.decode(tokens);
}

/**
 * Generate text using Groq API
 */
async function generateGroqText(
  groq: ReturnType<typeof createGroq>,
  model: string,
  params: {
    prompt: string;
    system?: string;
    temperature: number;
    maxTokens: number;
    frequencyPenalty: number;
    presencePenalty: number;
    stopSequences: string[];
  },
) {
  const { text: groqResponse } = await generateText({
    model: groq.languageModel(model),
    prompt: params.prompt,
    system: params.system,
    temperature: params.temperature,
    maxOutputTokens: params.maxTokens,
    frequencyPenalty: params.frequencyPenalty,
    presencePenalty: params.presencePenalty,
    stopSequences: params.stopSequences,
  });
  return groqResponse;
}

/**
 * Generate object using Groq API
 */
async function generateGroqObject(
  groq: ReturnType<typeof createGroq>,
  model: string,
  params: ObjectGenerationParams,
) {
  const { object } = await generateObject({
    model: groq.languageModel(model),
    output: "no-schema",
    prompt: params.prompt,
    temperature: params.temperature,
  });
  return object;
}

export const groqPlugin: Plugin = {
  name: "groq",
  description: "Groq plugin",
  config: {
    GROQ_API_KEY: process.env.GROQ_API_KEY,
    SMALL_GROQ_MODEL: process.env.SMALL_GROQ_MODEL,
    MEDIUM_GROQ_MODEL: process.env.MEDIUM_GROQ_MODEL,
    LARGE_GROQ_MODEL: process.env.LARGE_GROQ_MODEL,
  },
  async init() {
    if (!process.env.GROQ_API_KEY) {
      throw Error("Missing GROQ_API_KEY in environment variables");
    }
  },
  models: {
    [ModelType.TEXT_TOKENIZER_ENCODE]: async (
      _runtime,
      { prompt, modelType = ModelType.TEXT_LARGE }: TokenizeTextParams,
    ) => {
      return await tokenizeText(modelType ?? ModelType.TEXT_LARGE, prompt);
    },
    [ModelType.TEXT_TOKENIZER_DECODE]: async (
      _runtime,
      { tokens, modelType = ModelType.TEXT_LARGE }: DetokenizeTextParams,
    ) => {
      return await detokenizeText(modelType ?? ModelType.TEXT_LARGE, tokens);
    },
    [ModelType.TEXT_SMALL]: async (
      runtime,
      { prompt, stopSequences = [] }: GenerateTextParams,
    ) => {
      const temperature = 0.7;
      const frequency_penalty = 0.7;
      const presence_penalty = 0.7;
      const max_response_length = 8000;
      const baseURL = getBaseURL(runtime);
      const groq = createGroq({
        apiKey: runtime.getSetting("GROQ_API_KEY"),
        fetch: runtime.fetch,
        baseURL,
      });

      const model =
        runtime.getSetting("GROQ_SMALL_MODEL") ??
        runtime.getSetting("SMALL_MODEL") ??
        "llama-3.1-8b-instant";

      logger.log("generating text");
      logger.log(prompt);

      return await generateGroqText(groq, model, {
        prompt,
        system: runtime.character.system ?? undefined,
        temperature,
        maxTokens: max_response_length,
        frequencyPenalty: frequency_penalty,
        presencePenalty: presence_penalty,
        stopSequences,
      });
    },
    [ModelType.TEXT_LARGE]: async (
      runtime,
      {
        prompt,
        stopSequences = [],
        maxTokens = 8192,
        temperature = 0.7,
        frequencyPenalty = 0.7,
        presencePenalty = 0.7,
      }: GenerateTextParams,
    ) => {
      const model =
        runtime.getSetting("GROQ_LARGE_MODEL") ??
        runtime.getSetting("LARGE_MODEL") ??
        "meta-llama/llama-4-maverick-17b-128e-instruct";
      const baseURL = getBaseURL(runtime);
      const groq = createGroq({
        apiKey: runtime.getSetting("GROQ_API_KEY"),
        fetch: runtime.fetch,
        baseURL,
      });

      return await generateGroqText(groq, model, {
        prompt,
        system: runtime.character.system ?? undefined,
        temperature,
        maxTokens,
        frequencyPenalty,
        presencePenalty,
        stopSequences,
      });
    },
    [ModelType.OBJECT_SMALL]: async (
      runtime,
      params: ObjectGenerationParams,
    ) => {
      const baseURL = getBaseURL(runtime);
      const groq = createGroq({
        apiKey: runtime.getSetting("GROQ_API_KEY"),
        baseURL,
      });
      const model =
        runtime.getSetting("GROQ_SMALL_MODEL") ??
        runtime.getSetting("SMALL_MODEL") ??
        "llama-3.1-8b-instant";

      if (params.schema) {
        logger.info("Using OBJECT_SMALL without schema validation");
      }

      return await generateGroqObject(groq, model, params);
    },
    [ModelType.OBJECT_LARGE]: async (
      runtime,
      params: ObjectGenerationParams,
    ) => {
      const baseURL = getBaseURL(runtime);
      const groq = createGroq({
        apiKey: runtime.getSetting("GROQ_API_KEY"),
        baseURL,
      });
      const model =
        runtime.getSetting("GROQ_LARGE_MODEL") ??
        runtime.getSetting("LARGE_MODEL") ??
        "meta-llama/llama-4-maverick-17b-128e-instruct";

      if (params.schema) {
        logger.info("Using OBJECT_LARGE without schema validation");
      }

      return await generateGroqObject(groq, model, params);
    },
  },
  tests: [
    {
      name: "groq_plugin_tests",
      tests: [
        {
          name: "groq_test_url_and_api_key_validation",
          fn: async (runtime) => {
            const baseURL =
              getBaseURL(runtime) ?? "https://api.groq.com/openai/v1";
            const response = await fetch(`${baseURL}/models`, {
              headers: {
                Authorization: `Bearer ${runtime.getSetting("GROQ_API_KEY")}`,
              },
            });
            const data = await response.json();
            logger.log(
              `Models Available: ${(data as { data: unknown[] })?.data?.length}`,
            );
            if (!response.ok) {
              throw new Error(
                `Failed to validate Groq API key: ${response.statusText}`,
              );
            }
          },
        },
        {
          name: "groq_test_text_large",
          fn: async (runtime) => {
            const text = await runtime.useModel(ModelType.TEXT_LARGE, {
              prompt: "What is the nature of reality in 10 words?",
            });
            if (text.length === 0) {
              throw new Error("Failed to generate text");
            }
            logger.log(`generated with test_text_large: ${text}`);
          },
        },
        {
          name: "groq_test_text_small",
          fn: async (runtime) => {
            const text = await runtime.useModel(ModelType.TEXT_SMALL, {
              prompt: "What is the nature of reality in 10 words?",
            });
            if (text.length === 0) {
              throw new Error("Failed to generate text");
            }
            logger.log(`generated with test_text_small: ${text}`);
          },
        },
        {
          name: "groq_test_image_generation",
          fn: async (runtime) => {
            logger.log("groq_test_image_generation");
            const image = await runtime.useModel(ModelType.IMAGE, {
              prompt: "A beautiful sunset over a calm ocean",
              n: 1,
              size: "1024x1024",
            });
            logger.log(
              `generated with test_image_generation: ${JSON.stringify(image)}`,
            );
          },
        },
        {
          name: "groq_test_transcription",
          fn: async (runtime) => {
            logger.log("groq_test_transcription");
            const response = await fetch(
              "https://upload.wikimedia.org/wikipedia/en/4/40/Chris_Benoit_Voice_Message.ogg",
            );
            if (!response.ok) {
              throw new Error(
                `Failed to fetch audio sample: ${response.statusText}`,
              );
            }
            const arrayBuffer = await response.arrayBuffer();
            const transcription = await runtime.useModel(
              ModelType.TRANSCRIPTION,
              Buffer.from(new Uint8Array(arrayBuffer)),
            );
            logger.log(`generated with test_transcription: ${transcription}`);
          },
        },
        {
          name: "groq_test_text_tokenizer_encode",
          fn: async (runtime) => {
            const prompt = "Hello tokenizer encode!";
            const tokens = await runtime.useModel(
              ModelType.TEXT_TOKENIZER_ENCODE,
              { prompt },
            );
            if (!Array.isArray(tokens) || tokens.length === 0) {
              throw new Error(
                "Failed to tokenize text: expected non-empty array of tokens",
              );
            }
            logger.log(`Tokenized output: ${JSON.stringify(tokens)}`);
          },
        },
        {
          name: "groq_test_text_tokenizer_decode",
          fn: async (runtime) => {
            const prompt = "Hello tokenizer decode!";
            // Encode the string into tokens first
            const tokens = await runtime.useModel(
              ModelType.TEXT_TOKENIZER_ENCODE,
              { prompt },
            );
            // Now decode tokens back into text
            const decodedText = await runtime.useModel(
              ModelType.TEXT_TOKENIZER_DECODE,
              {
                tokens,
              },
            );
            if (decodedText !== prompt) {
              throw new Error(
                `Decoded text does not match original. Expected "${prompt}", got "${decodedText}"`,
              );
            }
            logger.log(`Decoded text: ${decodedText}`);
          },
        },
        {
          name: "groq_test_object_small",
          fn: async (runtime) => {
            const object = await runtime.useModel(ModelType.OBJECT_SMALL, {
              prompt:
                "Generate a JSON object representing a user profile with name, age, and hobbies",
              temperature: 0.7,
            });
            logger.log(`Generated object: ${JSON.stringify(object)}`);
          },
        },
        {
          name: "groq_test_object_large",
          fn: async (runtime) => {
            const object = await runtime.useModel(ModelType.OBJECT_LARGE, {
              prompt:
                "Generate a detailed JSON object representing a restaurant with name, cuisine type, menu items with prices, and customer reviews",
              temperature: 0.7,
            });
            logger.log(`Generated object: ${JSON.stringify(object)}`);
          },
        },
      ],
    },
  ],
};
export default groqPlugin;
