#include "elizaos/core.hpp"
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

class StagehandError extends Error {
  public recoverable: boolean;
  public details?: any;

  constructor(
    message: string,
    public code: string,
    public userMessage: string,
    isRetryable: boolean = true,
    details?: any
  ) {
    super(message);
    this.name = 'StagehandError';
    this.recoverable = isRetryable;
    this.isRetryable = isRetryable;
    this.details = details;
  }
}

class BrowserServiceNotAvailableError extends StagehandError {
  constructor() {
    super(
      'Browser service is not available',
      'SERVICE_NOT_AVAILABLE',
      'The browser automation service is not available. Please ensure the Stagehand plugin is properly configured.',
      false
    );
  }
}

class BrowserSessionError extends StagehandError {
  constructor(message: string, details?: any) {
    super(
      message,
      'BROWSER_SESSION_ERROR',
      'There was an error with the browser session. Please try again.',
      true,
      details
    );
  }
}

class BrowserNavigationError extends StagehandError {
  constructor(url: string, originalError?: Error) {
    const message = originalError
      ? `Failed to navigate to ${url}: ${originalError.message}`
      : `Failed to navigate to ${url}`;

    super(
      message,
      'NAVIGATION_ERROR',
      "I couldn't navigate to the requested page. Please check the URL and try again.",
      true,
      { url, originalError: originalError?.message }
    );
  }
}

class BrowserActionError extends StagehandError {
  constructor(action: string, target: string, originalError?: Error) {
    const message = originalError
      ? `Failed to ${action} on ${target}: ${originalError.message}`
      : `Failed to ${action} on ${target}`;

    super(
      message,
      'ACTION_ERROR',
      `I couldn't ${action} on the requested element. Please check if the element exists and try again.`,
      true,
      { action, target, originalError: originalError?.message }
    );
  }
}

class BrowserSecurityError extends StagehandError {
  public details?: any;

  constructor(message: string, details?: any) {
    super(message, 'SECURITY_ERROR', 'This action was blocked for security reasons.', false);
    this.details = details;
  }
}

]:`, error.message);
    callback?.({
      text: error.userMessage,
      error: true,
    });
  } else {
    logger.error('Unexpected browser error:', error);
    callback?.({
      text: action
        ? `I encountered an error while trying to ${action}. Please try again.`
        : 'I encountered an unexpected error. Please try again.',
      error: true,
    });
  }
  return [];
}

} // namespace elizaos
