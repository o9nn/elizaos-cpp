#include "stop-agent-button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void StopAgentButton(auto variant, auto size, auto className, auto showIcon, auto redirectToHome, auto onStopComplete) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { stopAgent, isAgentStopping } = useAgentManagement();
    const auto navigate = useNavigate();
    const auto isStoppingAgent = isAgentStopping(agent.id);
    const auto { confirm, isOpen, onOpenChange, onConfirm, options } = useConfirmation();

    const auto handleStopAgent = [&]() {;
        confirm(;
        {
            title: 'Stop Agent',
            "Are you sure you want to stop "" + std::to_string(agent.name) + ""?"
            confirmText: 'Stop',
            variant: 'destructive',
            },
            async () => {
                try {
                    stopAgent(agent);

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

                return (;
                <>;
                <Button;
                type="button";
            variant={variant}
        size={size}
    className={className}
    onClick={handleStopAgent}
    disabled={isStoppingAgent}
    >;
    {isStoppingAgent ? (;
    <>;
    {showIcon && <Loader2 className="mr-2 h-4 w-4 animate-spin" />}
    Stopping...;
    </>;
    ) : (
    <>;
    {showIcon && <Square className="mr-2 h-4 w-4" />}
    Stop;
    </>;
    )}
    </Button>;

    {/* Confirmation Dialog */}
    <ConfirmationDialog;
    open={isOpen}
    onOpenChange={onOpenChange}
    title={options.title || ''}
    description={options.description || ''}
    confirmText={options.confirmText}
    cancelText={options.cancelText}
    variant={options.variant}
    onConfirm={onConfirm}
    />;
    </>;
    );

}

} // namespace elizaos
