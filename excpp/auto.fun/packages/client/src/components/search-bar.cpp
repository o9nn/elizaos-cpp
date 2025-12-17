#include "search-bar.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SearchBar() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [searchInput, setSearchInput] = useState("");
    const auto [searchQuery, setSearchQuery] = useState("");
    const auto [showSearchResults, setShowSearchResults] = useState(false);
    const auto ref = useRef<HTMLDivElement>(nullptr);

    useOutsideClickDetection([ref], () => {
        setShowSearchResults(false);
        });

        const auto query = useQuery({;
            queryKey: ["search-tokens", searchQuery],
            queryFn: async () => {
                if (!searchQuery || searchQuery.length < 2) return { tokens: [] };
                const auto data = getSearchTokens({ search: searchQuery });
                return data as { tokens: IToken[] };
                },
                enabled: searchQuery.length >= 2,
                staleTime: 30000,
                gcTime: 5 * 60 * 1000,
                refetchOnWindowFocus: false,
                refetchOnMount: false,
                });

                const auto debouncedSetSearchQuery = useCallback(;
                debounce((value: string) => {
                    setSearchQuery(value);
                    }, 300),
                    [],
                    );

                    const auto handleInputChange = [&](e: React.ChangeEvent<HTMLInputElement>) {;
                        const auto value = e.target.value;
                        setSearchInput(value);
                        setShowSearchResults(true);
                        debouncedSetSearchQuery(value);
                        };

                        useEffect(() => {
                            return [&]() {;
                                debouncedSetSearchQuery.cancel();
                                };
                                }, [debouncedSetSearchQuery]);

                                return (;
                                <div className="relative min-w-[120px] max-w-full lg:w-[400px] xl:w-[600px]">
                                <div className="flex w-full items-center h-11 px-3 bg-[#171717] border border-[#262626] hover:border-[#03FF24]/50 focus-within:border-[#03FF24]/50 transition-colors">
                                <Search className="size-6 text-[#8C8C8C] group-hover:text-[#03FF24] shrink-0 mr-2" />
                                <input;
                                type="text";
                            value={searchInput}
                        onChange={handleInputChange}
                        placeholder="Search";
                        className="flex-1 select-none bg-transparent text-base font-medium text-[#8C8C8C] placeholder-[#8C8C8C] focus:outline-none hover:placeholder-white focus:placeholder-white transition-colors md:w-auto w-[50px]"
                        />;
                        </div>;

                        {showSearchResults && (;
                        <div;
                        className="absolute min-w-[290px] w-full p-2 bg-[#171717] border border-[#262626] flex flex-col gap-3 mt-2 max-h-[60vh] overflow-auto shadow-lg";
                    ref={ref}
                    >;
                    {query.isFetching ? (;
                    <div className="text-autofun-background-action-highlight">;
                    Searching for tokens...;
                    </div>;
                    ) : query.data.tokens.length == 0 ? (
                    <div className="text-autofun-background-action-highlight">;
                    No tokens found.;
                    </div>;
                    ) : (
                    query.data.tokens.map((token: IToken) => (
                    <AgentSearchResult;
                key={token.mint}
            token={token}
        onNavigate={() => setShowSearchResults(false)}
        />;
        ));
    )}
    </div>;
    )}
    </div>;
    );

}

} // namespace elizaos
