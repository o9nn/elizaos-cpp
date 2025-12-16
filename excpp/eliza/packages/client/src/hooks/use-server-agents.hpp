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
;
import type { UUID } from '@elizaos/core';

 = useToast();

  return useMutation({
    mutationFn: async ({ serverId, agentId }: { serverId: UUID; agentId: UUID }) => {
      const elizaClient = createElizaClient();
      return await elizaClient.agents.addAgentToServer(serverId, agentId);
    },
    onSuccess: (_data, variables) => {
      // Invalidate server agents query
      queryClient.invalidateQueries({ queryKey: ['serverAgents', variables.serverId] });
      queryClient.invalidateQueries({ queryKey: ['agentServers', variables.agentId] });

      toast({
        title: 'Agent Added',
        description: 'Agent has been successfully added to the server',
      });
    },
    onError: (error) => {
      toast({
        title: 'Error',
        description: error instanceof Error ? error.message : 'Failed to add agent to server',
        variant: 'destructive',
      });
    },
  });
}

 = useToast();

  return useMutation({
    mutationFn: async ({ serverId, agentId }: { serverId: UUID; agentId: UUID }) => {
      const elizaClient = createElizaClient();
      return await elizaClient.agents.removeAgentFromServer(serverId, agentId);
    },
    onSuccess: (_data, variables) => {
      // Invalidate server agents query
      queryClient.invalidateQueries({ queryKey: ['serverAgents', variables.serverId] });
      queryClient.invalidateQueries({ queryKey: ['agentServers', variables.agentId] });

      toast({
        title: 'Agent Removed',
        description: 'Agent has been successfully removed from the server',
      });
    },
    onError: (error) => {
      toast({
        title: 'Error',
        description: error instanceof Error ? error.message : 'Failed to remove agent from server',
        variant: 'destructive',
      });
    },
  });
}

} // namespace elizaos
