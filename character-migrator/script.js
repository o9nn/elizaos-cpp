// Registry configuration - centralized for maintainability
const REGISTRY_ORG = "elizaos-plugins";
const REGISTRY_REPO = "registry";
const REGISTRY_URL = `https://raw.githubusercontent.com/${REGISTRY_ORG}/${REGISTRY_REPO}/refs/heads/main/generated-registry.json`;

// Hardcoded fallback plugins
const FALLBACK_PLUGINS = [
  "@elizaos/plugin-sql",
  "@elizaos/plugin-local-ai",
  "@elizaos/plugin-0g",
  "@elizaos/plugin-allora",
  "@elizaos/plugin-ankr",
  "@elizaos/plugin-anthropic",
  "@elizaos/plugin-aptos",
  "@elizaos/plugin-arthera",
  "@elizaos/plugin-avalanche",
  "@elizaos/plugin-bedrock",
  "@elizaos/plugin-binance",
  "@elizaos/plugin-bnb",
  "@elizaos/plugin-bootstrap",
  "@elizaos/plugin-browser",
  "@elizaos/plugin-cardano",
  "@elizaos/plugin-coinbase",
  "@elizaos/plugin-coingecko",
  "@elizaos/plugin-discord",
  "@elizaos/plugin-echochambers",
  "@elizaos/plugin-elevenlabs",
  "@elizaos/plugin-evm",
  "@elizaos/plugin-farcaster",
  "@elizaos/plugin-ferePro",
  "@elizaos/plugin-flow",
  "@elizaos/plugin-fuel",
  "@elizaos/plugin-gelato",
  "@elizaos/plugin-giphy",
  "@elizaos/plugin-gitbook",
  "@elizaos/plugin-gitcoin-passport",
  "@elizaos/plugin-goat",
  "@elizaos/plugin-google-genai",
  "@elizaos/plugin-goplus",
  "@elizaos/plugin-grix",
  "@elizaos/plugin-groq",
  "@elizaos/plugin-hedera",
  "@elizaos/plugin-holdstation",
  "@elizaos/plugin-hyperbolic",
  "@elizaos/plugin-hyperliquid",
  "@elizaos/plugin-injective",
  "@elizaos/plugin-intiface",
  "@elizaos/plugin-irys",
  "@elizaos/plugin-kaia",
  "@elizaos/plugin-knowledge",
  "@elizaos/plugin-lensNetwork",
  "@elizaos/plugin-livekit",
  "@elizaos/plugin-mcp",
  "@elizaos/plugin-messari-ai-toolkit",
  "@elizaos/plugin-mina",
  "@elizaos/plugin-movement",
  "@elizaos/plugin-multiversx",
  "@elizaos/plugin-near",
  "@elizaos/plugin-node",
  "@elizaos/plugin-ollama",
  "@elizaos/plugin-openai",
  "@elizaos/plugin-openrouter",
  "@elizaos/plugin-orderly",
  "@elizaos/plugin-pdf",
  "@elizaos/plugin-pyth-data",
  "@elizaos/plugin-raiinmaker",
  "@elizaos/plugin-redpill",
  "@elizaos/plugin-sei",
  "@elizaos/plugin-solana",
  "@elizaos/plugin-spheron",
  "@elizaos/plugin-squid-router",
  "@elizaos/plugin-stargaze",
  "@elizaos/plugin-starknet",
  "@elizaos/plugin-storacha",
  "@elizaos/plugin-storage-s3",
  "@elizaos/plugin-story",
  "@elizaos/plugin-sui",
  "@elizaos/plugin-tee",
  "@elizaos/plugin-telegram",
  "@elizaos/plugin-thirdweb",
  "@elizaos/plugin-ton",
  "@elizaos/plugin-twitter",
  "@elizaos/plugin-venice",
  "@elizaos/plugin-video-understanding",
  "@elizaos/plugin-whatsapp",
  "@elizaos/plugin-xmtp",
];

