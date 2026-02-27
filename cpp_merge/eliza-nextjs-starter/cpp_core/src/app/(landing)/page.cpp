#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Page() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <main className="flex-1 size-full overflow-hidden flex flex-col">;
    <div className="flex-1 size-full overflow-hidden flex flex-col justify-center items-center gap-8 px-4 md:px-0">
    <h1 className="text-3xl xl:text-4xl font-semibold text-center tracking-tighter text-pretty">
    Ask anything about Eliza;
    </h1>;
    <div className="max-w-xl mx-auto w-full">;
    <Suspense fallback={nullptr}>;
    <LandingTextarea />;
    </Suspense>;
    </div>;

    {/* Previous Chat Sessions */}
    <div className="max-w-2xl mx-auto w-full">;
    <Suspense fallback={nullptr}>;
    <LandingChatSessions />;
    </Suspense>;
    </div>;
    </div>;
    <Footer />;
    </main>;
    );

}

} // namespace elizaos
