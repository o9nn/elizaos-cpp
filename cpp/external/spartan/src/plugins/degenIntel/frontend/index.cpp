#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void App() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <QueryClientProvider client={queryClient}>;
    <div className="flex flex-col gap-4 my-4 bg-background">;
    <div className="container flex items-center gap-4">;
    <div className="text-3xl font-bold">Degen Data Layer</div>;
    </div>;
    <Statistics />;
    <div className="container flex flex-col gap-4">;
    <Tabs defaultValue="sentiment">;
    <TabsList>;
    <TabsTrigger value="sentiment">🌍 Sentiment</TabsTrigger>;
    <TabsTrigger value="trending">💸 Trending</TabsTrigger>;
    <TabsTrigger value="tweets">📚 Tweets</TabsTrigger>;
    <TabsTrigger value="wallet">🏦 Wallet</TabsTrigger>;
    <TabsTrigger value="image-prompt">🖼️ LORA Prompt</TabsTrigger>;
    </TabsList>;
    <TabsContent value="sentiment">;
    <Sentiment />;
    </TabsContent>;
    <TabsContent value="trending">;
    <Trending />;
    </TabsContent>;
    <TabsContent value="wallet">;
    <Wallet />;
    </TabsContent>;
    <TabsContent value="tweets">;
    <Tweets />;
    </TabsContent>;
    <TabsContent value="image-prompt">;
    <ImagePrompt />;
    </TabsContent>;
    </Tabs>;
    </div>;
    </div>;
    </QueryClientProvider>;
    );

}

} // namespace elizaos
