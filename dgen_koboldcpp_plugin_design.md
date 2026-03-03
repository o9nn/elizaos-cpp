# Dgen/KoboldCpp Integration Plugin for ElizaOS

**Objective**: Create a unified LLM provider plugin for ElizaOS that seamlessly integrates DreamGen's cloud API with KoboldCpp's local GGUF inference, allowing the agent to switch between them dynamically.

## 1. Composition Intent

The user's request `/dgen /koboldcpp` implies a composition of these two skills. In the context of ElizaOS, this translates to a single, unified `IModelProvider` that can leverage both:

-   **DreamGen (`dgen`)**: For high-quality, creative, and character-driven text generation via its cloud API. Best for narrative, role-play, and nuanced dialogue.
-   **KoboldCpp (`koboldcpp`)**: For fast, local, and private inference using GGUF models. Best for general-purpose chat, quick completions, and offline capability.

The composition will be **additive (⊕)**, where the system can choose one provider or the other at runtime, rather than multiplicative (⊗), which would imply a fusion of their outputs.

## 2. Design: `dgen_kobold_provider` Plugin

We will create a new plugin named `dgen_kobold_provider` within the `cpp/packages/plugins/` directory. This plugin will register two new model providers with the ElizaOS runtime.

### 2.1. `ModelProviderName` Enum Extension

First, we will extend the `ModelProviderName` enum in `cpp/packages/plugins/plugin_specification/core-plugin-v1/src/types.hpp` to include our new providers:

```cpp
enum ModelProviderName {
  // ... existing providers
  DREAMGEN = "dreamgen",
  KOBOLDCPP = "koboldcpp"
}
```

### 2.2. Plugin Structure

The plugin will be located at `cpp/packages/plugins/dgen_kobold_provider/` and will have the following structure:

-   `CMakeLists.txt`: Build script for the plugin.
-   `src/`:
    -   `plugin.cpp` / `plugin.hpp`: The main plugin definition, which registers the providers.
    -   `dreamgen_provider.cpp` / `dreamgen_provider.hpp`: Implements the `IModelProvider` interface for the DreamGen API.
    -   `koboldcpp_provider.cpp` / `koboldcpp_provider.hpp`: Implements the `IModelProvider` interface for the KoboldCpp API.
    -   `types.hpp`: Shared types and configuration structures.
    -   `http_client.hpp`: A simple, reusable HTTP client for making API requests (using a library like `cpr` or a standard library implementation).

### 2.3. `IModelProvider` Implementation

Both `DreamGenProvider` and `KoboldCppProvider` will implement the core `useModel` method. The logic will be as follows:

1.  **Receive `GenerateTextParams`**: The method will take the standard text generation parameters.
2.  **Get API Key/Endpoint**: Retrieve the necessary API key (for DreamGen) or endpoint URL (for KoboldCpp) from the character's settings (`character.settings.secrets`).
3.  **Format Request**: Convert the `GenerateTextParams` into the JSON format expected by the target API (OpenAI-compatible format for both).
    -   **DreamGen**: Will leverage the `text` role and `name` fields for character-specific generation, as detailed in the `dgen` skill.
    -   **KoboldCpp**: Will use the standard `user`/`assistant` roles.
4.  **Make HTTP Request**: Use the `HttpClient` to send the POST request.
5.  **Parse Response**: Parse the JSON response and extract the generated text.
6.  **Return `GenerateTextResult`**: Return the result in the standard ElizaOS format.

### 2.4. Configuration (`character.json`)

The user will configure which provider to use and provide credentials in their `character.json` file:

```json
{
  "name": "Eliza",
  "settings": {
    "modelProvider": "dreamgen", // or "koboldcpp"
    "secrets": {
      "DREAMGEN_API_KEY": "your_dgen_api_key",
      "KOBOLDCPP_ENDPOINT": "http://localhost:5001"
    }
  }
}
```

The `getTokenForProvider` function will be updated to retrieve these secrets.

## 3. Implementation Steps

1.  **Create Plugin Directory**: Create the `cpp/packages/plugins/dgen_kobold_provider` directory and `CMakeLists.txt`.
2.  **Modify `types.hpp`**: Add `DREAMGEN` and `KOBOLDCPP` to the `ModelProviderName` enum.
3.  **Implement `HttpClient`**: Create a basic, reusable HTTP client.
4.  **Implement `DreamGenProvider`**: Create the class, implement `useModel`, and handle DreamGen-specific request formatting.
5.  **Implement `KoboldCppProvider`**: Create the class, implement `useModel` for the KoboldCpp API.
6.  **Implement `plugin.cpp`**: Write the main plugin file to register both providers with the runtime.
7.  **Update Root `CMakeLists.txt`**: Add the new plugin to the main build.
8.  **Update `getTokenForProvider`**: Add logic to retrieve the new secrets.
