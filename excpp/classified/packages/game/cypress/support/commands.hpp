#include "database-helpers.hpp"
#include "knowledge-helpers.hpp"
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

/// <reference types="cypress" />

;
;

/**
 * Custom Cypress Commands
 * Provides reusable commands for common test operations
 */

// Add custom command types
declare global {
  namespace Cypress {
    struct Chainable {
    std::string text;
    std::string userId;
    std::string roomId;
    std::optional<std::string> messageId;
    std::string text;
    std::optional<std::string> userId;
    std::optional<std::string> roomId;
    std::optional<std::string> messageId;
};

  }
}

// Initialize helpers
const knowledgeHelper = new KnowledgeTestHelper();
const dbHelper = new DatabaseTestHelper();

// Setup and Environment Commands
Cypress.Commands.add('setupTestEnvironment', () => {
  cy.log('Setting up test environment');
  cy.task('setupTestEnvironment');
});

// Custom command to wait for backend to be ready
Cypress.Commands.add('waitForBackend', () => {
  cy.request({
    method: 'GET',
    url: `${Cypress.env('BACKEND_URL') || 'http://localhost:7777'}/api/server/health`,
    retryOnStatusCodeFailure: true,
    retryOnNetworkFailure: true,
    timeout: 30000,
  }).then(() => {
    cy.log('✅ Backend is ready!');
  });
});

// Custom command to wait for elizaClient to be initialized
Cypress.Commands.add('waitForElizaClient', () => {
  cy.window().should((win: any) => {
    expect(win.elizaClient).to.exist;
    expect(win.elizaClient.socket).to.exist;
  });
});

// Custom command to send message via API
Cypress.Commands.add(
  'sendMessage',
  (
    contentOrMessage:
      | string
      | { text: string; userId?: string; roomId?: string; messageId?: string },
    roomId?: string
  ) => {
    const BACKEND_URL = Cypress.env('BACKEND_URL') || 'http://localhost:7777';

    let message: { text: string; userId: string; roomId: string; messageId?: string };

    // Handle both string and object formats
    if (typeof contentOrMessage === 'string') {
      message = {
        text: contentOrMessage,
        userId: `test-user-${Date.now()}`,
        roomId: roomId || '00000000-0000-0000-0000-000000000001',
        messageId: Date.now().toString(),
      };
    } else {
      message = {
        text: contentOrMessage.text,
        userId: contentOrMessage.userId || `test-user-${Date.now()}`,
        roomId: contentOrMessage.roomId || '00000000-0000-0000-0000-000000000001',
        messageId: contentOrMessage.messageId || Date.now().toString(),
      };
    }

    const headers: any = {
      'Content-Type': 'application/json',
    };

    // Add API key if configured
    const apiKey = Cypress.env('ELIZA_SERVER_AUTH_TOKEN');
    if (apiKey) {
      headers['X-API-KEY'] = apiKey;
    }

    return cy.request({
      method: 'POST',
      url: `${BACKEND_URL}/api/messaging/ingest-external`,
      body: {
        channel_id: message.roomId,
        server_id: '00000000-0000-0000-0000-000000000000',
        author_id: message.userId,
        author_display_name: message.userId,
        content: message.text,
        source_type: 'test',
        raw_message: { text: message.text, messageId: message.messageId },
        metadata: {
          test: true,
          originalMessageId: message.messageId,
        },
      },
      headers,
      failOnStatusCode: false,
    });
  }
);

// UI Helper Commands
Cypress.Commands.add('elementExists', (selector: string) => {
  cy.get('body').then(($body) => {
    return cy.wrap($body.find(selector).length > 0);
  });
});

Cypress.Commands.add('safeClick', (selector: string) => {
  cy.get(selector).should('be.visible').click({ force: true });
});

// Knowledge Management Commands
Cypress.Commands.add(
  'uploadKnowledgeFile',
  (fileName: string, content: string, fileType?: string) => {
    return cy.wrap(knowledgeHelper.uploadFile(fileName, content, fileType || 'text/plain'));
  }
);

Cypress.Commands.add('searchKnowledge', (query: string, count?: number) => {
  return knowledgeHelper.search(query, count);
});

Cypress.Commands.add('deleteKnowledgeDocument', (documentId: string) => {
  return knowledgeHelper.deleteDocument(documentId);
});

Cypress.Commands.add('cleanupKnowledgeTests', () => {
  return knowledgeHelper.cleanupTestDocuments();
});

Cypress.Commands.add('waitForDocumentProcessing', (documentId: string, timeout?: number) => {
  return knowledgeHelper.waitForDocumentProcessing(documentId, timeout);
});

// Capability Management Commands
Cypress.Commands.add('toggleCapability', (capability: string) => {
  const capabilityMap: Record<string, string> = {
    autonomy: 'autonomy-toggle',
    camera: 'camera-toggle',
    screen: 'screen-toggle',
    microphone: 'microphone-toggle',
    speaker: 'speakers-toggle',
    shell: 'shell-toggle',
    browser: 'browser-toggle',
  };

  const testId = capabilityMap[capability.toLowerCase()];
  if (testId) {
    cy.get(`[data-testid="${testId}"]`).click();
    cy.wait(1000); // Allow for API call
  } else {
    throw new Error(`Unknown capability: ${capability}`);
  }
});

Cypress.Commands.add('getCapabilityStatus', (capability: string) => {
  const BACKEND_URL = Cypress.env('BACKEND_URL') || 'http://localhost:7777';

  if (capability === 'autonomy') {
    return cy
      .request('GET', `${BACKEND_URL}/autonomy/status`)
      .then((response) => response.body.data.enabled);
  } else {
    return cy
      .request('GET', `${BACKEND_URL}/api/agents/default/capabilities/${capability}`)
      .then((response) => response.body.data.enabled);
  }
});

// Database Helper Commands
Cypress.Commands.add('authenticateDb', (username?: string, password?: string) => {
  return dbHelper.authenticate(username, password);
});

Cypress.Commands.add('getDbTables', () => {
  return dbHelper.getTables();
});

Cypress.Commands.add('cleanupDbTestRecords', (tableName: string, searchPattern?: string) => {
  return dbHelper.cleanupTestRecords(tableName, searchPattern);
});

// Export for use in tests
{};

} // namespace elizaos
