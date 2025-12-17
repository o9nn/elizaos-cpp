#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Utility functions for date and time operations
 */

/**
 * Checks if a team member is available at the current time
 * @param workDays Array of work days
 * @param workHours Object containing start and end hours
 * @param timeZone Time zone of the team member
 * @returns Boolean indicating if the member is available
 */
bool isAvailableNow(const std::vector<std::string>& workDays, const std::any& workHours, const std::string& timeZone);

/**
 * Calculates the next check-in time for a team member
 * @param workDays Array of work days
 * @param workHours Object containing start and end hours
 * @param timeZone Time zone of the team member
 * @param frequencyHours How often to check in (in hours)
 * @returns Date object for the next check-in
 */
Date calculateNextCheckIn(const std::vector<std::string>& workDays, const std::any& workHours, const std::string& timeZone, double frequencyHours = 24);

/**
 * Formats a date to a human-readable string
 * @param date Date to format
 * @param timeZone Optional timezone
 * @returns Formatted date string
 */
std::string formatDate(Date date, std::optional<std::string> timeZone);

/**
 * Calculates if a project is on track, at risk, or delayed
 * @param completionPercentage Percentage of completed tasks
 * @param elapsedTimePercentage Percentage of elapsed time
 * @returns Status string
 */
std::string getProjectStatus(double completionPercentage, double elapsedTimePercentage);

} // namespace elizaos
