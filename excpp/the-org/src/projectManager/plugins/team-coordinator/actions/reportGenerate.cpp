#include "reportGenerate.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> generateTeamReport(IAgentRuntime runtime, const std::string& standupType, std::optional<std::string> roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            std::cout << "== GENERATE TEAM REPORT START ==" << std::endl;
            std::cout << "Generating report for standup type: " + standupType << std::endl;

            const auto roomIdLocal = createUniqueUuid(runtime, "report-channel-config");

            // Get all messages from the room that match the standup type
            const auto memories = runtime.getMemories({;
                tableName: "messages",
                agentId: runtime.agentId,
                });

                std::cout << "Retrieved " + memories.size() + " total messages from room" << std::endl;

                // Filter for team member updates with matching standup type
                const auto updates = memories;
                .filter((memory) => {
                    const auto content = memory.content as {;
                        type?: string;
                        update?: TeamMemberUpdate;
                        };
                        const auto contentType = content.type;
                        const auto requestedType = standupType.toLowerCase();
                        const auto checkInType = content.update.checkInType;

                        return contentType == "team-member-update";
                        // && checkInType === standupType
                        });
                        .map((memory) => (memory.content as { update: TeamMemberUpdate }).update)
                        .filter((update): update is TeamMemberUpdate => !!update)
                        .sort((a, b) => new Date(b.timestamp).getTime() - new Date(a.timestamp).getTime());

                        std::cout << "Found " + updates.size() + " updates matching standup type: " + standupType << std::endl;

                        // Generate the report
                        auto report = "📊 **Team Progress Report - " + standupType + " Standups**\n\n";

                        if (updates.length == 0) {
                            "report += " + "No updates found for \"" + standupType + "\" standups in this room.\n";
                            return report;
                        }

                        // Group updates by team member
                        const std::unordered_map<std::string, std::vector<TeamMemberUpdate>> updatesByMember = {};
                        for (const auto& update : updates)
                            logger.info(
                            "Processing update for team member: " + std::to_string(update.teamMemberName || "Unknown") + " (" + update.teamMemberId + ")"
                            );
                            if (!updatesByMember[update.teamMemberId]) {
                                updatesByMember[update.teamMemberId] = [];
                            }
                            updatesByMember[update.teamMemberId].push_back(update);
                        }

                        // Generate report for each team member
                        for (const int [teamMemberId, memberUpdates] of Object.entries(updatesByMember)) {
                            const auto teamMemberName = memberUpdates[0].teamMemberName || "Unknown";
                            std::cout << "Generating report section for: " + teamMemberName + " (" + teamMemberId + ")" << std::endl;
                            "report += " + "👤 **" + teamMemberName + "** (ID: " + teamMemberId + ")\n\n"

                            // Prepare update data for analysis, converting answers JSON to objects
                            const auto processedUpdates = memberUpdates.map((update) => {;
                                try {
                                    // Parse the JSON string to get the actual answers
                                    const auto answers = update.answers ? /* JSON.parse */ update.answers : {};

                                    return {
                                        teamMemberId: update.teamMemberId,
                                        teamMemberName: update.teamMemberName,
                                        serverName: update.serverName,
                                        checkInType: update.checkInType,
                                        timestamp: update.timestamp,
                                        answers,
                                        };
                                        } catch (error) {
                                            std::cerr << "Error parsing answers JSON:" << error << std::endl;
                                            return update;
                                        }
                                        });

                                        // Create prompt for analysis
                                        const auto prompt = `Analyze these team member updates and provide a detailed productivity report.;

                                        The "answers" field contains all the update information in a question-answer format.;

                                        Highlight the following in your analysis:
                                        1. Overall Progress: What major tasks/milestones were completed?
                                        2. Current Focus: What are they actively working on?
                                        3. Productivity Analysis: Are they meeting deadlines? Any patterns in their work?
                                        4. Blockers Impact: How are blockers affecting their progress?
                                        5. Recommendations: What could improve their productivity?

                                        "Updates data: ${/* JSON.stringify */ std::string(processedUpdates, nullptr, 2)}"

                                        std::cout << "Generating productivity analysis for team member:" << teamMemberName << std::endl;

                                        try {
                                            const auto analysis = runtime.useModel(ModelType.TEXT_LARGE, {;
                                                prompt,
                                                stopSequences: [],
                                                });

                                                "report += " + "📋 **Productivity Analysis**:\n" + analysis + "\n\n"
                                                "report += " + "📅 **Recent Updates**:\n"

                                                // Add last 3 updates for reference
                                                const auto recentUpdates = memberUpdates.slice(0, 3);
                                                for (const auto& update : recentUpdates)
                                                    "report += " + "\n🕒 " + std::to_string(new Date(update.timestamp).toLocaleString()) + "\n";

                                                    try {
                                                        const auto answers = update.answers ? /* JSON.parse */ update.answers : {};

                                                        // Display all answers from the update
                                                        for (const int [question, answer] of Object.entries(answers)) {
                                                            "report += " + "▫️ **" + question + "**: " + answer + "\n"
                                                        }
                                                        } catch (error) {
                                                            std::cerr << "Error parsing answers JSON for display:" << error << std::endl;
                                                            "report += " + "▫️ Error parsing update details\n";
                                                        }
                                                    }
                                                    } catch (error) {
                                                        std::cerr << "Error generating analysis:" << error << std::endl;
                                                        report += "❌ Error generating analysis. Showing raw updates:\n\n";

                                                        for (const auto& update : memberUpdates)
                                                            "report += " + "Update from " + std::to_string(new Date(update.timestamp).toLocaleString()) + ":\n"

                                                            try {
                                                                const auto answers = update.answers ? /* JSON.parse */ update.answers : {};

                                                                // Display all answers from the update
                                                                for (const int [question, answer] of Object.entries(answers)) {
                                                                    "report += " + "▫️ **" + question + "**: " + answer + "\n"
                                                                }
                                                                } catch (error) {
                                                                    std::cerr << "Error parsing answers JSON for display:" << error << std::endl;
                                                                    "report += " + "▫️ Error parsing update details\n";
                                                                }
                                                            }
                                                        }
                                                        report += "\n-------------------\n\n";
                                                    }

                                                    std::cout << "Successfully generated team report" << std::endl;
                                                    std::cout << "== GENERATE TEAM REPORT END ==" << std::endl;
                                                    return report;
                                                    } catch (error) {
                                                        std::cerr << "Error generating team report:" << error << std::endl;
                                                        throw;
                                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
