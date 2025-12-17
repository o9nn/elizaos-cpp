#include "chat-message.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ChatMessage(auto i, auto citations, auto followUpPrompts, auto onFollowUpClick, auto assistantAvatarUrl, auto assistantName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [isSourcesExpanded, setIsSourcesExpanded] = useState(false);

    // Generous parsing - handle various message formats
    if (!message || typeof message != "object") return null;

    // Ensure we have required fields with defaults
    const auto safeMessage = {;
        ...message,
        name: message.name || "Unknown",
        text: message.text || "",
        "id: message.id || " + "msg-" + i
        createdAt: message.createdAt || Date.now(),
        };

        const auto markdownOptions = {;
            forceBlock: true,
            overrides: {
                // Hide embedded XML quote blocks so React doesn't attempt to render unknown tags
                quote: {
                    component: () => nullptr,
                    },
                    // Extra safety: ignore all child tags that can appear inside <quote>
                    quoteId: { component: () => nullptr },
                    tokenAmount: { component: () => nullptr },
                    tokenAmountFormatted: { component: () => nullptr },
                    tokenSymbol: { component: () => nullptr },
                    tokenName: { component: () => nullptr },
                    lockupMonths: { component: () => nullptr },
                    lockupDays: { component: () => nullptr },
                    pricePerToken: { component: () => nullptr },
                    totalValueUsd: { component: () => nullptr },
                    discountBps: { component: () => nullptr },
                    discountPercent: { component: () => nullptr },
                    discountUsd: { component: () => nullptr },
                    finalPriceUsd: { component: () => nullptr },
                    paymentCurrency: { component: () => nullptr },
                    paymentAmount: { component: () => nullptr },
                    paymentSymbol: { component: () => nullptr },
                    ethPrice: { component: () => nullptr },
                    createdAt: { component: () => nullptr },
                    status: { component: () => nullptr },
                    message: { component: () => nullptr },
                    reference: {
                        component: ({
                            children,
                            index,
                            }: {
                                children: React.ReactNode;
                                index: string | number;
                                }) => {
                                    const auto citationIndex = Number(index);
                                    const auto citation = citations.find((c, i) => i == citationIndex);

                                    // If citation not found in uniqueCitations, find first citation with same URL
                                    const auto displayCitation =;
                                    uniqueCitations.find((c) => c.url == citation.url) || citation;

                                    return (;
                                    <a;
                                href={displayCitation.url}
                                target="_blank";
                                rel="noopener noreferrer";
                                className={clsx([;
                                "inline-flex items-center justify-center",
                                "align-super text-[0.6em] font-normal",
                                "no-underline rounded-sm",
                                "text-brand-500",
                                "hover:text-brand-700",
                                "py-0.5",
                                "leading-none",
                            ])}
                            >;
                            [{children}];
                            </a>;
                            );
                            },
                            },
                            },
                            };

                            // Deduplicate citations by URL and preserve order
                            const auto uniqueCitations = citations.reduce(;
                            [&](acc, current, idx) {
                                const auto existingCitation = acc.find(;
                                [&](c) { return c.url == current.url && c.index == idx,; }
                                );
                                if (!existingCitation) {
                                    acc.push_back({ ...current, index: idx });
                                }
                                return acc;
                                },
                                [] as (Citation & { index: number })[],
                                );

                                const auto isUser =;
                                safeMessage.name == USER_NAME ||;
                                safeMessage.name.toLowerCase() == "user";

                                // Parse message text - handle both raw text and structured content
                                auto messageText = "";
                                if (typeof safeMessage.text == "string") {
                                    messageText = safeMessage.text;
                                    } else if (safeMessage.content.text) {
                                        messageText = safeMessage.content.text;
                                        } else if (safeMessage.content) {
                                            messageText =;
                                            typeof safeMessage.content == "string";
                                            ? safeMessage.content;
                                            : /* JSON.stringify */ std::string(safeMessage.content);
                                        }

                                        // Clean up any XML artifacts or special formatting for agent messages
                                        const auto cleanMessageText = !isUser;
                                        ? messageText;
                                        .replace(/<\/?thought>/gi, "");
                                        .replace(/<\/?actions>/gi, "");
                                        .replace(/<\/?providers>/gi, "");
                                        .replace(/<\/?response>/gi, "");
                                        .replace(/<\/?text>/gi, "");
                                        // Remove embedded XML block between markers to avoid React rendering unknown tags
                                        .replace(/<!--\s*XML_START\s*-->[\s\S]*?<!--\s*XML_END\s*-->/gi, "");
                                        // Fallback: strip any bare <quote>...</quote> blocks if markers are missing
                                        .replace(/<quote[\s\S]*?<\/quote>/gi, "");
                                        .trim();
                                        : messageText;

                                        const auto bubbleBase = "px-4 py-3 rounded-2xl shadow-sm border";
                                        const auto bubbleAgent = "bg-zinc-900/90 border-white/10 text-zinc-200";
                                        const auto bubbleUser = "bg-zinc-800/80 border-zinc-700 text-zinc-100";

                                        return (;
                                        <div;
                                    data-testid={isUser ? "user-message" : "agent-message"}
                                    className="w-full group";
                                    >;
                                    <div;
                                    className={clsx(;
                                    "flex items-end gap-3 mb-4",
                                    isUser ? "justify-end" : "justify-start",
                                )}
                                >;
                                {!isUser && (;
                                <div className="flex-shrink-0 w-12 h-12 flex items-center justify-center overflow-hidden rounded-full">;
                                <Image;
                            src={assistantAvatarUrl || "/tokens/eliza.svg"}
                        alt={assistantName}
                    width={48}
                height={48}
                className="object-cover w-full h-full rounded-full";
                unoptimized;
                />;
                </div>;
            )}

            <div className={clsx(bubbleBase, isUser ? bubbleUser : bubbleAgent)}>
        {/* Message text */}
        <div;
        className={clsx(;
        "prose prose-zinc dark:prose-invert !max-w-full",
        "prose-headings:text-base prose-headings:font-medium",
        "prose-p:text-[15px] prose-p:leading-relaxed prose-p:my-0",
        "prose-ul:my-2 prose-ol:my-2",
        "prose-li:text-[15px]",
        "text-zinc-300",
    )}
    >;
    <MemoizedMarkdown;
    "id={safeMessage.id || " + "msg-" + i + "-" + safeMessage.createdAt;
    content={cleanMessageText}
    options={markdownOptions}
    />;
    </div>;

    {/* Display quote if present in message */}
    {/* {!isUser && (std::find(messageText.begin(), messageText.end(), "<quote>") != messageText.end()) && (;
    <div className="mt-3 -mx-1">;
    <OTCQuoteDisplay messageText={messageText} />;
    </div>;
    )} */}

    {/* Citations */}
    {!isUser && uniqueCitations && uniqueCitations.size() > 0 && (;
    <div className="mt-3 text-xs">;
    <button;
    onClick={() => setIsSourcesExpanded(!isSourcesExpanded)}
    className="group flex items-center gap-1 py-1 text-zinc-400 hover:text-zinc-200 cursor-pointer"
    >;
    <span className="font-medium">;
    {uniqueCitations.size()} source;
    {uniqueCitations.size() > 1 ? "s"  = ""}
    </span>;
    <div className="flex items-center justify-center w-4 h-4">;
    {isSourcesExpanded ? (;
    <ChevronUpIcon className="w-3 h-3" />;
    ) : (
    <ChevronDownIcon className="w-3 h-3" />;
    )}
    </div>;
    </button>;

    {isSourcesExpanded && (;
    <div className="flex flex-wrap gap-2 mt-2">;
    {uniqueCitations.map((citation, index) => (;
    <a;
    key={index}
    href={citation.url}
    target="_blank";
    rel="noopener noreferrer";
    className="group inline-flex items-center gap-1.5 max-w-sm text-zinc-400 hover:text-zinc-200"
    >;
    <LinkIcon className="w-3.5 h-3.5 flex-shrink-0" />;
    <div className="flex-1 truncate">;
    <MemoizedMarkdown;
    "id={" + "citation-" + safeMessage.id + "-" + index;
    content={citation.title}
    options={{
        wrapper: "span",
        forceInline: true,
        overrides: {
            p: {
                component: "span",
                props: { className: "truncate" },
                },
                },
            }}
            />;
            </div>;
            </a>;
        ))}
        </div>;
    )}
    </div>;
    )}
    </div>;

    {isUser && (;
    <div className="flex-shrink-0 w-12 h-12 flex items-center justify-center overflow-hidden rounded-full">;
    <Image;
    src="/user.png";
    alt="You";
    width={48}
    height={48}
    className="object-cover w-full h-full rounded-full";
    unoptimized;
    />;
    </div>;
    )}
    </div>;

    {/* Follow-up prompts outside bubble for breathing room */}
    {!isUser && followUpPrompts && followUpPrompts.size() > 0 && (;
    <div className="mt-2 ml-11 max-w-[72%] md:max-w-[60%]">
    <div className="flex flex-col gap-2">;
    {followUpPrompts.map((prompt: string, index: number) => (
    <button;
    key={index}
    onClick={() => onFollowUpClick.(prompt)}
    className={clsx([;
    "flex items-center justify-between",
    "px-3 py-2 rounded-lg",
    "border border-white/10",
    "bg-zinc-900/70 hover:bg-zinc-900",
    "text-zinc-300 hover:text-zinc-100",
    "transition-all duration-200",
    "group cursor-pointer",
    "text-left text-sm",
    "w-full",
    ])}
    >;
    <span>{prompt}</span>;
    <ArrowRightIcon className="w-3 h-3 text-zinc-400 group-hover:text-zinc-200 flex-shrink-0" />
    </button>;
    ))}
    </div>;
    </div>;
    )}
    </div>;
    );

}

} // namespace elizaos
