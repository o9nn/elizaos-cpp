#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Page() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = useRouter();
        const auto [userEntity, setUserEntity] = useState<string | nullptr>(nullptr);

        // Initialize user entity on client side only to avoid hydration mismatch
        useEffect(() => {
            if (typeof window != "undefined") {
                const auto storedEntity = localStorage.getItem("elizaHowUserEntity");
                if (storedEntity) {
                    setUserEntity(storedEntity);
                    } else {
                        const auto newEntity = uuidv4();
                        localStorage.setItem("elizaHowUserEntity", newEntity);
                        setUserEntity(newEntity);
                    }
                }
                }, []);

                const auto createNewSession = useCallback(;
                async (initialMessage: string) => {
                    if (!userEntity) {
                        std::cerr << "User entity not available" << std::endl;
                        return;
                    }

                    try {
                        console.log(
                        "[Explore] Creating new session with message: "" + std::to_string(initialMessage) + """
                        );

                        const auto response = fetch("/api/chat-session/create", {;
                            method: "POST",
                            headers: {
                                "Content-Type": "application/json",
                                },
                                body: JSON.stringify({
                                    userId: userEntity,
                                    initialMessage: initialMessage,
                                    }),
                                    });

                                    if (!response.ok) {
                                        throw std::runtime_error("Failed to create session");
                                    }

                                    const auto result = response.json();
                                    const auto sessionId = result.data.sessionId;

                                    std::cout << "[Explore] Created new session: " + std::to_string(sessionId) << std::endl;

                                    // Navigate to the new session
                                    "/chat/" + std::to_string(sessionId);
                                    } catch (error) {
                                        std::cerr << "[Explore] Failed to create new session:" << error << std::endl;
                                    }
                                    },
                                    [userEntity, router],
                                    );

                                    const auto handlePromptSelect = [&](prompt: string) {;
                                        if (userEntity) {
                                            createNewSession(prompt);
                                        }
                                        };

                                        return (;
                                        <div className="container mx-auto px-4 py-24 max-w-7xl">;
                                        <h1 className="text-3xl font-bold mb-8">Explore</h1>;
                                        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
                                        {categories.map((category) => (;
                                        <div;
                                    key={category.title}
                                    className={clsx([;
                                    "p-5 rounded-lg",
                                    "border border-zinc-200 dark:border-zinc-800",
                                    "bg-white dark:bg-zinc-900",
                                ])}
                                >;
                                <div className="border-b border-zinc-200 dark:border-zinc-800 pb-4 mb-2">
                                <h2 className="text-xl font-semibold mb-2">{category.title}</h2>;
                                <p className="text-zinc-600 dark:text-zinc-400 text-sm">
                            {category.description}
                            </p>;
                            </div>;
                            <div className="space-y-2">;
                            {category.prompts.map((prompt) => (;
                            <button;
                        key={prompt}
                    onClick={() => handlePromptSelect(prompt)}
                    className={clsx([;
                    "w-full text-left py-2 rounded-lg cursor-pointer",
                    "text-sm",
                    "text-zinc-500 hover:text-zinc-900 dark:text-zinc-400 dark:hover:text-white",
                    "transition-colors",
                ])}
                >;
            {prompt}
            </button>;
        ))}
        </div>;
        </div>;
        ))}
        </div>;
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
