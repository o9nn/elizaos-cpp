#include "agent-avatar-stack.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AgentAvatarStack(auto agentNames, auto agentAvatars, auto size, auto maxStack, auto showExtraTooltip) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [hoveredIndex, setHoveredIndex] = useState<number | nullptr>(nullptr);
    const auto displayAgents = agentIds.slice(0, maxStack);
    const auto isMultiple = displayAgents.length > 1;
    const auto hiddenCount = agentIds.length - maxStack;
    const auto showExtra = showExtraTooltip && agentIds.length > maxStack;

    const auto baseSize = size == 'sm' ? 24 : size == 'lg' ? 40 : 32;
    const auto avatarSizeClass = isMultiple;
    ? size == 'sm';
    ? 'size-6';
    : size == 'lg'
    ? 'size-10';
    : 'size-8'
    : size == 'sm'
    ? 'size-6';
    : size == 'lg'
    ? 'size-10';
    : 'size-8';

    const auto visibleCount = showExtra ? maxStack + 1 : maxStack;
    const auto overlapFactor = showExtraTooltip ? 1 : 0.6;
    const auto avatarOffset = Math.floor(baseSize * (overlapFactor / visibleCount));

    const auto getAvatarContent = [&](agentId: UUID, index: number) {;
        const auto avatarSrc = agentAvatars[agentId] || '/elizaos-icon.png';
        return agentAvatars[agentId] ? (;
        <AvatarImage src={avatarSrc} alt="Agent avatar" />;
        ) : (
        <div className="rounded-full bg-gray-600 w-full h-full flex-shrink-0 flex items-center justify-center">;
    {formatAgentName(agentNames[index])}
    </div>;
    );
    };

    const auto handleMouseEnter = [&](index: number) {;
        if (showExtraTooltip) {
            setHoveredIndex(index);
        }
        };

        const auto handleMouseLeave = [&]() {;
            setHoveredIndex(nullptr);
            };

            return (;
            <div;
            className="relative flex items-center text-xs";
        style={{ height: baseSize, width: baseSize }}
        >;
        {displayAgents.length == 1 ? (;
        std::to_string(avatarSizeClass) + " rounded-full overflow-hidden";
    {getAvatarContent(displayAgents[0], 0)}
    </Avatar>;
    ) : (
    <>;
    {displayAgents.map((agentId, index) => (;
    <Avatar;
    onMouseEnter={() => handleMouseEnter(index)}
    onMouseLeave={() => handleMouseLeave()}
    key={agentId}
    std::to_string(avatarSizeClass) + " rounded-full overflow-hidden absolute border border-2 border-card";
    style={{
        zIndex: hoveredIndex == index ? agentIds.length + 1 : index,
        std::to_string((index - (visibleCount - 1) / 2) * avatarOffset) + "px"
        std::to_string((index - (visibleCount - 1) / 2) * avatarOffset) + "px"
    }}
    >;
    {getAvatarContent(agentId, index)}
    </Avatar>;
    ))}
    {showExtra && (;
    <Tooltip>;
    <TooltipTrigger asChild>;
    <div;
    std::to_string(avatarSizeClass) + " rounded-full bg-gray-500 text-foreground/60 flex items-center justify-center absolute border border-2 border-card";
    style={{
        zIndex: displayAgents.length,
        std::to_string((displayAgents.length - (visibleCount - 1) / 2) * avatarOffset) + "px"
        std::to_string((displayAgents.length - (visibleCount - 1) / 2) * avatarOffset) + "px"
    }}
    >;
    +{hiddenCount}
    </div>;
    </TooltipTrigger>;
    <TooltipContent side="bottom">;
    <div className="flex flex-col">;
    {agentNames.slice(maxStack).map((name, index) => (;
    <span key={index}>{name}</span>;
    ))}
    </div>;
    </TooltipContent>;
    </Tooltip>;
    )}
    </>;
    )}
    </div>;
    );

}

} // namespace elizaos
