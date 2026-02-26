#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/docs/docs/rest/sidebar.h"

std::shared_ptr<SidebarsConfig> sidebar = object{
    object::pair{std::string("apisidebar"), array<object>{ object{
        object::pair{std::string("type"), std::string("doc")}, 
        object::pair{std::string("id"), std::string("rest/eliza-os-api")}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("system")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-ping")}, 
            object::pair{std::string("label"), std::string("Ping health check")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-hello")}, 
            object::pair{std::string("label"), std::string("Basic health check")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-status")}, 
            object::pair{std::string("label"), std::string("Get system status")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-health")}, 
            object::pair{std::string("label"), std::string("Health check endpoint")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/stop-server")}, 
            object::pair{std::string("label"), std::string("Stop the server")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-debug-servers")}, 
            object::pair{std::string("label"), std::string("Get server debug info")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-servers")}, 
            object::pair{std::string("label"), std::string("Get server debug info")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-local-env")}, 
            object::pair{std::string("label"), std::string("Get local environment variables")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/update-local-env")}, 
            object::pair{std::string("label"), std::string("Update local environment variables")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("agents")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/list-agents")}, 
            object::pair{std::string("label"), std::string("List all agents")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-agent")}, 
            object::pair{std::string("label"), std::string("Create a new agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-agent")}, 
            object::pair{std::string("label"), std::string("Get agent details")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/update-agent")}, 
            object::pair{std::string("label"), std::string("Update agent")}, 
            object::pair{std::string("className"), std::string("api-method patch")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-agent")}, 
            object::pair{std::string("label"), std::string("Delete an agent")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/start-agent")}, 
            object::pair{std::string("label"), std::string("Start an agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/stop-agent")}, 
            object::pair{std::string("label"), std::string("Stop an agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-agent-panels")}, 
            object::pair{std::string("label"), std::string("Get agent panels")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-all-worlds")}, 
            object::pair{std::string("label"), std::string("Get all worlds")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-agent-world")}, 
            object::pair{std::string("label"), std::string("Create a world for an agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/update-agent-world")}, 
            object::pair{std::string("label"), std::string("Update a world")}, 
            object::pair{std::string("className"), std::string("api-method patch")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-agent-rooms")}, 
            object::pair{std::string("label"), std::string("Get agent rooms")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-room")}, 
            object::pair{std::string("label"), std::string("Create a room")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-room")}, 
            object::pair{std::string("label"), std::string("Get room details")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/update-room")}, 
            object::pair{std::string("label"), std::string("Update a room")}, 
            object::pair{std::string("className"), std::string("api-method patch")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-room")}, 
            object::pair{std::string("label"), std::string("Delete a room")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-room-memories")}, 
            object::pair{std::string("label"), std::string("Get room memories")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/send-message")}, 
            object::pair{std::string("label"), std::string("Send a message to an agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/send-audio-message")}, 
            object::pair{std::string("label"), std::string("Send an audio message")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/synthesize-speech")}, 
            object::pair{std::string("label"), std::string("Convert text to speech")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/generate-speech")}, 
            object::pair{std::string("label"), std::string("Generate speech from text")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/conversation-to-speech")}, 
            object::pair{std::string("label"), std::string("Process conversation and return speech")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("memory")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-room-memories")}, 
            object::pair{std::string("label"), std::string("Get room memories")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-room-memories")}, 
            object::pair{std::string("label"), std::string("Delete all memories for a room")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-agent-memories")}, 
            object::pair{std::string("label"), std::string("Get agent memories")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-all-agent-memories")}, 
            object::pair{std::string("label"), std::string("Delete all agent memories")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/update-agent-memory")}, 
            object::pair{std::string("label"), std::string("Update a memory")}, 
            object::pair{std::string("className"), std::string("api-method patch")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-memory-room")}, 
            object::pair{std::string("label"), std::string("Create a room")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("rooms")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-agent-rooms")}, 
            object::pair{std::string("label"), std::string("Get agent rooms")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-room")}, 
            object::pair{std::string("label"), std::string("Create a room")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-room")}, 
            object::pair{std::string("label"), std::string("Get room details")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/update-room")}, 
            object::pair{std::string("label"), std::string("Update a room")}, 
            object::pair{std::string("className"), std::string("api-method patch")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-room")}, 
            object::pair{std::string("label"), std::string("Delete a room")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-room-memories")}, 
            object::pair{std::string("label"), std::string("Get room memories")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("messaging")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/submit-message")}, 
            object::pair{std::string("label"), std::string("Submit a message")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/complete-message")}, 
            object::pair{std::string("label"), std::string("Complete a message")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/ingest-external-message")}, 
            object::pair{std::string("label"), std::string("Ingest external message")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-central-servers")}, 
            object::pair{std::string("label"), std::string("Get central servers")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-server")}, 
            object::pair{std::string("label"), std::string("Create server")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/add-agent-to-server")}, 
            object::pair{std::string("label"), std::string("Add agent to server")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-server-agents")}, 
            object::pair{std::string("label"), std::string("Get server agents")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/remove-agent-from-server")}, 
            object::pair{std::string("label"), std::string("Remove agent from server")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-server-channels")}, 
            object::pair{std::string("label"), std::string("Get server channels")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-channel")}, 
            object::pair{std::string("label"), std::string("Create channel")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-dm-channel")}, 
            object::pair{std::string("label"), std::string("Get or create DM channel")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-central-channel")}, 
            object::pair{std::string("label"), std::string("Create central channel")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-central-server-channels")}, 
            object::pair{std::string("label"), std::string("Get central server channels")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-channel-details")}, 
            object::pair{std::string("label"), std::string("Get channel details")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-channel-info")}, 
            object::pair{std::string("label"), std::string("Get channel info")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/update-channel")}, 
            object::pair{std::string("label"), std::string("Update channel")}, 
            object::pair{std::string("className"), std::string("api-method patch")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-channel")}, 
            object::pair{std::string("label"), std::string("Delete channel")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-channel-participants")}, 
            object::pair{std::string("label"), std::string("Get channel participants")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/add-agent-to-channel")}, 
            object::pair{std::string("label"), std::string("Add agent to channel")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-channel-messages")}, 
            object::pair{std::string("label"), std::string("Get channel messages")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/send-channel-message")}, 
            object::pair{std::string("label"), std::string("Send message to channel")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-all-channel-messages")}, 
            object::pair{std::string("label"), std::string("Delete all channel messages")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-channel-message")}, 
            object::pair{std::string("label"), std::string("Delete channel message")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-all-user-channel-messages")}, 
            object::pair{std::string("label"), std::string("Delete all channel messages by user")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/create-group-channel")}, 
            object::pair{std::string("label"), std::string("Create group channel")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/process-external-message")}, 
            object::pair{std::string("label"), std::string("Process external message")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("audio")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/send-audio-message")}, 
            object::pair{std::string("label"), std::string("Send an audio message")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/synthesize-speech")}, 
            object::pair{std::string("label"), std::string("Convert text to speech")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/generate-speech")}, 
            object::pair{std::string("label"), std::string("Generate speech from text")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/conversation-to-speech")}, 
            object::pair{std::string("label"), std::string("Process conversation and return speech")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/transcribe-audio")}, 
            object::pair{std::string("label"), std::string("Transcribe audio")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/process-audio-message")}, 
            object::pair{std::string("label"), std::string("Process audio message")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/synthesize-speech")}, 
            object::pair{std::string("label"), std::string("Synthesize speech from text")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/generate-speech")}, 
            object::pair{std::string("label"), std::string("Generate speech from text")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/conversation-to-speech")}, 
            object::pair{std::string("label"), std::string("Convert conversation to speech")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("media")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/upload-agent-media")}, 
            object::pair{std::string("label"), std::string("Upload media for agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/upload-channel-media")}, 
            object::pair{std::string("label"), std::string("Upload media to channel")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("logs")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-logs")}, 
            object::pair{std::string("label"), std::string("Get system logs")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/post-logs")}, 
            object::pair{std::string("label"), std::string("Get system logs (POST)")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/clear-logs")}, 
            object::pair{std::string("label"), std::string("Clear system logs")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/get-agent-logs")}, 
            object::pair{std::string("label"), std::string("Get agent logs")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        }, object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/delete-agent-log")}, 
            object::pair{std::string("label"), std::string("Delete a specific log entry")}, 
            object::pair{std::string("className"), std::string("api-method delete")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("websocket")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/web-socket-connection")}, 
            object::pair{std::string("label"), std::string("WebSocket Connection")}, 
            object::pair{std::string("className"), std::string("api-method get")}
        } }}
    }, object{
        object::pair{std::string("type"), std::string("category")}, 
        object::pair{std::string("label"), std::string("messages")}, 
        object::pair{std::string("items"), array<object>{ object{
            object::pair{std::string("type"), std::string("doc")}, 
            object::pair{std::string("id"), std::string("rest/send-message")}, 
            object::pair{std::string("label"), std::string("Send a message to an agent")}, 
            object::pair{std::string("className"), std::string("api-method post")}
        } }}
    } }}
};

void Main(void)
{
}

MAIN
