/**
 * Shared module-level state for communication between the autonomous loop
 * and action handlers. Since Bun/Node is single-threaded, no race conditions.
 *
 * The autonomous loop callback captures the LLM's response text and stores it here.
 * Action handlers read it to extract parameters when options.parameters is empty.
 */

let currentLlmResponseText = "";

/** Set the current LLM response text (called from run.ts callback) */
export function setCurrentLlmResponse(text: string): void {
  currentLlmResponseText = text;
}

/** Get the current LLM response text (called from action handlers) */
export function getCurrentLlmResponse(): string {
  return currentLlmResponseText;
}

/** Clear the response (called at start of each step) */
export function clearCurrentLlmResponse(): void {
  currentLlmResponseText = "";
}
