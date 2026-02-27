#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct DashboardStat {
    std: label;
    std: value;
    std: description;
    std: intent;
    std: icon;
    std::optional<std:> tag;
    std::optional<std:> direction;
};

struct ChartDataPoint {
    std: date;
    double spendings;
    double sales;
    double coffee;
};

struct ChartData {
    std::vector<ChartDataPoint> week;
    std::vector<ChartDataPoint> month;
    std::vector<ChartDataPoint> year;
};

struct RebelRanking {
    double id;
    std: name;
    std: handle;
    std: streak;
    double points;
    std: avatar;
    std::optional<bool> featured;
    std::optional<std:> subtitle;
};

struct SecurityStatus {
    std: title;
    std: value;
    std: status;
    std: std::variant;
};

struct Notification {
    std: id;
    std: title;
    std: message;
    std: timestamp;
    std: type;
    bool read;
    std: priority;
};

struct WidgetData {
    std: location;
    std: timezone;
    std: temperature;
    std: weather;
    std: date;
};

struct MockData {
    std::vector<DashboardStat> dashboardStats;
    ChartData chartData;
    std::vector<RebelRanking> rebelsRanking;
    std::vector<SecurityStatus> securityStatus;
    std::vector<Notification> notifications;
    WidgetData widgetData;
};

using TimePeriod = std:;

} // namespace elizaos
