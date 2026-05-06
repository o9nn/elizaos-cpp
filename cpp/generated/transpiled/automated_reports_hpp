#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_REPORTING_AUTOMATED-REPORTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_LIB_REPORTING_AUTOMATED-REPORTS_H
#include "core.h"
#include "jspdf.h"
#include "chartjs-node-canvas.h"
#include "../monitoring/logger.h"

class ReportGenerator;

class ReportGenerator : public object, public std::enable_shared_from_this<ReportGenerator> {
public:
    using std::enable_shared_from_this<ReportGenerator>::shared_from_this;
    std::shared_ptr<ChartJSNodeCanvas> chartGenerator;

    ReportGenerator();
    virtual std::shared_ptr<Promise<std::shared_ptr<Buffer>>> generateReport(std::shared_ptr<ReportData> data, std::shared_ptr<ReportConfig> config);
    virtual void addVisualization(std::shared_ptr<jsPDF> doc, array<std::shared_ptr<VisualizationData>> visualizations);
};

#endif
