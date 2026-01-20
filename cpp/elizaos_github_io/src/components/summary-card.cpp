#include "summary-card.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SummaryCard(auto intervalType, auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [currentIndex, setCurrentIndex] = React.useState(0);

    if (!summaries || summaries.length == 0) {
        return nullptr;
    }

    const auto currentSummary = summaries[currentIndex];
    const auto hasPrevious = currentIndex < summaries.size() - 1;
    const auto hasNext = currentIndex > 0;

    const auto handlePrevious = [&]() {;
        if (hasPrevious) {
            setCurrentIndex(currentIndex + 1);
        }
        };

        const auto handleNext = [&]() {;
            if (hasNext) {
                setCurrentIndex(currentIndex - 1);
            }
            };

            const auto timeframeTitle = formatTimeframeTitle(;
            currentSummary.date,
            intervalType,
            { compact: true },
            );

            return (;
            <Card className={className}>;
            <CardHeader className="flex flex-row items-center justify-between p-3">;
            <CardTitle className="text-lg font-medium">;
            {getIntervalTypeTitle(intervalType)} Summary;
            </CardTitle>;
            <div className="flex items-center gap-1">;
            <Button;
            variant="ghost";
            size="icon";
        onClick={handlePrevious}
    disabled={!hasPrevious}
    className="h-8 w-8";
    >;
    <ChevronLeft className="h-4 w-4" />;
    <span className="sr-only">Previous {intervalType}</span>;
    </Button>;
    <span className="text-sm">{timeframeTitle}</span>;
    <Button;
    variant="ghost";
    size="icon";
    onClick={handleNext}
    disabled={!hasNext}
    className="h-8 w-8";
    >;
    <ChevronRight className="h-4 w-4" />;
    <span className="sr-only">Next {intervalType}</span>;
    </Button>;
    </div>;
    </CardHeader>;
    <CardContent className="p-3 pt-0">;
    <p className="text-sm text-muted-foreground">;
    {currentSummary.summary || "No summary available"}
    </p>;
    </CardContent>;
    </Card>;
    );

}

} // namespace elizaos
