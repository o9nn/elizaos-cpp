#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_FRONTEND_TYPES_DASHBOARD_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_FRONTEND_TYPES_DASHBOARD_H
#include "core.hpp"

typedef any TimePeriod;

class DashboardStat;
class ChartDataPoint;
class ChartData;
class RebelRanking;
class SecurityStatus;
class Notification;
class WidgetData;
class MockData;

class DashboardStat : public object, public std::enable_shared_from_this<DashboardStat> {
public:
    using std::enable_shared_from_this<DashboardStat>::shared_from_this;
    string label;

    string value;

    string description;

    any intent;

    string icon;

    string tag;

    any direction;
};

class ChartDataPoint : public object, public std::enable_shared_from_this<ChartDataPoint> {
public:
    using std::enable_shared_from_this<ChartDataPoint>::shared_from_this;
    string date;

    double spendings;

    double sales;

    double coffee;
};

class ChartData : public object, public std::enable_shared_from_this<ChartData> {
public:
    using std::enable_shared_from_this<ChartData>::shared_from_this;
    array<std::shared_ptr<ChartDataPoint>> week;

    array<std::shared_ptr<ChartDataPoint>> month;

    array<std::shared_ptr<ChartDataPoint>> year;
};

class RebelRanking : public object, public std::enable_shared_from_this<RebelRanking> {
public:
    using std::enable_shared_from_this<RebelRanking>::shared_from_this;
    double id;

    string name;

    string handle;

    string streak;

    double points;

    string avatar;

    boolean featured;

    string subtitle;
};

class SecurityStatus : public object, public std::enable_shared_from_this<SecurityStatus> {
public:
    using std::enable_shared_from_this<SecurityStatus>::shared_from_this;
    string title;

    string value;

    string status;

    any variant;
};

class Notification : public object, public std::enable_shared_from_this<Notification> {
public:
    using std::enable_shared_from_this<Notification>::shared_from_this;
    string id;

    string title;

    string message;

    string timestamp;

    any type;

    boolean read;

    any priority;
};

class WidgetData : public object, public std::enable_shared_from_this<WidgetData> {
public:
    using std::enable_shared_from_this<WidgetData>::shared_from_this;
    string location;

    string timezone;

    string temperature;

    string weather;

    string date;
};

class MockData : public object, public std::enable_shared_from_this<MockData> {
public:
    using std::enable_shared_from_this<MockData>::shared_from_this;
    array<std::shared_ptr<DashboardStat>> dashboardStats;

    std::shared_ptr<ChartData> chartData;

    array<std::shared_ptr<RebelRanking>> rebelsRanking;

    array<std::shared_ptr<SecurityStatus>> securityStatus;

    array<std::shared_ptr<Notification>> notifications;

    std::shared_ptr<WidgetData> widgetData;
};

#endif
