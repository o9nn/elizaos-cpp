#include ".monitoring/logger.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

class ReportGenerator {
  private chartGenerator: ChartJSNodeCanvas;

  constructor() {
    this.chartGenerator = new ChartJSNodeCanvas({
      width: 800,
      height: 400,
      backgroundColour: 'white'
    });
  }

  async generateReport(
    data: ReportData,
    config: ReportConfig
  ): Promise<Buffer> {
    try {
      const doc = new jsPDF();
      
      // Generate report sections
      await Promise.all([
        this.addExecutiveSummary(doc, data.summary),
        this.addStatisticalAnalysis(doc, data.statistics),
        this.addVisualization(doc, data.visualizations),
        this.addRecommendations(doc, data.recommendations)
      ]);

      // Add metadata and styling
      this.applyReportStyling(doc, config);
      
      return doc.output();
    } catch (error) {
      logger.error('Error generating report:', error);
      throw error;
    }
  }

  private async addVisualization(
    doc: jsPDF,
    visualizations: VisualizationData[]
  ) {
    for (const viz of visualizations) {
      const chart = await this.chartGenerator.renderToBuffer(viz.config);
      doc.addImage(
        chart,
        'PNG',
        10,
        doc.lastAutoTable.finalY + 10,
        190,
        100
      );
      doc.addPage();
    }
  }
} 
} // namespace elizaos
