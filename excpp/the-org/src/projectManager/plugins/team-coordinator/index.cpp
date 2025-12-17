#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void initialize(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Initialize services
    new CheckInService(runtime);
    // new ScheduleService(runtime);

    // Return actions
    return {
        actions: [
        // checkInFormatAction,
        recordCheckInAction,
        teamMemberUpdatesAction,
        listCheckInSchedules,
        generateReport,
        addTeamMemberAction,
        listTeamMembersAction,
        updatesFormatAction,
        ],
        };

}

} // namespace elizaos
