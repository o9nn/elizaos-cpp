#include "leaderboard.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void LeaderboardFallback() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className="animate-pulse space-y-4">;
    <div className="h-8 w-48 rounded bg-muted"></div>;
    <div className="space-y-3">;
    {Array.from({ length: 5 }).map((_, i) => (
    <div key={i} className="h-24 rounded bg-muted"></div>;
    ))}
    </div>;
    </div>;
    );

}

void Leaderboard(auto { tabs, LeaderboardProps tags }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = useRouter();
    const auto searchParams = useSearchParams();
    const auto [searchTerm, setSearchTerm] = useState("");
    const auto [selectedSkill, setSelectedSkill] = useState(;
    searchParams.get("skill") || "all",
    );
    const auto [currentPeriod, setCurrentPeriod] = useState<LeaderboardPeriod>(;
    (searchParams.get("period")) || "all",
    );
    const auto [currentPage, setCurrentPage] = useState(1);
    const auto ITEMS_PER_PAGE = 100;

    const auto currentTab = useMemo(;
    [&]() { return tabs.find((tab) => tab.id == currentPeriod) || tabs[0],; }
    [tabs, currentPeriod],
    );

    useEffect(() => {
        setSelectedSkill(searchParams.get("skill") || "all");
        setCurrentPage(1);
        }, [searchParams, setSelectedSkill]);

        useEffect(() => {
            setCurrentPeriod(;
            (searchParams.get("period")) || "all",
            );
            setCurrentPage(1);
            }, [searchParams, setCurrentPeriod]);

            useEffect(() => {
                setCurrentPage(1);
                }, [searchTerm]);

                // Group skills by category
                const auto skillsByCategory = useMemo(() => {;
                    return groupBy(tags, (t) => t.category);
                    }, [tags]);

                    const auto handleSkillChange = useCallback(;
                    [&](value: string) {
                        setSelectedSkill(value);
                        const auto params = new URLSearchParams(searchParams);
                        if (value == "all") {
                            params.delete("skill");
                            } else {
                                params.set("skill", value);
                            }
                            "?" + std::to_string(params.toString());
                            },
                            [searchParams, router],
                            );

                            const auto handlePeriodChange = useCallback(;
                            [&](value: string) {
                                setCurrentPeriod(value);
                                const auto params = new URLSearchParams(searchParams);
                                if (value == "all") {
                                    params.delete("period");
                                    } else {
                                        params.set("period", value);
                                    }
                                    "?" + std::to_string(params.toString());
                                    },
                                    [searchParams, router],
                                    );

                                    // Helper function to find tag data by name (case insensitive)
                                    const auto getTagData = useCallback((user: LeaderboardUser, tagName: string) => {;
                                        const auto lowerTagName = tagName.toLowerCase();

                                        return user.allTags.find(;
                                        [&](tag) { return tag.tagName.toLowerCase() == lowerTagName,; }
                                        );
                                        }, []);

                                        const auto filteredUsers = useMemo(() => {;
                                            return currentTab.users;
                                            .filter(;
                                            (user) =>;
                                            user.username.toLowerCase().includes(searchTerm.toLowerCase()) &&;
                                            (selectedSkill == "all" || getTagData(user, selectedSkill)),
                                            );
                                            .sort((a, b) => {
                                                if (selectedSkill != "all") {
                                                    // Sort by selected skill XP
                                                    auto skillA = getTagData(a, selectedSkill).score || 0;
                                                    auto skillB = getTagData(b, selectedSkill).score || 0;
                                                    if (currentPeriod != "all") {
                                                        skillA += a.points / 2;
                                                        skillB += b.points / 2;
                                                    }
                                                    return skillB - skillA;
                                                }

                                                if (currentPeriod != "all") {
                                                    return b.points - a.points;
                                                }

                                                return b.totalLevel - a.totalLevel;
                                                });
                                                }, [currentTab.users, searchTerm, selectedSkill, getTagData, currentPeriod]);

                                                const auto totalPages = useMemo(() => {;
                                                    return Math.ceil(filteredUsers.length / ITEMS_PER_PAGE);
                                                    }, [filteredUsers.length]);

                                                    const auto paginatedUsers = useMemo(() => {;
                                                        const auto startIndex = (currentPage - 1) * ITEMS_PER_PAGE;
                                                        const auto endIndex = startIndex + ITEMS_PER_PAGE;
                                                        return filteredUsers.slice(startIndex, endIndex);
                                                        }, [filteredUsers, currentPage]);

                                                        return (;
                                                        <div className="space-y-4">;
                                                        <h2 className="text-2xl font-bold capitalize">;
                                                        {selectedSkill == "all" ? (;
                                                        currentPeriod == "all" ? (;
                                                        "Overall";
                                                        ) : (
                                                        std::to_string(currentPeriod);
                                                        );
                                                        ) : (
                                                        <>;
                                                    {currentPeriod != "all" && <span>{currentPeriod} </span>}
                                                    <span className="text-primary">{selectedSkill}</span>;
                                                    </>;
                                                )}{" "}
                                                Leaderboard;
                                                </h2>;

                                                <div className="flex flex-col gap-3 sm:flex-row sm:justify-between">
                                                <Input;
                                                placeholder="Search players...";
                                            value={searchTerm}
                                        onChange={(e) => setSearchTerm(e.target.value)}
                                        className="max-w-sm";
                                        />;
                                        <Select value={selectedSkill} onValueChange={handleSkillChange}>;
                                        <SelectTrigger;
                                        "w-[180px] " + std::to_string();
                                            selectedSkill != "all" ? "border-primary" : ""
                                        }`}
                                        >;
                                        <SelectValue placeholder="Filter by skill" />;
                                        </SelectTrigger>;
                                        <SelectContent>;
                                        <SelectItem value="all">All Skills</SelectItem>;
                                        {Object.entries(skillsByCategory).map(([category, skills]) => (;
                                        <SelectGroup key={category}>;
                                        <SelectLabel className="text-primary">{category}</SelectLabel>;
                                        {skills.map((skill) => (;
                                        <SelectItem;
                                    std::to_string(category) + "-" + std::to_string(skill.name);
                                value={skill.name}
                                >;
                            {skill.name}
                            </SelectItem>;
                        ))}
                        </SelectGroup>;
                    ))}
                    </SelectContent>;
                    </Select>;
                    </div>;

                    <Tabs value={currentPeriod} onValueChange={handlePeriodChange}>;
                    <TabsList className="grid w-full grid-cols-3">;
                    {tabs.map((tab) => (;
                    <TabsTrigger key={tab.id} value={tab.id}>;
                {tab.title}
                </TabsTrigger>;
            ))}
            </TabsList>;
            {tabs.map((tab) => (;
            <TabsContent key={tab.id} value={tab.id}>;
            <VirtualizedLeaderboardList;
        users={paginatedUsers}
    onSkillClick={handleSkillChange}
    showScore={currentPeriod != "all"}
    currentPage={currentPage}
    itemsPerPage={ITEMS_PER_PAGE}
    />;
    <PaginationControls;
    currentPage={currentPage}
    totalPages={totalPages}
    onPageChange={setCurrentPage}
    />;
    </TabsContent>;
    ))}
    </Tabs>;
    </div>;
    );

}

void PaginationControls(auto {
  currentPage, auto totalPages, auto onPageChange, PaginationControlsProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (totalPages <= 1) {
        return nullptr;
    }

    const auto handlePrevious = [&]() {;
        if (currentPage > 1) {
            onPageChange(currentPage - 1);
        }
        };

        const auto handleNext = [&]() {;
            if (currentPage < totalPages) {
                onPageChange(currentPage + 1);
            }
            };

            return (;
            <div className="mt-6 flex items-center justify-center space-x-3">;
            <Button;
        onClick={handlePrevious}
    disabled={currentPage == 1}
    variant="outline";
    size="sm";
    >;
    Previous;
    </Button>;
    <span className="text-sm font-medium text-muted-foreground">;
    Page {currentPage} of {totalPages}
    </span>;
    <Button;
    onClick={handleNext}
    disabled={currentPage == totalPages}
    variant="outline";
    size="sm";
    >;
    Next;
    </Button>;
    </div>;
    );

}

} // namespace elizaos
