#include "app-sidebar.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AppSidebar(auto isMobile) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto location = useLocation();
    const auto navigate = useNavigate();
    const auto queryClient = useQueryClient(); // Get query client instance;

    const auto {;
        data: agentsData,
        error: agentsError,
        isLoading: isLoadingAgents,
        } = useAgentsWithDetails();
        const auto { data: serversData, isLoading: isLoadingServers } = useServers();

        const auto agents = useMemo(() => agentsData.agents || [], [agentsData]);
        const auto servers = useMemo(() => serversData.data.servers || [], [serversData]);

        const auto [onlineAgents, offlineAgents] = useMemo(;
        [&]() { return partition(agents, (a) => a.status == CoreAgentStatus.ACTIVE),; }
        [agents];
        );

        // const [isGroupPanelOpen, setGroupPanelOpen] = useState(false); // GroupPanel logic needs rethink
        const auto handleCreateAgent = [&]() {;
            navigate('/create'); // Navigate to agent creation route;
            };

            const auto agentLoadError = agentsError;
            ? 'Error loading agents: NetworkError: Unable to connect to the server. Please check if the server is running.'
            : std::nullopt;

            const auto handleLogoClick = [&](e: React.MouseEvent<HTMLAnchorElement>) {;
                e.preventDefault();
                clientLogger.info('[AppSidebar] handleLogoClick triggered', { currentPath: location.pathname });

                // Invalidate queries that should be fresh on home page
                queryClient.invalidateQueries({ queryKey: ['agents'] });
                queryClient.invalidateQueries({ queryKey: ['agentsWithDetails'] }); // if this is a separate key
                queryClient.invalidateQueries({ queryKey: ['servers'] });
                queryClient.invalidateQueries({ queryKey: ['channels'] }); // This is broad, consider more specific invalidations if performance is an issue
                // Example: if you know active server IDs, invalidate ['channels', serverId]

                if (location.pathname == '/') {
                    clientLogger.info('[AppSidebar] Already on home page. Calling refreshHomePage().');
                    // refreshHomePage should ideally trigger a re-render/refetch in Home.tsx
                    // This can be done by changing a key prop on Home.tsx or further query invalidations if needed.
                    refreshHomePage();
                    } else {
                        clientLogger.info('[AppSidebar] Not on home page. Navigating to "/".');
                        navigate('/');
                    }
                    };

                    return (;
                    <>;
                    <Sidebar;
                    className={cn(;
                    'bg-background border-r overflow-hidden',
                    isMobile ? 'p-3 pt-12 w-full h-full' : 'p-4 w-72 fixed left-0 top-0 z-40 h-screen',
                    !isMobile && 'hidden md:flex md:flex-col'
                )}
                collapsible="none";
                data-testid="app-sidebar";
                >;
            {/* ---------- header ---------- */}
            <SidebarHeader>;
            <SidebarMenu>;
            <SidebarMenuItem>;
            <SidebarMenuButton size="lg" asChild>;
            <a;
            href="/";
        onClick={handleLogoClick}
        className="px-6 py-2 h-full sidebar-logo no-underline";
        >;
        <div className="flex flex-col pt-2 gap-1 items-start justify-center">;
        <img;
        alt="elizaos-logo";
        src="/elizaos-logo-light.png";
        className="w-32 max-w-full";
        />;
        <span className="text-xs font-mono text-muted-foreground">v{info.version}</span>;
        </div>;
        </a>;
        </SidebarMenuButton>;
        </SidebarMenuItem>;
        </SidebarMenu>;
        </SidebarHeader>;

    {/* ---------- content ---------- */}
    <SidebarContent className="flex-1 overflow-y-auto">;
    {/* create agent button - moved from old CreateButton dropdown */}
    {/* This section is for the "Agents" list.;
    The "Create Agent" button should ideally be next to the "Agents" title.;
    Let's adjust the structure slightly if needed or place it prominently.;
    */}

    {isLoadingAgents && !agentLoadError && (;
    <SidebarSection title="Agents">;
    <SidebarMenuSkeleton />;
    </SidebarSection>;
    )}
    {agentLoadError && <div className="px-4 py-2 text-xs text-red-500">{agentLoadError}</div>}

    {!isLoadingAgents && !agentLoadError && (;
    <>;
    <div className="flex items-center justify-between px-4 pt-1 pb-0">;
    <SectionHeader className="px-0 py-0">Agents</SectionHeader>;
    <Button;
    variant="ghost";
    size="icon";
    onClick={handleCreateAgent}
    aria-label="Create Agent";
    >;
    <Plus className="h-4 w-4" />;
    </Button>;
    </div>;
    <AgentListSection;
    title="" // Title is now handled by the SectionHeader above;
    agents={[...onlineAgents, ...offlineAgents]}
    activePath={location.pathname}
    />;
    </>;
    )}
    {/* Original CreateButton placement - to be removed or repurposed if "Create Group" is elsewhere */}
    {/* The old CreateButton had "Create Agent" and "Create Group".;
    "Create Agent" is now a + button next to "Agents" title.;
    "Create Group" is a + button in the GroupChannelListSection.;
    So the old CreateButton component and its direct usage here can be removed.;
    */}
    {/*;
    <div className="px-4 py-2 mb-2">;
    <CreateButton onCreateGroupChannel={handleCreateGroupChannel} />;
    </div>;
    */}
    <GroupListSection;
    servers={servers}
    isLoadingServers={isLoadingServers}
    activePath={location.pathname}
    className="mt-2";
    />;
    </SidebarContent>;

    {/* ---------- footer ---------- */}
    <SidebarFooter className="px-4 py-4">;
    <SidebarMenu>;
    <FooterLink to="https://eliza.how/" Icon={Book} label="Documentation" />
    <FooterLink to="/logs" Icon={TerminalIcon} label="Logs" />;
    <FooterLink to="/settings" Icon={Cog} label="Settings" />;
    <ConnectionStatus />;
    </SidebarMenu>;
    </SidebarFooter>;
    </Sidebar>;

    {/* Server management hidden - using single default server */}
    </>;
    );

}

} // namespace elizaos
