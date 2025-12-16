#include "sessions-client.hpp"
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
 * Workflow Utilities for Defensive Error Handling
 * 
 * This module provides utilities for GitHub Actions workflows to handle
 * common failure scenarios gracefully, particularly for session logging
 * and external API integrations.
 * 
 * Fixes GitHub Issue #27: https://github.com/ZoneCog/elizaos-cpp/issues/27
 */

;

/**
 * Environment configuration for workflow error handling
 */
struct WorkflowConfig {
    std::optional<std::string> sessionLogsEndpoint;
    std::optional<bool> enableSessionLogging;
    std::optional<bool> failOnSessionLogError;
    std::optional<double> retryCount;
    std::optional<double> timeoutMs;
};


/**
 * Workflow context information
 */
struct WorkflowContext {
    std::optional<std::string> workflowName;
    std::optional<std::string> runId;
    std::optional<std::string> actor;
    std::optional<std::string> repository;
    std::optional<std::string> event;
    std::optional<std::string> ref;
};


/**
 * Main utility class for defensive workflow operations
 */
class WorkflowUtils {
  private config: WorkflowConfig;
  private context: WorkflowContext;
  private sessionsClient: SessionsClient;

  constructor(config: WorkflowConfig = {}) {
    this.config = {
      enableSessionLogging: true,
      failOnSessionLogError: false,
      retryCount: 3,
      timeoutMs: 10000,
      ...config
    };

    this.context = this.extractWorkflowContext();
    this.sessionsClient = new SessionsClient(
      this.config.sessionLogsEndpoint,
      {
        retries: this.config.retryCount || 3,
        timeout: this.config.timeoutMs || 10000,
        failOnError: this.config.failOnSessionLogError || false
      }
    );
  }

  /**
   * Extract workflow context from environment variables
   */
  private extractWorkflowContext(): WorkflowContext {
    return {
      workflowName: process.env.GITHUB_WORKFLOW || undefined,
      runId: process.env.GITHUB_RUN_ID || undefined,
      actor: process.env.GITHUB_ACTOR || undefined,
      repository: process.env.GITHUB_REPOSITORY || undefined,
      event: process.env.GITHUB_EVENT_NAME || undefined,
      ref: process.env.GITHUB_REF || undefined
    };
  }

  /**
   * Safely upload session logs without failing the workflow
   */
  async uploadSessionLogs(
    logs: any[], 
    sessionId?: string, 
    metadata?: Record<string, any>
  ): Promise<boolean> {
    if (!this.config.enableSessionLogging) {
      console.log('Session logging is disabled - skipping');
      return false;
    }

    const logData: SessionLogData = {
      sessionId: sessionId || this.context.runId || `session-${Date.now()}`,
      logs,
      timestamp: Date.now(),
      metadata: {
        ...this.context,
        ...metadata
      }
    };

    try {
      return await this.sessionsClient.uploadSessionLog(logData);
    } catch (error) {
      console.error('Unexpected error in session logging:', error);
      
      if (this.config.failOnSessionLogError) {
        throw error;
      }
      
      console.warn('Continuing despite session logging error...');
      return false;
    }
  }

  /**
   * Check if the session logging service is available
   */
  async checkSessionLogHealth(): Promise<boolean> {
    if (!this.config.enableSessionLogging) {
      return false;
    }

    try {
      return await this.sessionsClient.healthCheck();
    } catch (error) {
      console.warn('Health check failed:', error);
      return false;
    }
  }

  /**
   * Set GitHub Action output safely
   */
  setOutput(name: string, value: string): void {
    try {
      // GitHub Actions output format
      console.log(`::set-output name=${name}::${value}`);
    } catch (error) {
      console.warn(`Failed to set output ${name}:`, error);
    }
  }

  /**
   * Set GitHub Action environment variable safely
   */
  setEnvVar(name: string, value: string): void {
    try {
      const fs = require('fs');
      const path = process.env.GITHUB_ENV;
      
      if (path) {
        fs.appendFileSync(path, `${name}=${value}\n`);
      } else {
        console.warn('GITHUB_ENV not set - falling back to process.env');
        process.env[name] = value;
      }
    } catch (error) {
      console.warn(`Failed to set environment variable ${name}:`, error);
    }
  }

  /**
   * Log warning with GitHub Actions annotation
   */
  logWarning(message: string, file?: string, line?: number): void {
    let annotation = `::warning`;
    if (file) annotation += ` file=${file}`;
    if (line) annotation += `,line=${line}`;
    annotation += `::${message}`;
    
    console.log(annotation);
  }

  /**
   * Log error with GitHub Actions annotation
   */
  logError(message: string, file?: string, line?: number): void {
    let annotation = `::error`;
    if (file) annotation += ` file=${file}`;
    if (line) annotation += `,line=${line}`;
    annotation += `::${message}`;
    
    console.log(annotation);
  }

  /**
   * Exit gracefully with proper session log cleanup
   */
  async exitGracefully(
    exitCode: number = 0, 
    finalLogs?: any[], 
    reason?: string
  ): Promise<void> {
    if (finalLogs && finalLogs.length > 0) {
      console.log('Uploading final session logs before exit...');
      await this.uploadSessionLogs(finalLogs, undefined, { 
        exitReason: reason,
        exitCode 
      });
    }

    if (reason) {
      console.log(`Exiting gracefully: ${reason}`);
    }

    process.exit(exitCode);
  }

  /**
   * Wrap a  = {}
  ): Promise<T | null> {
    const opts = {
      failOnError: false,
      logErrors: true,
      retryCount: 1,
      ...options
    };

    for (let attempt = 1; attempt <= opts.retryCount; attempt++) {
      try {
        return await operation();
      } catch (error) {
        if (opts.logErrors) {
          console.error(`${operationName} failed (attempt ${attempt}/${opts.retryCount}):`, error);
        }

        if (attempt === opts.retryCount) {
          if (opts.failOnError) {
            throw error;
          } else {
            console.warn(`${operationName} failed after ${opts.retryCount} attempts - continuing gracefully`);
            return null;
          }
        }

        // Wait before retry
        await new Promise(resolve => setTimeout(resolve, 1000 * attempt));
      }
    }

    return null;
  }
}

/**
 * Factory ;

  return new WorkflowUtils(config);
}

// Export types and classes
{ WorkflowUtils, type WorkflowConfig, type WorkflowContext };

// Default default WorkflowUtils;
} // namespace elizaos
