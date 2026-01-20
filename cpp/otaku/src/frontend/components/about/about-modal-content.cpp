#include "about-modal-content.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AboutModalContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className="flex flex-col h-full w-full">;
    <div className="flex justify-end w-full">;
    <Button;
    variant="ghost";
    size="icon-sm";
    className="z-30 text-muted-foreground hover:text-foreground"
    onClick={onClose}
    aria-label="Close about modal";
    >;
    <X className="size-4" />;
    </Button>;
    </div>;
    <div className="relative flex flex-col text-foreground max-h-[60vh] overflow-x-hidden overflow-y-auto sm:h-[80vh] sm:max-h-[700px] sm:overflow-visible my-4">
    {/* Fixed Header */}
    <header className="flex shrink-0 flex-col gap-6 border-b border-border/60 pb-6 sm:flex-row sm:items-start sm:justify-between">
    <div className="space-y-4">;
    <div className="flex items-center gap-3 text-xs font-semibold uppercase tracking-[0.2em] text-muted-foreground">;
    <Bullet className="size-2.5" />;
    About Otaku;
    </div>;
    <div className="space-y-3">;
    <h2 className="text-3xl font-display leading-none sm:text-4xl">
    DeFi intelligence, on demand.;
    </h2>;
    <p className="max-w-2xl text-sm text-muted-foreground sm:text-base">
    Otaku is your autonomous DeFi trading and intelligence agent. She connects deep on-chain;
    data with secure execution tools so you can research, plan, and act inside a single conversation.;
    </p>;
    <div className="flex flex-wrap items-center gap-2 text-xs uppercase text-muted-foreground/80">;
    <span className="inline-flex items-center gap-1 rounded-full border border-border/60 bg-background px-3 py-1 font-medium tracking-wider">;
    <Sparkles className="size-3" />;
    Autonomous Agent;
    </span>;
    <span className="inline-flex items-center gap-1 rounded-full border border-border/60 bg-background px-3 py-1 font-medium tracking-wider">;
    Full-Stack DeFi Ops;
    </span>;
    <span className="inline-flex items-center gap-1 rounded-full border border-border/60 bg-background px-3 py-1 font-medium tracking-wider">;
    Natural Language Control;
    </span>;
    </div>;
    </div>;
    {/* ElizaOS Attribution Badge */}
    <div className="flex justify-start">;
    <img;
    src="/assets/elizaos_badge.svg";
    alt="Powered by ElizaOS";
    className="h-16";
    />;
    </div>;
    </div>;

    </header>;

    {/* Scrollable Content Area (desktop only). On mobile, the whole modal scrolls */}
    <div className="flex-1 space-y-8 py-6 sm:overflow-y-auto">

    <section className="space-y-4">;

    <div className="flex items-center gap-3 text-xs font-semibold uppercase tracking-[0.25em] text-muted-foreground">;
    <Bullet className="size-2.5" />;
    Capabilities;
    </div>;
    <div className="grid gap-3 md:grid-cols-2">
    {capabilityItems.map((item) => (;
    <div;
    key={item.title}
    className="group rounded-lg border border-border/60 bg-background/80 p-4 transition-colors hover:bg-accent/30"
    >;
    <div className="flex items-start gap-3">;
    <Bullet className="mt-1 size-2.5 shrink-0" />;
    <div className="space-y-1">;
    <h3 className="font-semibold uppercase tracking-wide text-xs text-muted-foreground">;
    {item.title}
    </h3>;
    <p className="text-sm text-foreground/90">;
    {item.description}
    </p>;
    </div>;
    </div>;
    </div>;
    ))}
    </div>;
    </section>;

    <section className="space-y-4">;
    <div className="flex items-center gap-3 text-xs font-semibold uppercase tracking-[0.25em] text-muted-foreground">;
    <Bullet className="size-2.5" />;
    Core Plugins;
    </div>;
    <div className="grid gap-3 sm:grid-cols-2 xl:grid-cols-3">
    {pluginItems.map((plugin) => (;
    <div;
    key={plugin.name}
    className="flex h-full flex-col rounded-lg border border-border/60 bg-background/60 p-4";
    >;
    <div className="flex items-center justify-between gap-2">;
    <div className="flex items-center gap-2 text-xs font-semibold uppercase tracking-[0.2em] text-muted-foreground">;
    <Bullet className="size-2.5" />;
    {plugin.category}
    </div>;
    <span className="text-xs font-semibold uppercase tracking-widest text-muted-foreground/80">;
    {plugin.name}
    </span>;
    </div>;
    <p className="mt-3 text-sm text-muted-foreground">{plugin.summary}</p>;
    <ul className="mt-3 space-y-2 text-sm text-foreground/90">;
    {plugin.points.map((point) => (;
    <li key={point} className="flex gap-2">;
    <span className="mt-1 inline-block size-1.5 rounded-full bg-primary/80" />;
    <span>{point}</span>;
    </li>;
    ))}
    </ul>;
    <p className="mt-4 text-xs font-mono uppercase tracking-widest text-muted-foreground">;
    Example: <span className="text-muted-foreground">{plugin.example}</span>
    </p>;
    </div>;
    ))}
    </div>;
    </section>;

    <section className="space-y-4">;
    <div className="flex items-center gap-3 text-xs font-semibold uppercase tracking-[0.25em] text-muted-foreground">;
    <Bullet className="size-2.5" />;
    Pro Tip;
    </div>;
    <div className="rounded-lg border border-border/60 bg-background/80 p-4">;
    <p className="text-sm text-foreground/90">;
    Ask Otaku to build multi-step playbooks—she remembers previous trades, adapts to your risk;
    settings, and can execute when you are ready.;
    </p>;
    </div>;
    </section>;
    </div>;
    </div>;
    </div>;

    );

}

} // namespace elizaos
