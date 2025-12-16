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

/**
 * Defensive Sessions Client with 404 Error Handling
 * 
 * This module provides robust session log upload functionality that gracefully
 * handles endpoint unavailability (404 errors) without failing the entire workflow.
 * 
 * Fixes GitHub Issue #27: https://github.com/ZoneCog/elizaos-cpp/issues/27
 */

struct SessionLogData {
    std::string sessionId;
    std::vector<std::any> logs;
    double timestamp;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct SessionUploadOptions {
    std::optional<std::string> endpoint;
    std::optional<double> retries;
    std::optional<double> timeout;
    std::optional<bool> failOnError;
};


interface UploadError extends Error {
  status?: number;
  response?: any;
}

class SessionsClient {
  private defaultEndpoint: string;
  private defaultOptions: SessionUploadOptions;

  constructor(endpoint?: string, options: SessionUploadOptions = {}) {
    this.defaultEndpoint = endpoint || process.env.SESSION_LOGS_ENDPOINT || '';
    this.defaultOptions = {
      retries: 3,
      timeout: 10000,
      failOnError: false,
      ...options
    };
  }

  /**
   * Upload session logs with defensive error handling
   * @param logData - The session log data to upload
   * @param options - Upload options (optional)
   * @returns Promise<boolean> - true if successful, false if failed but gracefully handled
   */
  async uploadSessionLog(
    logData: SessionLogData, 
    options: SessionUploadOptions = {}
  ): Promise<boolean> {
    const opts = { ...this.defaultOptions, ...options };
    const endpoint = opts.endpoint || this.defaultEndpoint;

    if (!endpoint) {
      console.warn('Session log endpoint not configured - skipping upload');
      return false;
    }

    let lastError: UploadError | null = null;
    
    for (let attempt = 1; attempt <= (opts.retries || 1); attempt++) {
      try {
        console.log(`Uploading session logs (attempt ${attempt}/${opts.retries}) to: ${endpoint}`);
        
        const response = await this.performUpload(endpoint, logData, opts.timeout);
        
        if (response.ok) {
          console.log('Session logs uploaded successfully');
          return true;
        }
        
        // Handle specific HTTP status codes
        const error: UploadError = new Error(`HTTP ${response.status}: ${response.statusText}`);
        error.status = response.status;
        error.response = response;
        
        if (response.status === 404) {
          console.warn('Session log endpoint returned 404 – endpoint not found or service unavailable');
          console.warn('This is likely a configuration issue or the logging service is down');
          console.warn('Continuing execution without failing the workflow...');
          return false; // Gracefully handle 404 without throwing
        }
        
        if (response.status >= 400 && response.status < 500) {
          // Client errors (400-499) - don't retry
          console.error(`Client error ${response.status}: ${response.statusText}`);
          if (opts.failOnError) {
            throw error;
          }
          return false;
        }
        
        if (response.status >= 500) {
          // Server errors (500+) - retry
          console.warn(`Server error ${response.status}: ${response.statusText} - will retry`);
          lastError = error;
          continue;
        }
        
        lastError = error;
        
      } catch (error: any) {
        lastError = error;
        
        // Handle network errors, timeouts, etc.
        if (error.code === 'ECONNREFUSED' || error.code === 'ENOTFOUND') {
          console.warn(`Network error: ${error.message} - endpoint may be unavailable`);
          console.warn('Continuing execution without failing the workflow...');
          return false;
        }
        
        if (error.name === 'AbortError' || error.code === 'TIMEOUT') {
          console.warn(`Request timeout after ${opts.timeout}ms - will retry`);
          continue;
        }
        
        console.warn(`Upload attempt ${attempt} failed:`, error.message);
        
        if (attempt === opts.retries) {
          break; // Don't continue if this was the last attempt
        }
        
        // Wait before retry (exponential backoff)
        await this.delay(Math.min(1000 * Math.pow(2, attempt - 1), 10000));
      }
    }
    
    // All retries exhausted
    console.error(`Failed to upload session logs after ${opts.retries} attempts`);
    console.error('Last error:', lastError?.message);
    
    if (opts.failOnError && lastError) {
      throw lastError;
    }
    
    console.warn('Continuing execution despite session log upload failure...');
    return false;
  }

  /**
   * Perform the actual HTTP upload
   */
  private async performUpload(
    endpoint: string, 
    logData: SessionLogData, 
    timeoutMs?: number
  ): Promise<Response> {
    const controller = new AbortController();
    const timeoutId = timeoutMs ? setTimeout(() => controller.abort(), timeoutMs) : null;
    
    try {
      const response = await fetch(endpoint, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'User-Agent': 'ElizaOS-SessionsClient/1.0'
        },
        body: JSON.stringify({
          ...logData,
          uploadedAt: new Date().toISOString()
        }),
        signal: controller.signal
      });
      
      return response;
    } finally {
      if (timeoutId) {
        clearTimeout(timeoutId);
      }
    }
  }

  /**
   * Utility method for delays
   */
  private delay(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
  }

  /**
   * Health check for the session logging endpoint
   */
  async healthCheck(endpoint?: string): Promise<boolean> {
    const url = endpoint || this.defaultEndpoint;
    if (!url) {
      return false;
    }
    
    try {
      const response = await fetch(url, { 
        method: 'HEAD',
        signal: AbortSignal.timeout(5000)
      });
      return response.ok;
    } catch {
      return false;
    }
  }
}

// Export for use in workflows and other modules
{ SessionsClient, type SessionLogData, type SessionUploadOptions };

// Default for direct usage
default SessionsClient;

// CLI usage when run as a script
if (require.main === module) {
  const client = new SessionsClient();
  
  // Example usage
  const exampleLogData: SessionLogData = {
    sessionId: process.env.GITHUB_RUN_ID || 'test-session',
    logs: ['Example log entry'],
    timestamp: Date.now(),
    metadata: {
      workflow: process.env.GITHUB_WORKFLOW,
      runId: process.env.GITHUB_RUN_ID,
      actor: process.env.GITHUB_ACTOR
    }
  };
  
  client.uploadSessionLog(exampleLogData)
    .then(success => {
      console.log(`Session log upload ${success ? 'succeeded' : 'failed gracefully'}`);
      process.exit(0);
    })
    .catch(error => {
      console.error('Unexpected error:', error);
      process.exit(1);
    });
}
} // namespace elizaos
