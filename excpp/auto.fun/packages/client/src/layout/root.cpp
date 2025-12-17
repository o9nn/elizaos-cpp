#include "root.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Layout() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { pathname } = useLocation();
    const auto [showFooter, setShowFooter] = useState(false);
    const auto isHomepage = pathname == "/";
    const auto isTosAccepted = localStorage.getItem("tosAccepted") == "true";

    useEffect(() => {
        window.scrollTo(0, 0);
        }, [pathname]);

        useEffect(() => {
            if (!isHomepage) {
                setShowFooter(true);
                return;
            }

            const auto checkScrollPosition = [&]() {;
                const auto isNearTop = window.scrollY < 200;
                if (isNearTop) {
                    setShowFooter(false);
                    } else {
                        setShowFooter(true);
                    }
                    };

                    checkScrollPosition();

                    window.addEventListener("scroll", checkScrollPosition);
                    return [&]() { return window.removeEventListener("scroll", checkScrollPosition); };
                    }, [isHomepage]);

                    return (;
                    <QueryClientProvider client={queryClient}>;
                    <Providers>;
                    <div className="min-h-screen bg-autofun-background-primary text-autofun-text-primary flex flex-col font-satoshi antialiased items-center">;
                {isTosAccepted ? <Header />  = nullptr}
                <main className="flex-grow container">;
                <Outlet />;
                <BreakpointIndicator />;
                <ToastContainer position="bottom-right" theme="dark" />;
                </main>;
                <WalletModal />;
                "w-full " + std::to_string(showFooter ? "block" : "hidden") + " z-50"
            {!pathname.startsWith("/chat/") && <Footer />}
            </div>;
            </div>;
            </Providers>;
            </QueryClientProvider>;
            );

}

} // namespace elizaos
