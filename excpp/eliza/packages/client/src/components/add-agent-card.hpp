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
; // Assuming you have a Card component
;

const AddAgentCard: React.FC = () => {
  const navigate = useNavigate();

  const handleClick = () => {
    navigate('/create'); // Navigate to the agent creation page
  };

  return (
    <Card
      className="w-full h-full cursor-pointer hover:shadow-lg transition-shadow flex flex-col items-center justify-center min-h-[180px] border-2 border-dashed hover:border-primary"
      onClick={handleClick}
      onKeyPress={(e) => e.key === 'Enter' && handleClick()}
      tabIndex={0} // Make it focusable
      role="button"
      aria-label="Create new agent"
      data-testid="add-agent-button"
    >
      <CardContent className="p-6 flex flex-col items-center justify-center text-center">
        <PlusCircle className="w-12 h-12 text-muted-foreground mb-3" />
        <p className="text-sm font-medium text-muted-foreground">Create New Agent</p>
      </CardContent>
    </Card>
  );
};

default AddAgentCard;

} // namespace elizaos