// Global variable to store available plugins
let availablePlugins = [];

// Hardcoded mappings for providers/clients that don't match plugin names directly
const PROVIDER_PLUGIN_MAPPINGS = {
  google: "@elizaos/plugin-google-genai",
  llama_local: "@elizaos/plugin-ollama",
  // Add more mappings as needed
};

const CLIENT_PLUGIN_MAPPINGS = {
  // Add client mappings as needed
};

// Function to fetch plugins from registry
async function fetchPlugins() {
  try {
    const response = await fetch(REGISTRY_URL);
    const registryData = await response.json();

    // Extract plugin names from registry that support v1 and are plugins
    const registryPlugins = Object.entries(registryData.registry || {})
      .filter(([name, data]) => {
        // Check if it's a plugin and has v1 support
        const isPlugin = name.includes("plugin");
        const hasV1Support = data.supports.v1 === true;
        const hasV1Version =
          data.npm.v1 !== null ||
          (data.git.v1.version !== null && data.git.v1.branch !== null);

        return isPlugin && hasV1Support && hasV1Version;
      })
      .map(([name]) => name.replace(/^@elizaos-plugins\//, "@elizaos/"))
      .sort();

    availablePlugins = registryPlugins;
    console.log(
      "Successfully fetched plugins from registry:",
      registryPlugins.length
    );
  } catch (error) {
    console.warn(
      "Failed to fetch from registry, using fallback plugins:",
      error
    );
    availablePlugins = FALLBACK_PLUGINS;
  }
}

// Function to match V1 clients/modelProvider with available plugins
function matchPlugins(v1Character) {
  const matchedPlugins = new Set();

  // Check clients
  if (v1Character.clients && Array.isArray(v1Character.clients)) {
    v1Character.clients.forEach((client) => {
      if (typeof client === "string") {
        const clientLower = client.toLowerCase();

        // Check hardcoded mappings first
        if (CLIENT_PLUGIN_MAPPINGS[clientLower]) {
          const mappedPlugin = CLIENT_PLUGIN_MAPPINGS[clientLower];
          if (availablePlugins.includes(mappedPlugin)) {
            matchedPlugins.add(mappedPlugin);
          }
        } else {
          // Fall back to direct mapping
          const pluginName = `@elizaos/plugin-${clientLower}`;
          if (availablePlugins.includes(pluginName)) {
            matchedPlugins.add(pluginName);
          }
        }
      }
    });
  }

  // Check modelProvider
  if (
    v1Character.modelProvider &&
    typeof v1Character.modelProvider === "string"
  ) {
    const providerLower = v1Character.modelProvider.toLowerCase();
    let providerMatched = false;

    // Check hardcoded mappings first
    if (PROVIDER_PLUGIN_MAPPINGS[providerLower]) {
      const mappedPlugin = PROVIDER_PLUGIN_MAPPINGS[providerLower];
      if (availablePlugins.includes(mappedPlugin)) {
        matchedPlugins.add(mappedPlugin);
        providerMatched = true;
      }
    } else {
      // Fall back to direct mapping
      const pluginName = `@elizaos/plugin-${providerLower}`;
      if (availablePlugins.includes(pluginName)) {
        matchedPlugins.add(pluginName);
        providerMatched = true;
      }
    }

    // If no provider matched, default to OpenAI
    if (!providerMatched) {
      matchedPlugins.add("@elizaos/plugin-openai");
    }
  } else {
    // If no modelProvider specified, default to OpenAI
    if (availablePlugins.includes("@elizaos/plugin-openai")) {
      matchedPlugins.add("@elizaos/plugin-openai");
    }
  }

  return Array.from(matchedPlugins).sort();
}

async function convert() {
  try {
    const inputText = document.getElementById("input").value.trim();
    const outputActions = document.getElementById("outputActions");

    // Hide buttons initially
    outputActions.classList.remove("show");

    if (!inputText) {
      showNotification("Please enter V1 JSON data first", "error");
      return;
    }

    const v1 = JSON.parse(inputText);

    // Ensure plugins are loaded
    if (availablePlugins.length === 0) {
      showNotification("Loading plugins...", "success");
      await fetchPlugins();
    }

    // Combine bio and lore
    const bio = (v1.bio || []).concat(v1.lore || []);

    // Transform messageExamples
    const messageExamples = (v1.messageExamples || []).map((thread) =>
      thread.map((msg) => ({
        name: msg.user,
        content: msg.content,
      }))
    );

    // Match plugins based on V1 clients/modelProvider
    const matchedPlugins = matchPlugins(v1);

    // Always include these essential plugins if they're not already present
    const essentialPlugins = [
      "@elizaos/plugin-sql",
      "@elizaos/plugin-bootstrap",
    ];
    const finalPlugins = [
      ...new Set([...matchedPlugins, ...essentialPlugins]),
    ].sort();

    // Build v2 character
    const v2 = {
      name: v1.name,
      plugins: finalPlugins,
      settings: v1.settings,
      system: v1.system,
      bio: bio,
      topics: v1.topics,
      messageExamples: messageExamples,
      postExamples: v1.postExamples,
      style: v1.style,
    };

    document.getElementById("output").value = JSON.stringify(v2, null, 2);

    const pluginMessage =
      matchedPlugins.length > 0
        ? ` Found ${
            matchedPlugins.length
          } matching plugins: ${matchedPlugins.join(", ")}`
        : " No matching plugins found";

    showNotification(
      "Successfully converted V1 to V2!" + pluginMessage,
      "success"
    );

    // Show buttons after successful conversion
    outputActions.classList.add("show");
  } catch (e) {
    // Hide buttons on error
    const outputActions = document.getElementById("outputActions");
    const outputField = document.getElementById("output");

    outputActions.classList.remove("show");
    outputField.value = ""; // Clear output on error

    showNotification(
      "Invalid JSON or error in conversion: " + e.message,
      "error"
    );
  }
}

function copyToClipboard(elementId) {
  const element = document.getElementById(elementId);
  const text = element.value;

  if (!text.trim()) {
    showNotification("Nothing to copy", "error");
    return;
  }

  navigator.clipboard
    .writeText(text)
    .then(() => {
      const type = elementId === "input" ? "Input" : "Output";
      showNotification(`${type} copied to clipboard!`, "success");
    })
    .catch((err) => {
      // Fallback for older browsers
      element.select();
      document.execCommand("copy");
      const type = elementId === "input" ? "Input" : "Output";
      showNotification(`${type} copied to clipboard!`, "success");
    });
}

function downloadJSON(elementId, filename) {
  const element = document.getElementById(elementId);
  const text = element.value;

  if (!text.trim()) {
    showNotification("Nothing to download", "error");
    return;
  }

  try {
    // Validate JSON before downloading
    JSON.parse(text);

    const blob = new Blob([text], { type: "application/json" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);

    showNotification(`Downloaded ${filename}`, "success");
  } catch (e) {
    showNotification("Invalid JSON - cannot download", "error");
  }
}

function showNotification(message, type) {
  const notification = document.getElementById("notification");
  notification.textContent = message;
  notification.className = `notification ${type}`;
  notification.style.display = "block";

  setTimeout(() => {
    notification.style.display = "none";
  }, 3000);
}

// Initialize plugins on page load
document.addEventListener("DOMContentLoaded", function () {
  fetchPlugins();

  // Add event listener to hide output actions when input is cleared
  const inputField = document.getElementById("input");
  const outputActions = document.getElementById("outputActions");
  const outputField = document.getElementById("output");

  inputField.addEventListener("input", function () {
    if (!inputField.value.trim()) {
      outputActions.classList.remove("show");
      outputField.value = "";
    }
  });
});
