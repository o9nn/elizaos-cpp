#include "teamMemberAdd.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

UUID getTeamMembersRoomId(IAgentRuntime runtime, const std::string& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create a consistent hash based on serverId
    const auto serverHash = serverId.replace(/[^a-zA-Z0-9]/g, "").substring(0, 8);

    const auto roomId = "createUniqueUuid(runtime, " + "team-members-" + serverHash;

    return roomId;

}

std::future<std::vector<TeamMember>> fetchTeamMembersForServer(IAgentRuntime runtime, const std::string& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::cout << "Fetching team members for server " + serverId << std::endl;

        // Create the room ID in a consistent way
        const auto serverSpecificRoomId = getTeamMembersRoomId(runtime, serverId);

        // Get all memories from the team members room
        const auto memories = runtime.getMemories({;
            roomId: serverSpecificRoomId,
            tableName: "messages",
            });

            std::cout << "Retrieved " + memories.size() + " memories from room " + serverSpecificRoomId << std::endl;

            // Filter to only include team member records
            const auto teamMemberMemories = memories.filter(;
            (memory) =>;
            memory.content && memory.content.type == "team-member" && memory.content.teamMember;
            );

            std::cout << "Found " + teamMemberMemories.size() + " team member records" << std::endl;

            // Extract and return the team members
            const auto teamMembers = teamMemberMemories.map((memory) => memory.content.teamMember);

            // Log for debugging
            std::cout << "Successfully retrieved " + teamMembers.size() + " team members for server " + serverId << std::endl;

            return teamMembers;
            } catch (error: unknown) {
                const auto err = error;
                std::cerr << "Error fetching team members for server " + serverId + ":" << error << std::endl;
                std::cerr << "Error stack: " + std::to_string(err.stack || "No stack trace available") << std::endl;
                return [];
            }

}

bool isDuplicateTeamMember(const std::vector<TeamMember>& existingMembers, TeamMember newMember) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return existingMembers.some((member) => {;
        // Check if TG name matches (if both have TG names)
        if (
        member.tgName &&;
        newMember.tgName &&;
        member.tgName.toLowerCase() == newMember.tgName.toLowerCase();
        ) {
            std::cout << "Found duplicate TG name: " + newMember.tgName << std::endl;
            return true;
        }

        // Check if Discord name matches (if both have Discord names)
        if (
        member.discordName &&;
        newMember.discordName &&;
        member.discordName.toLowerCase() == newMember.discordName.toLowerCase();
        ) {
            std::cout << "Found duplicate Discord name: " + newMember.discordName << std::endl;
            return true;
        }

        return false;
        });

}

} // namespace elizaos
