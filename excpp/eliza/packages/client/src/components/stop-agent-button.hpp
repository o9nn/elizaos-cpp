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
;
;
import type { Agent } from '@elizaos/core';
;
;

struct StopAgentButtonProps {
    Agent agent;
    std::optional<'default' | 'destructive' | 'outline'> variant;
    std::optional<'default' | 'sm' | 'lg' | 'icon'> size;
    std::optional<std::string> className;
    std::optional<bool> showIcon;
    std::optional<bool> redirectToHome;
    std::optional<() => void> onStopComplete;
};


default : StopAgentButtonProps) {
  const { stopAgent, isAgentStopping } = useAgentManagement();
  const navigate = useNavigate();
  const isStoppingAgent = isAgentStopping(agent.id);
  const { confirm, isOpen, onOpenChange, onConfirm, options } = useConfirmation();

  const handleStopAgent = () => {
    confirm(
      {
        title: 'Stop Agent',
        description: `Are you sure you want to stop "${agent.name}"?`,
        confirmText: 'Stop',
        variant: 'destructive',
      },
      async () => {
        try {
          await stopAgent(agent);

          // Call the onStopComplete callback if provided
          if (onStopComplete) {
            onStopComplete();
          }

          // Navigate to homepage if redirectToHome is true
          if (redirectToHome) {
            navigate('/');
          }
        } catch (error) {
          // If error occurs, don't navigate or call callback
          clientLogger.error('Error stopping agent:', error);
        }
      }
    );
  };

  return (
    <>
      <Button
        type="button"
        variant={variant}
        size={size}
        className={className}
        onClick={handleStopAgent}
        disabled={isStoppingAgent}
      >
        {isStoppingAgent ? (
          <>
            {showIcon && <Loader2 className="mr-2 h-4 w-4 animate-spin" />}
            Stopping...
          </>
        ) : (
          <>
            {showIcon && <Square className="mr-2 h-4 w-4" />}
            Stop
          </>
        )}
      </Button>

      {/* Confirmation Dialog */}
      <ConfirmationDialog
        open={isOpen}
        onOpenChange={onOpenChange}
        title={options?.title || ''}
        description={options?.description || ''}
        confirmText={options?.confirmText}
        cancelText={options?.cancelText}
        variant={options?.variant}
        onConfirm={onConfirm}
      />
    </>
  );
}

} // namespace elizaos
