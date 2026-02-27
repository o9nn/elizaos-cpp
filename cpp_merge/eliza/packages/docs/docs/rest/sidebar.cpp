#include "sidebar.hpp"

std::shared_ptr<SidebarsConfig> sidebar = object{
    object::pair{std:("apisidebar"), array<object>{ object{
        object::pair{std:("type"), std:("doc")}, 
        object::pair{std:("id"), std:("rest/eliza-os-api")}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("system")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-ping")}, 
            object::pair{std:("label"), std:("Ping health check")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-hello")}, 
            object::pair{std:("label"), std:("Basic health check")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-status")}, 
            object::pair{std:("label"), std:("Get system status")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-health")}, 
            object::pair{std:("label"), std:("Health check endpoint")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/stop-server")}, 
            object::pair{std:("label"), std:("Stop the server")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-debug-servers")}, 
            object::pair{std:("label"), std:("Get server debug info")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-servers")}, 
            object::pair{std:("label"), std:("Get server debug info")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-local-env")}, 
            object::pair{std:("label"), std:("Get local environment variables")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/update-local-env")}, 
            object::pair{std:("label"), std:("Update local environment variables")}, 
            object::pair{std:("className"), std:("api-method post")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("agents")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/list-agents")}, 
            object::pair{std:("label"), std:("List all agents")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-agent")}, 
            object::pair{std:("label"), std:("Create a new agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-agent")}, 
            object::pair{std:("label"), std:("Get agent details")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/update-agent")}, 
            object::pair{std:("label"), std:("Update agent")}, 
            object::pair{std:("className"), std:("api-method patch")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-agent")}, 
            object::pair{std:("label"), std:("Delete an agent")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/start-agent")}, 
            object::pair{std:("label"), std:("Start an agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/stop-agent")}, 
            object::pair{std:("label"), std:("Stop an agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-agent-panels")}, 
            object::pair{std:("label"), std:("Get agent panels")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-all-worlds")}, 
            object::pair{std:("label"), std:("Get all worlds")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-agent-world")}, 
            object::pair{std:("label"), std:("Create a world for an agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/update-agent-world")}, 
            object::pair{std:("label"), std:("Update a world")}, 
            object::pair{std:("className"), std:("api-method patch")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-agent-rooms")}, 
            object::pair{std:("label"), std:("Get agent rooms")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-room")}, 
            object::pair{std:("label"), std:("Create a room")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-room")}, 
            object::pair{std:("label"), std:("Get room details")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/update-room")}, 
            object::pair{std:("label"), std:("Update a room")}, 
            object::pair{std:("className"), std:("api-method patch")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-room")}, 
            object::pair{std:("label"), std:("Delete a room")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-room-memories")}, 
            object::pair{std:("label"), std:("Get room memories")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/send-message")}, 
            object::pair{std:("label"), std:("Send a message to an agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/send-audio-message")}, 
            object::pair{std:("label"), std:("Send an audio message")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/synthesize-speech")}, 
            object::pair{std:("label"), std:("Convert text to speech")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/generate-speech")}, 
            object::pair{std:("label"), std:("Generate speech from text")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/conversation-to-speech")}, 
            object::pair{std:("label"), std:("Process conversation and return speech")}, 
            object::pair{std:("className"), std:("api-method post")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("memory")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-room-memories")}, 
            object::pair{std:("label"), std:("Get room memories")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-room-memories")}, 
            object::pair{std:("label"), std:("Delete all memories for a room")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-agent-memories")}, 
            object::pair{std:("label"), std:("Get agent memories")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-all-agent-memories")}, 
            object::pair{std:("label"), std:("Delete all agent memories")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/update-agent-memory")}, 
            object::pair{std:("label"), std:("Update a memory")}, 
            object::pair{std:("className"), std:("api-method patch")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-memory-room")}, 
            object::pair{std:("label"), std:("Create a room")}, 
            object::pair{std:("className"), std:("api-method post")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("rooms")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-agent-rooms")}, 
            object::pair{std:("label"), std:("Get agent rooms")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-room")}, 
            object::pair{std:("label"), std:("Create a room")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-room")}, 
            object::pair{std:("label"), std:("Get room details")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/update-room")}, 
            object::pair{std:("label"), std:("Update a room")}, 
            object::pair{std:("className"), std:("api-method patch")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-room")}, 
            object::pair{std:("label"), std:("Delete a room")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-room-memories")}, 
            object::pair{std:("label"), std:("Get room memories")}, 
            object::pair{std:("className"), std:("api-method get")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("messaging")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/submit-message")}, 
            object::pair{std:("label"), std:("Submit a message")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/complete-message")}, 
            object::pair{std:("label"), std:("Complete a message")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/ingest-external-message")}, 
            object::pair{std:("label"), std:("Ingest external message")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-central-servers")}, 
            object::pair{std:("label"), std:("Get central servers")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-server")}, 
            object::pair{std:("label"), std:("Create server")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/add-agent-to-server")}, 
            object::pair{std:("label"), std:("Add agent to server")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-server-agents")}, 
            object::pair{std:("label"), std:("Get server agents")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/remove-agent-from-server")}, 
            object::pair{std:("label"), std:("Remove agent from server")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-server-channels")}, 
            object::pair{std:("label"), std:("Get server channels")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-channel")}, 
            object::pair{std:("label"), std:("Create channel")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-dm-channel")}, 
            object::pair{std:("label"), std:("Get or create DM channel")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-central-channel")}, 
            object::pair{std:("label"), std:("Create central channel")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-central-server-channels")}, 
            object::pair{std:("label"), std:("Get central server channels")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-channel-details")}, 
            object::pair{std:("label"), std:("Get channel details")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-channel-info")}, 
            object::pair{std:("label"), std:("Get channel info")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/update-channel")}, 
            object::pair{std:("label"), std:("Update channel")}, 
            object::pair{std:("className"), std:("api-method patch")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-channel")}, 
            object::pair{std:("label"), std:("Delete channel")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-channel-participants")}, 
            object::pair{std:("label"), std:("Get channel participants")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/add-agent-to-channel")}, 
            object::pair{std:("label"), std:("Add agent to channel")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-channel-messages")}, 
            object::pair{std:("label"), std:("Get channel messages")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/send-channel-message")}, 
            object::pair{std:("label"), std:("Send message to channel")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-all-channel-messages")}, 
            object::pair{std:("label"), std:("Delete all channel messages")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-channel-message")}, 
            object::pair{std:("label"), std:("Delete channel message")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-all-user-channel-messages")}, 
            object::pair{std:("label"), std:("Delete all channel messages by user")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/create-group-channel")}, 
            object::pair{std:("label"), std:("Create group channel")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/process-external-message")}, 
            object::pair{std:("label"), std:("Process external message")}, 
            object::pair{std:("className"), std:("api-method post")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("audio")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/send-audio-message")}, 
            object::pair{std:("label"), std:("Send an audio message")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/synthesize-speech")}, 
            object::pair{std:("label"), std:("Convert text to speech")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/generate-speech")}, 
            object::pair{std:("label"), std:("Generate speech from text")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/conversation-to-speech")}, 
            object::pair{std:("label"), std:("Process conversation and return speech")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/transcribe-audio")}, 
            object::pair{std:("label"), std:("Transcribe audio")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/process-audio-message")}, 
            object::pair{std:("label"), std:("Process audio message")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/synthesize-speech")}, 
            object::pair{std:("label"), std:("Synthesize speech from text")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/generate-speech")}, 
            object::pair{std:("label"), std:("Generate speech from text")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/conversation-to-speech")}, 
            object::pair{std:("label"), std:("Convert conversation to speech")}, 
            object::pair{std:("className"), std:("api-method post")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("media")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/upload-agent-media")}, 
            object::pair{std:("label"), std:("Upload media for agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/upload-channel-media")}, 
            object::pair{std:("label"), std:("Upload media to channel")}, 
            object::pair{std:("className"), std:("api-method post")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("logs")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-logs")}, 
            object::pair{std:("label"), std:("Get system logs")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/post-logs")}, 
            object::pair{std:("label"), std:("Get system logs (POST)")}, 
            object::pair{std:("className"), std:("api-method post")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/clear-logs")}, 
            object::pair{std:("label"), std:("Clear system logs")}, 
            object::pair{std:("className"), std:("api-method delete")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/get-agent-logs")}, 
            object::pair{std:("label"), std:("Get agent logs")}, 
            object::pair{std:("className"), std:("api-method get")}
        }, object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/delete-agent-log")}, 
            object::pair{std:("label"), std:("Delete a specific log entry")}, 
            object::pair{std:("className"), std:("api-method delete")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("websocket")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/web-socket-connection")}, 
            object::pair{std:("label"), std:("WebSocket Connection")}, 
            object::pair{std:("className"), std:("api-method get")}
        } }}
    }, object{
        object::pair{std:("type"), std:("category")}, 
        object::pair{std:("label"), std:("messages")}, 
        object::pair{std:("items"), array<object>{ object{
            object::pair{std:("type"), std:("doc")}, 
            object::pair{std:("id"), std:("rest/send-message")}, 
            object::pair{std:("label"), std:("Send a message to an agent")}, 
            object::pair{std:("className"), std:("api-method post")}
        } }}
    } }}
};

void Main(void)
{
}

MAIN
