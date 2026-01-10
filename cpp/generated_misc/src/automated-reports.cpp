#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizas-list/src/lib/reporting/automated-reports.h"

ReportGenerator::ReportGenerator() {
    this->chartGenerator = std::make_shared<ChartJSNodeCanvas>(object{
        object::pair{std::string("width"), 800}, 
        object::pair{std::string("height"), 400}, 
        object::pair{std::string("backgroundColour"), std::string("white")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<Buffer>>> ReportGenerator::generateReport(std::shared_ptr<ReportData> data, std::shared_ptr<ReportConfig> config)
{
    try
    {
        auto doc = std::make_shared<jsPDF>();
        std::async([=]() { Promise->all(std::tuple<any, any, std::shared_ptr<Promise<void>>, any>{ this->addExecutiveSummary(doc, data->summary), this->addStatisticalAnalysis(doc, data->statistics), this->addVisualization(doc, data->visualizations), this->addRecommendations(doc, data->recommendations) }); });
        this->applyReportStyling(doc, config);
        return doc->output();
    }
    catch (const any& error)
    {
        logger["error"](std::string("Error generating report:"), error);
        throw any(error);
    }
}

void ReportGenerator::addVisualization(std::shared_ptr<jsPDF> doc, array<std::shared_ptr<VisualizationData>> visualizations)
{
    for (auto& viz : visualizations)
    {
        auto chart = std::async([=]() { this->chartGenerator->renderToBuffer(viz->config); });
        doc->addImage(chart, std::string("PNG"), 10, doc->lastAutoTable->finalY + 10, 190, 100);
        doc->addPage();
    }
}

