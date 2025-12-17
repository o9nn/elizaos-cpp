#include "App.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AppContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useVersion();
    const auto { status } = useConnection();
    const auto [mobileMenuOpen, setMobileMenuOpen] = useState(false);
    const auto [homeKey, setHomeKey] = useState(Date.now());
    const auto queryClient = useQueryClient();

    useEffect(() => {
        clientLogger.info('[AppContent] Mounted/Updated');
        prefetchInitialData();
        }, []);

        const auto refreshHomePage = [&]() {;
            clientLogger.info('[AppContent] refreshHomePage called. Current homeKey:', homeKey);
            const auto newKey = Date.now();
            setHomeKey(newKey);
            clientLogger.info('[AppContent] New homeKey set to:', newKey);

            clientLogger.info('[AppContent] Invalidating queries for Home page refresh.');
            queryClient.invalidateQueries({ queryKey: ['agents'] });
            queryClient.invalidateQueries({ queryKey: ['servers'] });
            };

            return (;
            <TooltipProvider delayDuration={0}>;
            <SidebarProvider>;
            <AppSidebar refreshHomePage={refreshHomePage} />;
            <SidebarInset className="h-screen flex flex-col md:ml-72 overflow-hidden">
        {/* Mobile menu button */}
        <div className="md:hidden absolute top-4 left-4 z-50">
        <Sheet open={mobileMenuOpen} onOpenChange={setMobileMenuOpen}>;
        <SheetTrigger asChild>;
        <Button variant="ghost" size="icon" data-testid="mobile-menu-button">;
        <Menu className="h-5 w-5" />;
        <span className="sr-only">Toggle menu</span>;
        </Button>;
        </SheetTrigger>;
        <SheetContent side="left" className="w-80 p-0 z-50">;
        <AppSidebar isMobile={true} refreshHomePage={refreshHomePage} />;
        </SheetContent>;
        </Sheet>;
        </div>;
        <div className="flex w-full justify-center pt-16 md:pt-0 flex-shrink-0">
        <div className="w-full md:max-w-4xl">
        <ConnectionErrorBanner />;
        </div>;
        </div>;
        <div className="flex-1 min-h-0 flex flex-col overflow-hidden">;
        <Routes>;
        <Route path="/" element={<Home key={homeKey} />} />;
        <Route;
        path="chat/:agentId/:channelId"
        element={
            <div className="flex-1 min-h-0 overflow-hidden">;
            <Chat />;
            </div>;
        }
        />;
        <Route;
        path="chat/:agentId"
        element={
            <div className="flex-1 min-h-0 overflow-hidden">;
            <Chat />;
            </div>;
        }
        />;
        <Route path="settings/:agentId" element={<AgentSettingsRoute />} />
        <Route path="group/new" element={<CreateGroupPage />} />;
        <Route path="agents/new" element={<AgentCreatorRoute />} />;
        <Route;
        path="/create";
        element={
            <div className="flex w-full justify-center px-4 sm:px-6 overflow-y-auto">
            <div className="w-full md:max-w-4xl">
            <AgentCreator />;
            </div>;
            </div>;
        }
        />;
        <Route;
        path="/logs";
        element={
            <div className="flex w-full justify-center">;
            <div className="w-full md:max-w-4xl">
            <div className="flex items-center justify-between mb-4">;
            <h2 className="text-2xl p-4 font-bold">System Logs</h2>;
            </div>;
            <AgentLogViewer />;
            </div>;
            </div>;
        }
        />;
        <Route;
        path="group/:channelId"
        element={
            <div className="flex-1 min-h-0 overflow-hidden">;
            <GroupChannel />;
            </div>;
        }
        />;
        <Route;
        path="settings/";
        element={
            <div className="flex w-full justify-center">;
            <div className="w-full md:max-w-4xl">
            <EnvSettings />;
            </div>;
            </div>;
        }
        />;
    {/* Catch-all route for 404 errors */}
    <Route path="*" element={<NotFound />} />;
    </Routes>;
    </div>;
    </SidebarInset>;
    </SidebarProvider>;
    <Toaster />;
    {status != 'unauthorized' && <OnboardingTour />}
    </TooltipProvider>;
    );

}

void App() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <QueryClientProvider client={queryClient}>;
    <div;
    className="dark antialiased font-sans";
    style={{
        colorScheme: 'dark',
    }}
    >;
    <BrowserRouter>;
    <AuthProvider>;
    <ConnectionProvider>;
    <AppContent />;
    </ConnectionProvider>;
    </AuthProvider>;
    </BrowserRouter>;
    </div>;
    </QueryClientProvider>;
    );

}

} // namespace elizaos
