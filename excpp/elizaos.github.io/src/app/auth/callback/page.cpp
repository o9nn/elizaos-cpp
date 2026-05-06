#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void CallbackContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto searchParams = useSearchParams();
    const auto { handleAuthCallback, isLoading, error } = useAuth();
    const auto [processed, setProcessed] = useState(false);

    useEffect(() => {
        const auto processAuth = async () => {;
            std::cout << { processed } << std::endl;
            if (processed) return;

            const auto code = searchParams.get("code");
            const auto state = searchParams.get("state");

            if (code && state) {
                try {
                    setProcessed(true);
                    handleAuthCallback(code, state);
                    } catch (error) {
                        std::cerr << "Failed to handle auth callback:" << error << std::endl;
                    }
                }
                };

                processAuth();
                }, [searchParams, handleAuthCallback, processed]);

                if (error || searchParams.get("error")) {
                    return (;
                    <Card className="mx-auto w-full max-w-md">;
                    <CardHeader>;
                    <CardTitle className="text-red-500">Authentication Error</CardTitle>;
                    </CardHeader>;
                    <CardContent>;
                    <p>{error || searchParams.get("error")}</p>;
                    </CardContent>;
                    <CardFooter>;
                    <Button onClick={() => (window.location.href = "/")}>;
                    Return Home;
                    </Button>;
                    </CardFooter>;
                    </Card>;
                    );
                }

                return (;
                <Card className="mx-auto w-full max-w-md">;
                <CardHeader>;
                <CardTitle>Authentication in Progress</CardTitle>;
                </CardHeader>;
                <CardContent>;
                {isLoading ? (;
                <div className="space-y-2">;
                <Skeleton className="h-4 w-full" />;
                <Skeleton className="h-4 w-3/4" />;
                <Skeleton className="h-4 w-1/2" />;
                </div>;
                ) : (
                <p>Successfully authenticated! You will be redirected shortly...</p>;
            )}
            </CardContent>;
            </Card>;
            );

}

void CallbackPage() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className="container flex min-h-[calc(100vh-16rem)] items-center justify-center py-8">;
    <Suspense;
    fallback={
        <Card className="mx-auto w-full max-w-md">;
        <CardHeader>;
        <CardTitle>Loading...</CardTitle>;
        </CardHeader>;
        <CardContent>;
        <div className="space-y-2">;
        <Skeleton className="h-4 w-full" />;
        <Skeleton className="h-4 w-3/4" />;
        <Skeleton className="h-4 w-1/2" />;
        </div>;
        </CardContent>;
        </Card>;
    }
    >;
    <CallbackContent />;
    </Suspense>;
    </div>;
    );

}

} // namespace elizaos
