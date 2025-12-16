#include "elizaos/core.hpp"
#include "types.hpp"
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

;
;
import type { Content, Entity, IAgentRuntime, Memory, State, TemplateType } from './types';

;

;

;





// Text Utils

/**
 * Convert all double-brace bindings in a Handlebars template
 * to triple-brace bindings, so the output is NOT HTML-escaped.
 *
 * - Ignores block/partial/comment tags that start with # / ! >.
 * - Ignores the else keyword.
 * - Ignores bindings that are already triple-braced.
 *
 * @param  tpl  Handlebars template source
 * @return      Transformed template
 */
// really shouldn't be exported
/*

*/

/**
 * Function to compose a prompt using a provided template and state.
 * It compiles the template (upgrading double braces to triple braces for non-HTML escaping)
 * and then populates it with values from the state. Additionally, it processes the
 * resulting string with `composeRandomUser` to replace placeholders like `{{nameX}}`.
 *
 * @param {Object} options - Object containing state and template information.
 * @param {State} options.state - The state object containing values to fill the template.
 * @param {TemplateType} options.template - The template string or  The composed prompt output, with state values and random user names populated.
 */
const composePrompt = ({
  state,
  template,
}: {
  state: { [key: string]: string };
  template: TemplateType;
}) => {
}

/**
 * Function to compose a prompt using a provided template and state.
 *
 * @param {Object} options - Object containing state and template information.
 * @param {State} options.state - The state object containing values to fill the template.
 * @param {TemplateType} options.template - The template to be used for composing the prompt.
 * @returns {string} The composed prompt output.
 */
const composePromptFromState = ({
  state,
  template,
}: {
  state: State;
  template: TemplateType;
}) => {
}

/**
 * Adds a header to a body of text.
 *
 * This  header - The header to add to the body.
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
const addHeader = (header: string, body: string) => {
  return coreAddHeader(header, body);
};


const formatPosts = ({
  messages,
  entities,
  conversationHeader = true,
}: {
  messages: Memory[];
  entities: Entity[];
  conversationHeader?: boolean;
}) => {
  return coreFormatPosts({ messages, entities, conversationHeader })
}

/**
 * Format messages into a string
 * @param {Object} params - The formatting parameters
 * @param {Memory[]} params.messages - List of messages to format
 * @param {Entity[]} params.entities - List of entities for name resolution
 * @returns {string} Formatted message string with timestamps and user information
 */
const formatMessages = ({
  messages,
  entities,
}: {
  messages: Memory[];
  entities: Entity[];
}) => {
  return coreFormatMessages({ messages, entities });
}

const formatTimestamp = (messageDate: number) => {
  return coreFormatTimestamp(messageDate);
}

/**
 * Validates a UUID value.
 *
 * @param {unknown} value - The value to validate.
 * @returns {UUID | null} Returns the validated UUID value or null if validation fails.
 */


/**
 * Converts a string or number to a UUID.
 *
 * @param {string | number} target - The string or number to convert to a UUID.
 * @returns {UUID} The UUID generated from the input target.
 * @throws {TypeError} Throws an error if the input target is not a string.
 */

} // namespace elizaos
