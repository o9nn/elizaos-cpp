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
    std::string label;
    std::string value;
    std::string description;
    std::string intent;
    std::string icon;
    std::optional<std::string> tag;
    std::optional<std::string> direction;
};

struct ChartDataPoint {
    std::string date;
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
    std::string name;
    std::string handle;
    std::string streak;
    double points;
    std::string avatar;
    std::optional<bool> featured;
    std::optional<std::string> subtitle;
};

struct SecurityStatus {
    std::string title;
    std::string value;
    std::string status;
    std::string variant;
};

struct Notification {
    std::string id;
    std::string title;
    std::string message;
    std::string timestamp;
    std::string type;
    bool read;
    std::string priority;
};

struct WidgetData {
    std::string location;
    std::string timezone;
    std::string temperature;
    std::string weather;
    std::string date;
};

struct MockData {
    std::vector<DashboardStat> dashboardStats;
    ChartData chartData;
    std::vector<RebelRanking> rebelsRanking;
    std::vector<SecurityStatus> securityStatus;
    std::vector<Notification> notifications;
    WidgetData widgetData;
};

using TimePeriod = std::string;

} // namespace elizaos
