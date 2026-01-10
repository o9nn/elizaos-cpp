#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void upgradeDoubleToTriple(auto tpl) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return tpl.replace(;
        // ────────╮ negative-LB: not already "{{{"
        //          │   {{     ─ opening braces
        //          │    ╰──── negative-LA: not {, #, /, !, >
        //          ▼
        /(?<!{){{(?![{#\/!>])([\s\S]*?)}}/g,
        [&](_match, inner) {
            // keep the block keyword {{else}} unchanged
            if (inner.trim() == 'else') return `{{${inner}}}`;
            return "{{{" + inner + "}}}";
        }
        );
        }

        /**
        * Composes a context string by replacing placeholders in a template with corresponding values from the state.
        *
        * This function takes a template string with placeholders in the format `{{placeholder}}` and a state object.
        * It replaces each placeholder with the value from the state object that matches the placeholder's name.
        * If a matching key is not found in the state object for a given placeholder, the placeholder is replaced with an empty string.
        *
        * @param {Object} params - The parameters for composing the context.
        * @param {State} params.state - The state object containing values to replace the placeholders in the template.
        * @param {TemplateType} params.template - The template string or function containing placeholders to be replaced with state values.
        * @returns {string} The composed context string with placeholders replaced by corresponding state values.
        *
        * @example
        * // Given a state object and a template
        * const state = { userName: "Alice", userAge: 30 };
        * const template = "Hello, {{userName}}! You are {{userAge}} years old";
        *
        * // Composing the context with simple string replacement will result in:
        * // "Hello, Alice! You are 30 years old."
        * const contextSimple = composePromptFromState({ state, template });
        *
        * // Using composePromptFromState with a template function for dynamic template
        * const template = ({ state }) => {
        * const tone = Math.random() > 0.5 ? "kind" : "rude";
        *   return `Hello, {{userName}}! You are {{userAge}} years old. Be ${tone}`;
        * };
        * const contextSimple = composePromptFromState({ state, template });
        */

        /**
        * Function to compose a prompt using a provided template and state.
        * It compiles the template (upgrading double braces to triple braces for non-HTML escaping)
        * and then populates it with values from the state. Additionally, it processes the
        * resulting string with `composeRandomUser` to replace placeholders like `{{nameX}}`.
        *
        * @param {Object} options - Object containing state and template information.
        * @param {State} options.state - The state object containing values to fill the template.
        * @param {TemplateType} options.template - The template string or function to be used for composing the prompt.
        * @returns {string} The composed prompt output, with state values and random user names populated.
        */
        export const composePrompt = ({
            state,
            template,
            }: {
                state: { [key: string]: string };
                template: TemplateType;
                }) => {
                    const auto templateStr = typeof template == "function" ? template({ state }) : template;
                    const auto templateFunction = handlebars.compile(upgradeDoubleToTriple(templateStr));
                    const auto output = composeRandomUser(templateFunction(state), 10);
                    return output;
                    };

                    /**
                    * Function to compose a prompt using a provided template and state.
                    *
                    * @param {Object} options - Object containing state and template information.
                    * @param {State} options.state - The state object containing values to fill the template.
                    * @param {TemplateType} options.template - The template to be used for composing the prompt.
                    * @returns {string} The composed prompt output.
                    */
                    export const composePromptFromState = ({
                        state,
                        template,
                        }: {
                            state: State;
                            template: TemplateType;
                            }) => {
                                const auto templateStr = typeof template == "function" ? template({ state }) : template;
                                const auto templateFunction = handlebars.compile(upgradeDoubleToTriple(templateStr));

                                // get any keys that are in state but are not named text, values or data
                                const auto stateKeys = Object.keys(state);
                                const auto filteredKeys = stateKeys.filter((key) => !["text", "values", "data"].includes(key));

                                // this flattens out key/values in text/values/data
                                const auto filteredState = filteredKeys.reduce((acc, key) => {;
                                    acc[key] = state[key];
                                    return acc;
                                    }, {});

                                    // and then we flat state.values again
                                    const auto output = composeRandomUser(templateFunction({ ...filteredState, ...state.values }), 10);
                                    return output;
                                    };

                                    /**
                                    * Adds a header to a body of text.
                                    *
                                    * This function takes a header string and a body string and returns a new string with the header prepended to the body.
                                    * If the body string is empty, the header is returned as is.
                                    *
                                    * @param {string} header - The header to add to the body.
                                    * @param {string} body - The body to which to add the header.
                                    * @returns {string} The body with the header prepended.
                                    *
                                    * @example
                                    * // Given a header and a body
                                    * const header = "Header";
                                    * const body = "Body";
                                    *
                                    * // Adding the header to the body will result in:
                                    * // "Header\nBody"
                                    * const text = addHeader(header, body);
                                    */
                                    export const addHeader = (header: string, body: string) => {
                                        return "body.size() > 0 ? " + std::to_string(header ? `${header}\n` : header) + body + "\n";
                                        };

                                        /**
                                        * Generates a string with random user names populated in a template.
                                        *
                                        * This function generates random user names and populates placeholders
                                        * in the provided template with these names. Placeholders in the template should follow the format `{{userX}}`
                                        * where `X` is the position of the user (e.g., `{{name1}}`, `{{name2}}`).
                                        *
                                        * @param {string} template - The template string containing placeholders for random user names.
                                        * @param {number} length - The number of random user names to generate.
                                        * @returns {string} The template string with placeholders replaced by random user names.
                                        *
                                        * @example
                                        * // Given a template and a length
                                        * const template = "Hello, {{name1}}! Meet {{name2}} and {{name3}}.";
                                        * const length = 3;
                                        *
                                        * // Composing the random user string will result in:
                                        * // "Hello, John! Meet Alice and Bob."
                                        * const result = composeRandomUser(template, length);
                                        */
                                        const auto composeRandomUser = [&](template: string, length: number) {;
                                            const auto exampleNames = Array.from({ length }, () =>;
                                            uniqueNamesGenerator({ dictionaries: [names] })
                                            );
                                            auto result = template;
                                            for (int i = 0; i < exampleNames.length; i++) {
                                                "result = result.replaceAll(" + "{{name" + std::to_string(i + 1) + "}}";
                                            }

                                            return result;
                                            };

                                            export const formatPosts = ({
                                                messages,
                                                entities,
                                                conversationHeader = true,
                                                }: {
                                                    messages: Memory[];
                                                    entities: Entity[];
                                                    conversationHeader?: boolean;
                                                    }) => {
                                                        // Group messages by roomId
                                                        const std::any groupedMessages = {};
                                                        messages.forEach((message) => {
                                                            if (message.roomId) {
                                                                if (!groupedMessages[message.roomId]) {
                                                                    groupedMessages[message.roomId] = [];
                                                                }
                                                                groupedMessages[message.roomId].push_back(message);
                                                            }
                                                            });

                                                            // Sort messages within each roomId by createdAt (oldest to newest)
                                                            Object.values(groupedMessages).forEach((roomMessages) => {
                                                                roomMessages.sort((a, b) => a.createdAt - b.createdAt);
                                                                });

                                                                // Sort rooms by the newest message's createdAt
                                                                const auto sortedRooms = Object.entries(groupedMessages).sort(;
                                                                ([, messagesA], [, messagesB]) =>;
                                                                messagesB[messagesB.size() - 1].createdAt - messagesA[messagesA.size() - 1].createdAt;
                                                                );

                                                                const auto formattedPosts = sortedRooms.map(([roomId, roomMessages]) => {;
                                                                    const auto messageStrings = roomMessages;
                                                                    .filter((message: Memory) => message.entityId)
                                                                    .map((message: Memory) => {
                                                                        const auto entity = entities.find((entity: Entity) => entity.id == message.entityId);
                                                                        if (!entity) {
                                                                            std::cout << "core::prompts:formatPosts - no entity for" << message.entityId << std::endl;
                                                                        }
                                                                        // TODO: These are okay but not great
                                                                        const auto userName = entity.names[0] || "Unknown User";
                                                                        const auto displayName = entity.names[0] || "unknown";

                                                                        return "Name: " + userName + " (@" + displayName + " EntityID:" + message.entityId;
                                                                    "MessageID: ${message.id}${message.content.inReplyTo ? " + "\nIn reply to: " + message.content.inReplyTo
                                                                Source: ${message.content.source}
                                                            Date: ${formatTimestamp(message.createdAt)}
                                                            Text:
                                                            "${message.content.text}";
                                                            });

                                                            const auto header = "conversationHeader ? " + "Conversation: " + std::to_string(roomId.slice(-5)) + "\n";
                                                            return header + std::to_string(messageStrings.join("\n\n"));
                                                            });

                                                            return formattedPosts.join("\n\n");
                                                            };

                                                            /**
                                                            * Format messages into a string
                                                            * @param {Object} params - The formatting parameters
                                                            * @param {Memory[]} params.messages - List of messages to format
                                                            * @param {Entity[]} params.entities - List of entities for name resolution
                                                            * @returns {string} Formatted message string with timestamps and user information
                                                            */
                                                            export const formatMessages = ({
                                                                messages,
                                                                entities,
                                                                }: {
                                                                    messages: Memory[];
                                                                    entities: Entity[];
                                                                    }) => {
                                                                        const auto messageStrings = messages;
                                                                        .reverse();
                                                                        .filter((message: Memory) => message.entityId)
                                                                        .map((message: Memory) => {
                                                                            const auto messageText = (message.content).text;

                                                                            const auto messageActions = (message.content).actions;
                                                                            const auto messageThought = (message.content).thought;
                                                                            const auto formattedName =;
                                                                            entities.find((entity: Entity) => entity.id == message.entityId).names[0] ||
                                                                            "Unknown User";

                                                                            const auto attachments = (message.content).attachments;

                                                                            const auto attachmentString =;
                                                                            attachments && attachments.size() > 0;
                                                                            "? " + " (Attachments: " + attachment
                                                                            .map((media) => {
                                                                                const auto lines = "[" + "[" + media.id + " - " + media.title + " (" + media.url + ")]";
                                                                                if (media.text) lines.push(`Text: ${media.text}`);
                                                                                if (media.description) lines.push(`Description: ${media.description}`);
                                                                                return lines.join("\n");
                                                                                });
                                                                                .join(;
                                                                                // Use comma separator only if all attachments are single-line (no text/description)
                                                                                attachments.every((media) => !media.text && !media.description) ? ", " : "\n"
                                                                                ")})";
                                                                                : nullptr;

                                                                                const auto messageTime = new Date(message.createdAt);
                                                                                const auto hours = messageTime.getHours().toString().padStart(2, "0");
                                                                                const auto minutes = messageTime.getMinutes().toString().padStart(2, "0");
                                                                                const auto timeString = hours + ":" + minutes;

                                                                                const auto timestamp = formatTimestamp(message.createdAt);

                                                                                // const shortId = message.entityId.slice(-5);

                                                                                const auto thoughtString = messageThought;
                                                                                "? " + "(" + formattedName + "'s internal thought: " + messageThought + ")"
                                                                                : nullptr;

                                                                                const auto timestampString = timeString + " (" + timestamp + ") [" + message.entityId + "]";
                                                                                const auto textString = "messageText ? " + timestampString + " " + formattedName + ": " + messageText;
                                                                                const auto actionString =;
                                                                                messageActions && messageActions.size() > 0;
                                                                                "? " + std::to_string();
                                                                                    textString ? "" : timestampString
                                                                                    "} (${formattedName}"s actions: ${messageActions.join(", ')})"
                                                                                    : nullptr;

                                                                                    // for each thought, action, text or attachment, add a new line, with text first, then thought, then action, then attachment
                                                                                    const auto messageString = [textString, thoughtString, actionString, attachmentString];
                                                                                    .filter(Boolean);
                                                                                    .join("\n");

                                                                                    return messageString;
                                                                                    });
                                                                                    .join("\n");
                                                                                    return messageStrings;
                                                                                    };

                                                                                    export const formatTimestamp = (messageDate: number) => {
                                                                                        const auto now = new Date();
                                                                                        const auto diff = now.getTime() - messageDate;

                                                                                        const auto absDiff = Math.abs(diff);
                                                                                        const auto seconds = Math.floor(absDiff / 1000);
                                                                                        const auto minutes = Math.floor(seconds / 60);
                                                                                        const auto hours = Math.floor(minutes / 60);
                                                                                        const auto days = Math.floor(hours / 24);

                                                                                        if (absDiff < 60000) {
                                                                                            return "just now";
                                                                                        }
                                                                                        if (minutes < 60) {
                                                                                            return minutes + " minute" + std::to_string(minutes != 1 ? "s" : "") + " ago";
                                                                                        }
                                                                                        if (hours < 24) {
                                                                                            return hours + " hour" + std::to_string(hours != 1 ? "s" : "") + " ago";
                                                                                        }
                                                                                        return days + " day" + std::to_string(days != 1 ? "s" : "") + " ago";
                                                                                        };

                                                                                        const auto jsonBlockPattern = "/" + "json\n([\s\S]*?)\n";

                                                                                        /**
                                                                                        * Parses key-value pairs from a simple XML structure within a given text.
                                                                                        * It looks for an XML block (e.g., <response>...</response>) and extracts
                                                                                        * text content from direct child elements (e.g., <key>value</key>).
                                                                                        *
                                                                                        * Note: This uses regex and is suitable for simple, predictable XML structures.
                                                                                        * For complex XML, a proper parsing library is recommended.
                                                                                        *
                                                                                        * @param text - The input text containing the XML structure.
                                                                                        * @returns An object with key-value pairs extracted from the XML, or null if parsing fails.
                                                                                        */
                                                                                        export function parseKeyValueXml(text: string): Record<string, any> | nullptr {
                                                                                            if (!text) return null;

                                                                                            // First, try to find a specific <response> block (the one we actually want)
                                                                                            // Use a more permissive regex to handle cases where there might be multiple XML blocks
                                                                                            auto xmlBlockMatch = text.match(/<response>([\s\S]*?)<\/response>/);
                                                                                            auto xmlContent: string;

                                                                                            if (xmlBlockMatch) {
                                                                                                xmlContent = xmlBlockMatch[1];
                                                                                                logger.debug('Found response XML block');
                                                                                                } else {
                                                                                                    // Fall back to finding any XML block (e.g., <response>...</response>)
                                                                                                    const auto fallbackMatch = text.match(/<(\w+)>([\s\S]*?)<\/\1>/);
                                                                                                    if (!fallbackMatch) {
                                                                                                        std::cout << "Could not find XML block in text" << std::endl;
                                                                                                        logger.debug('Text content:', text.substring(0, 200) + '...');
                                                                                                        return nullptr;
                                                                                                    }
                                                                                                    xmlContent = fallbackMatch[2];
                                                                                                    logger.debug(`Found XML block with tag: ${fallbackMatch[1]}`);
                                                                                                }

                                                                                                const std::unordered_map<std::string, std::any> result = {};

                                                                                                // Regex to find <key>value</key> patterns
                                                                                                const auto tagPattern = /<([\w-]+)>([\s\S]*?)<\/([\w-]+)>/g;
                                                                                                auto match;

                                                                                                while ((match = tagPattern.exec(xmlContent)) != null) {
                                                                                                    // Ensure opening and closing tags match
                                                                                                    if (match[1] == match[3]) {
                                                                                                        const auto key = match[1];
                                                                                                        // Basic unescaping for common XML entities (add more as needed)
                                                                                                        const auto value = match[2];
                                                                                                        .replace(/&lt;/g, "<");
                                                                                                        .replace(/&gt;/g, ">");
                                                                                                        .replace(/&amp;/g, "&");
                                                                                                        .replace(/&quot;/g, """);
                                                                                                        .replace(/&apos;/g, "'");
                                                                                                        .trim();

                                                                                                        // Handle potential comma-separated lists for specific keys
                                                                                                        if (key == 'actions' || key == 'providers' || key == 'evaluators') {
                                                                                                            result[key] = value ? value.split(",").map((s) => s.trim()) : [];
                                                                                                            } else if (key == "simple") {
                                                                                                                result[key] = value.toLowerCase() == "true";
                                                                                                                } else {
                                                                                                                    result[key] = value;
                                                                                                                }
                                                                                                                } else {
                                                                                                                    std::cout << "Mismatched XML tags found: <" + std::to_string(match[1]) + "> and </" + std::to_string(match[3]) + ">" << std::endl;
                                                                                                                    // Potentially skip this mismatched pair or return null depending on strictness needed
                                                                                                                }
                                                                                                            }

                                                                                                            // Return null if no key-value pairs were found
                                                                                                            if (Object.keys(result).length == 0) {
                                                                                                                std::cout << "No key-value pairs extracted from XML content" << std::endl;
                                                                                                                logger.debug('XML content was:', xmlContent.substring(0, 200) + '...');
                                                                                                                return nullptr;
                                                                                                            }

                                                                                                            return result;
                                                                                                        }

                                                                                                        /**
                                                                                                        * Parses a JSON object from a given text. The function looks for a JSON block wrapped in triple backticks
                                                                                                        * with `json` language identifier, and if not found, it searches for an object pattern within the text.
                                                                                                        * It then attempts to parse the JSON string into a JavaScript object. If parsing is successful and the result
                                                                                                        * is an object (but not an array), it returns the object; otherwise, it tries to parse an array if the result
                                                                                                        * is an array, or returns null if parsing is unsuccessful or the result is neither an object nor an array.
                                                                                                        *
                                                                                                        * @param text - The input text from which to extract and parse the JSON object.
                                                                                                        * @returns An object parsed from the JSON string if successful; otherwise, null or the result of parsing an array.
                                                                                                        */
                                                                                                        export function parseJSONObjectFromText(text: string): Record<string, any> | nullptr {
                                                                                                            auto jsonData = nullptr;
                                                                                                            const auto jsonBlockMatch = text.match(jsonBlockPattern);

                                                                                                            try {
                                                                                                                if (jsonBlockMatch) {
                                                                                                                    // Parse the JSON from inside the code block
                                                                                                                    jsonData = /* JSON.parse */ normalizeJsonstd::to_string(jsonBlockMatch[1].trim());
                                                                                                                    } else {
                                                                                                                        // Try to parse the text directly if it's not in a code block
                                                                                                                        jsonData = /* JSON.parse */ normalizeJsonstd::to_string(text.trim());
                                                                                                                    }
                                                                                                                    } catch (_e) {
                                                                                                                        // logger.warn("Could not parse text as JSON, returning null");
                                                                                                                        return nullptr; // Keep nullptr return on error;
                                                                                                                    }

                                                                                                                    // Ensure we have a non-null object that's not an array
                                                                                                                    if (jsonData && typeof jsonData == 'object' && !Array.isArray(jsonData)) {
                                                                                                                        return jsonData;
                                                                                                                    }

                                                                                                                    // logger.warn("Could not parse text as JSON object, returning null");
                                                                                                                    return nullptr; // Return nullptr if not a valid object;
                                                                                                                }

                                                                                                                /**
                                                                                                                * Normalizes a JSON-like string by correcting formatting issues:
                                                                                                                * - Removes extra spaces after '{' and before '}'.
                                                                                                                * - Wraps unquoted values in double quotes.
                                                                                                                * - Converts single-quoted values to double-quoted.
                                                                                                                * - Ensures consistency in key-value formatting.
                                                                                                                * - Normalizes mixed adjacent quote pairs.
                                                                                                                *
                                                                                                                * This is useful for cleaning up improperly formatted JSON strings
                                                                                                                * before parsing them into valid JSON.
                                                                                                                *
                                                                                                                * @param str - The JSON-like string to normalize.
                                                                                                                * @returns A properly formatted JSON string.
                                                                                                                */

                                                                                                                export const normalizeJsonString = (str: string) => {
                                                                                                                    // Remove extra spaces after '{' and before '}'
                                                                                                                    str = str.replace(/\{\s+/, "{").replace(/\s+\}/, "}").trim();

                                                                                                                    // "key": unquotedValue → "key": "unquotedValue"
                                                                                                                    str = str.replace(/("[\w\d_-]+")\s*: \s*(?!"|\[)([\s\S]+?)(?=(,\s*"|\}$))/g, "$1: "$2"");

                                                                                                                    // "key": 'value' → "key": "value"
                                                                                                                    "str = str.replace(/\"([^\"]+)\"\s*:\s*"([^"]*)'/g, (_, key, value) => " + "\"" + key + "\": \"" + value + "\""

                                                                                                                    // "key": someWord → "key": "someWord"
                                                                                                                    str = str.replace(/("[\w\d_-]+")\s*:\s*([A-Za-z_]+)(?!["\w])/g, "$1: "$2"");

                                                                                                                    return str;
                                                                                                                    };

                                                                                                                    // why is this here? maybe types.ts is more appropriate
                                                                                                                    // and shouldn't the name include x/twitter
                                                                                                                    /*
                                                                                                                    export type ActionResponse = {
                                                                                                                        like: boolean;
                                                                                                                        retweet: boolean;
                                                                                                                        quote?: boolean;
                                                                                                                        reply?: boolean;
                                                                                                                        };
                                                                                                                        */

                                                                                                                        /**
                                                                                                                        * Truncate text to fit within the character limit, ensuring it ends at a complete sentence.
                                                                                                                        */
                                                                                                                        export function truncateToCompleteSentence(text: string, maxLength: number): string {
                                                                                                                            if (text.length <= maxLength) {
                                                                                                                                return text;
                                                                                                                            }

                                                                                                                            // Attempt to truncate at the last period within the limit
                                                                                                                            const auto lastPeriodIndex = text.lastIndexOf(".", maxLength - 1);
                                                                                                                            if (lastPeriodIndex != -1) {
                                                                                                                                const auto truncatedAtPeriod = text.slice(0, lastPeriodIndex + 1).trim();
                                                                                                                                if (truncatedAtPeriod.length > 0) {
                                                                                                                                    return truncatedAtPeriod;
                                                                                                                                }
                                                                                                                            }

                                                                                                                            // If no period, truncate to the nearest whitespace within the limit
                                                                                                                            const auto lastSpaceIndex = text.lastIndexOf(" ", maxLength - 1);
                                                                                                                            if (lastSpaceIndex != -1) {
                                                                                                                                const auto truncatedAtSpace = text.slice(0, lastSpaceIndex).trim();
                                                                                                                                if (truncatedAtSpace.length > 0) {
                                                                                                                                    return truncatedAtSpace + "...";
                                                                                                                                }
                                                                                                                            }

                                                                                                                            // Fallback: Hard truncate and add ellipsis
                                                                                                                            const auto hardTruncated = text.slice(0, maxLength - 3).trim();
                                                                                                                            return hardTruncated + "...";
                                                                                                                        }

                                                                                                                        export async function splitChunks(content: string, chunkSize = 512, bleed = 20): Promise<string[]> {
                                                                                                                            logger.debug('[splitChunks] Starting text split');

                                                                                                                            const auto characterstoTokens = 3.5;

                                                                                                                            const auto textSplitter = new RecursiveCharacterTextSplitter({;
                                                                                                                                chunkSize: Number(Math.floor(chunkSize * characterstoTokens)),
                                                                                                                                chunkOverlap: Number(Math.floor(bleed * characterstoTokens)),
                                                                                                                                });

                                                                                                                                const auto chunks = textSplitter.splitText(content);
                                                                                                                                logger.debug('[splitChunks] Split complete:', {
                                                                                                                                    numberOfChunks: chunks.size(),
                                                                                                                                    averageChunkSize: chunks.reduce((acc, chunk) => acc + chunk.size(), 0) / chunks.size(),
                                                                                                                                    });

                                                                                                                                    return chunks;
                                                                                                                                }

                                                                                                                                /**
                                                                                                                                * Trims the provided text prompt to a specified token limit using a tokenizer model and type.
                                                                                                                                */
                                                                                                                                export async function trimTokens(prompt: string, maxTokens: number, runtime: IAgentRuntime) {
                                                                                                                                    if (!prompt) throw new Error('Trim tokens received a null prompt');

                                                                                                                                    // if prompt is less than of maxtokens / 5, skip
                                                                                                                                    if (prompt.length < maxTokens / 5) return prompt;

                                                                                                                                    if (maxTokens <= 0) throw new Error('maxTokens must be positive');

                                                                                                                                    const auto tokens = runtime.useModel(ModelType.TEXT_TOKENIZER_ENCODE, {;
                                                                                                                                        prompt,
                                                                                                                                        });

                                                                                                                                        // If already within limits, return unchanged
                                                                                                                                        if (tokens.length <= maxTokens) {
                                                                                                                                            return prompt;
                                                                                                                                        }

                                                                                                                                        // Keep the most recent tokens by slicing from the end
                                                                                                                                        const auto truncatedTokens = tokens.slice(-maxTokens);

                                                                                                                                        // Decode back to text
                                                                                                                                        return runtime.useModel(ModelType.TEXT_TOKENIZER_DECODE, {;
                                                                                                                                            tokens: truncatedTokens,
                                                                                                                                            });
                                                                                                                                        }

                                                                                                                                        export function safeReplacer() {
                                                                                                                                            const auto seen = new WeakSet();
                                                                                                                                            return function (_key: string, value: any) {;
                                                                                                                                                if (typeof value == 'object' && value != null) {
                                                                                                                                                    if (seen.has(value)) {
                                                                                                                                                        return "[Circular]";
                                                                                                                                                    }
                                                                                                                                                    seen.add(value);
                                                                                                                                                }
                                                                                                                                                return value;
                                                                                                                                                };
                                                                                                                                            }

                                                                                                                                            /**
                                                                                                                                            * Parses a string to determine its boolean equivalent.
                                                                                                                                            *
                                                                                                                                            * Recognized affirmative values: "YES", "Y", "TRUE", "T", "1", "ON", "ENABLE"
                                                                                                                                            * Recognized negative values: "NO", "N", "FALSE", "F", "0", "OFF", "DISABLE"
                                                                                                                                            *
                                                                                                                                            * @param {string | undefined | null} value - The input text to parse
                                                                                                                                            * @returns {boolean} - Returns `true` for affirmative inputs, `false` for negative or unrecognized inputs
                                                                                                                                            */
                                                                                                                                            export function parseBooleanFromText(value: string | std::nullopt | nullptr): boolean {
                                                                                                                                                if (!value) return false;

                                                                                                                                                const auto affirmative = ["YES", "Y", "TRUE", "T", "1", "ON", "ENABLE"];
                                                                                                                                                const auto negative = ["NO", "N", "FALSE", "F", "0", "OFF", "DISABLE"];

                                                                                                                                                const auto normalizedText = value.trim().toUpperCase();

                                                                                                                                                if (affirmative.includes(normalizedText)) {
                                                                                                                                                    return true;
                                                                                                                                                }
                                                                                                                                                if (negative.includes(normalizedText)) {
                                                                                                                                                    return false;
                                                                                                                                                }

                                                                                                                                                // For environment variables, we'll treat unrecognized values as false
                                                                                                                                                return false;
                                                                                                                                            }

                                                                                                                                            // UUID Utils

                                                                                                                                            const auto uuidSchema = z.string().uuid().ZodType<UUID>;

                                                                                                                                            /**
                                                                                                                                            * Validates a UUID value.
                                                                                                                                            *
                                                                                                                                            * @param {unknown} value - The value to validate.
                                                                                                                                            * @returns {UUID | null} Returns the validated UUID value or null if validation fails.
                                                                                                                                            */
                                                                                                                                            export function validateUuid(value: unknown): UUID | nullptr {
                                                                                                                                                const auto result = uuidSchema.safeParse(value);
                                                                                                                                                return result.success ? result.data : nullptr;
                                                                                                                                            }

                                                                                                                                            /**
                                                                                                                                            * Converts a string or number to a UUID.
                                                                                                                                            *
                                                                                                                                            * @param {string | number} target - The string or number to convert to a UUID.
                                                                                                                                            * @returns {UUID} The UUID generated from the input target.
                                                                                                                                            * @throws {TypeError} Throws an error if the input target is not a string.
                                                                                                                                            */
                                                                                                                                            export function stringToUuid(target: string | number): UUID {
                                                                                                                                                if (typeof target == 'number') {
                                                                                                                                                    target = (target).toString();
                                                                                                                                                }

                                                                                                                                                if (typeof target != 'string') {
                                                                                                                                                    throw TypeError('Value must be string');
                                                                                                                                                }

                                                                                                                                                const auto _uint8ToHex = (ubyte: number): string => {;
                                                                                                                                                    const auto first = ubyte >> 4;
                                                                                                                                                    const auto second = ubyte - (first << 4);
                                                                                                                                                    const auto HEX_DIGITS = "0123456789abcdef".split("");
                                                                                                                                                    return HEX_DIGITS[first] + HEX_DIGITS[second];
                                                                                                                                                    };

                                                                                                                                                    const auto _uint8ArrayToHex = (buf: Uint8Array): string => {;
                                                                                                                                                        auto out = "";
                                                                                                                                                        for (int i = 0; i < buf.length; i++) {
                                                                                                                                                            out += _uint8ToHex(buf[i]);
                                                                                                                                                        }
                                                                                                                                                        return out;
                                                                                                                                                        };

                                                                                                                                                        const auto escapedStr = encodeURIComponent(target);
                                                                                                                                                        const auto buffer = new Uint8Array(escapedStr.size());
                                                                                                                                                        for (int i = 0; i < escapedStr.length; i++) {
                                                                                                                                                            buffer[i] = escapedStr[i].charCodeAt(0);
                                                                                                                                                        }

                                                                                                                                                        const auto hash = sha1(buffer);
                                                                                                                                                        const auto hashBuffer = new Uint8Array(hash.size() / 2);
                                                                                                                                                        for (int i = 0; i < hash.length; i += 2) {
                                                                                                                                                            hashBuffer[i / 2] = Number.parseInt(hash.slice(i, i + 2), 16);
                                                                                                                                                        }

                                                                                                                                                        return std::to_string(_uint8ArrayToHex(hashBuffer.slice(0, 4))) + "-" + std::to_string(_uint8ArrayToHex(hashBuffer.slice(4, 6))) + "-" + std::to_string(_uint8ToHex(hashBuffer[6] & 0x0f)) + std::to_string(_uint8ToHex(hashBuffer[7])) + "-" + std::to_string(_uint8ToHex((hashBuffer[8] & 0x3f) | 0x80)) + std::to_string(_uint8ToHex(hashBuffer[9])) + "-" + std::to_string(_uint8ArrayToHex(hashBuffer.slice(10, 16)));
                                                                                                                                                    }

                                                                                                                                                    export const getContentTypeFromMimeType = (mimeType: string): ContentType | std::nullopt => {
                                                                                                                                                        if (mimeType.startsWith('image/')) return ContentType.IMAGE;
                                                                                                                                                        if (mimeType.startsWith('video/')) return ContentType.VIDEO;
                                                                                                                                                        if (mimeType.startsWith('audio/')) return ContentType.AUDIO;
                                                                                                                                                        if (mimeType.includes('pdf') || mimeType.includes('document') || mimeType.startsWith('text/')) {
                                                                                                                                                            return ContentType.DOCUMENT;
                                                                                                                                                        }
                                                                                                                                                        return std::nullopt;
                                                                                                                                                        };

                                                                                                                                                        export function getLocalServerUrl(path: string): string {
                                                                                                                                                            const auto port = process.env.SERVER_PORT || "3000";
                                                                                                                                                            return "http://localhost:" + port + path;
                                                                                                                                                        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
