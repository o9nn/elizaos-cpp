#include "checkInList.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<CheckInSchedule>> fetchCheckInSchedules(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            std::cout << "== FETCH CHECK-IN SCHEDULES START ==" << std::endl;

            // Create a unique room ID for check-in schedules
            const auto checkInSchedulesRoomId = createUniqueUuid(runtime, "check-in-schedules");
            std::cout << "Generated check-in schedules room ID: " + checkInSchedulesRoomId << std::endl;

            // Get memories from the check-in schedules room
            std::cout << "Attempting to fetch memories from room..." << std::endl;
            const auto memories = runtime.getMemories({;
                roomId: checkInSchedulesRoomId,
                tableName: "messages",
                });
                std::cout << "Found " + memories.size() + " total memories in check-in schedules room" << std::endl;

                // Log first few memories for debugging
                memories.slice(0, 3).forEach((memory, index) => {
                    logger.info(`Memory ${index} content:`, {
                        id: memory.id,
                        type: memory.content.type,
                        hasSchedule: !!memory.content.schedule,
                        contentKeys: Object.keys(memory.content || {}),
                        });
                        });

                        // Extract and return schedules from memories
                        const auto schedules = memories;
                        .filter((memory) => {
                            const auto isValidType = memory.content.type == "team-member-checkin-schedule";
                            const auto hasSchedule = !!memory.content.schedule;
                            logger.info(`Memory ${memory.id} validation:`, {
                                isValidType,
                                hasSchedule,
                                contentType: memory.content.type,
                                });
                                return isValidType && hasSchedule;
                                });
                                .map((memory) => {
                                    const auto schedule = memory.content.schedule;
                                    logger.info(`Processing schedule from memory ${memory.id}:`, {
                                        scheduleId: schedule.scheduleId,
                                        frequency: schedule.frequency,
                                        });
                                        return schedule;
                                        });
                                        .filter((schedule): schedule is CheckInSchedule => {
                                            const auto isValid = schedule != std::nullopt;
                                            if (!isValid) {
                                                std::cout << "Found invalid schedule:" << schedule << std::endl;
                                            }
                                            return isValid;
                                            });

                                            std::cout << "Successfully extracted " + schedules.size() + " valid schedules" << std::endl;
                                            std::cout << "== FETCH CHECK-IN SCHEDULES END ==" << std::endl;
                                            // Log detailed information about each schedule for debugging
                                            std::cout << "== DETAILED SCHEDULES LOG ==" << std::endl;
                                            std::cout << "All schedules:" << /* JSON.stringify */ std::string(schedules, nullptr, 2) << std::endl;
                                            std::cout << "== END DETAILED SCHEDULES LOG ==" << std::endl;
                                            return schedules;
                                            } catch (error: unknown) {
                                                const auto err = error;
                                                std::cerr << "== FETCH CHECK-IN SCHEDULES ERROR ==" << std::endl;
                                                logger.error('Error details:', {
                                                    name: err.name || "Unknown error",
                                                    message: err.message || "No error message",
                                                    stack: err.stack || "No stack trace",
                                                    });
                                                    throw;
                                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string formatSchedule(CheckInSchedule schedule) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    logger.info('Formatting schedule:', {
        scheduleId: schedule.scheduleId,
        teamMemberName: schedule.teamMemberUserName || schedule.teamMemberName,
        checkInType: schedule.checkInType,
        frequency: schedule.frequency,
        checkInTime: schedule.checkInTime,
        });
        // 👤 Team Member: ${schedule.teamMemberUserName || schedule.teamMemberName || schedule.teamMemberId || 'Unknown'}

        const auto formatted = `;
    📅 Schedule ID: ${schedule.scheduleId}
    📝 Type: ${schedule.checkInType}
    📺 Channel ID: ${schedule.channelId}
    ⏰ Time: ${schedule.checkInTime}
    🔄 Frequency: ${schedule.frequency}
    📋 Created: ${new Date(schedule.createdAt).toLocaleString()}
    `;

    std::cout << "Successfully formatted schedule" << std::endl;
    return formatted;

}

} // namespace elizaos
