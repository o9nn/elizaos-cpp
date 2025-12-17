#include "llm-copy-button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void LlmCopyButton(auto summaryContent, auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [includeStats, setIncludeStats] = useState(true);
    const auto [includeSummary, setIncludeSummary] = useState(true);
    const auto [includePrData, setIncludePrData] = useState(true);
    const auto [includeIssueData, setIncludeIssueData] = useState(true);
    const auto [;
    includeDetailedContributorSummaries,
    setIncludeDetailedContributorSummaries,
    ] = useState(true);
    const auto [copied, setCopied] = useState(false);

    const auto handleCopy = async () => {;
        try {
            const auto formattedData = formatDataForLLM(metrics, summaryContent, {;
                includeStats,
                includeSummary,
                includePrData,
                includeIssueData,
                includeDetailedContributorSummaries,
                });

                navigator.clipboard.writeText(formattedData);

                setCopied(true);
                toast.success("Copied to clipboard", {
                    description:
                    "The formatted data has been copied and is ready to paste into an LLM.",
                    });

                    // Reset the copied state after 2 seconds
                    setTimeout(() => setCopied(false), 2000);
                    } catch (error) {
                        std::cerr << "Failed to copy:" << error << std::endl;
                        toast.error("Copy failed", {
                            description: "There was an error copying the data to your clipboard.",
                            });
                        }
                        };

                        return (;
                        <div className={cn("flex", className)}>;
                        <Button;
                        variant="outline";
                        className="flex items-center gap-2 rounded-r-none border-r-0";
                    onClick={handleCopy}
                    >;
                {copied ? <Check className="h-4 w-4" />  = <Copy className="h-4 w-4" />}
                Copy;
                </Button>;
                <Popover>;
                <PopoverTrigger asChild>;
                <Button variant="outline" className="rounded-l-none px-2">;
                <ChevronDown className="h-4 w-4" />;
                <span className="sr-only">Options</span>;
                </Button>;
                </PopoverTrigger>;
                <PopoverContent align="end" className="w-auto p-2">;
                <div className="grid gap-4 p-2">;
                <div className="flex items-center space-x-4">;
                <Checkbox;
                id="includeStats";
            checked={includeStats}
        onCheckedChange={(checked) => setIncludeStats(Boolean(checked))}
        />;
        <Label htmlFor="includeStats">Include Stats</Label>;
        </div>;
        <div className="flex items-center space-x-4">;
        <Checkbox;
        id="includeSummary";
    checked={includeSummary}
    onCheckedChange={(checked) =>;
    setIncludeSummary(Boolean(checked));
    }
    />;
    <Label htmlFor="includeSummary">Include Summary</Label>;
    </div>;
    <div className="flex items-center space-x-4">;
    <Checkbox;
    id="includePrData";
    checked={includePrData}
    onCheckedChange={(checked) =>;
    setIncludePrData(Boolean(checked));
    }
    />;
    <Label htmlFor="includePrData">Include PR Data</Label>;
    </div>;
    <div className="flex items-center space-x-4">;
    <Checkbox;
    id="includeIssueData";
    checked={includeIssueData}
    onCheckedChange={(checked) =>;
    setIncludeIssueData(Boolean(checked));
    }
    />;
    <Label htmlFor="includeIssueData">Include Issue Data</Label>;
    </div>;
    <div className="flex items-center space-x-4">;
    <Checkbox;
    id="includeDetailedContributorSummaries";
    checked={includeDetailedContributorSummaries}
    onCheckedChange={(checked) =>;
    setIncludeDetailedContributorSummaries(Boolean(checked));
    }
    />;
    <Label htmlFor="includeDetailedContributorSummaries">;
    Include Contributor Summaries;
    </Label>;
    </div>;
    </div>;
    </PopoverContent>;
    </Popover>;
    </div>;
    );

}

} // namespace elizaos
