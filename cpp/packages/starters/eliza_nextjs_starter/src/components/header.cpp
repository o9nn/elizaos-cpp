#include "header.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Header() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [mobileMenuOpen, setMobileMenuOpen] = useState(false);
    const auto [mounted, setMounted] = useState(false);

    useEffect(() => setMounted(true), []);

    const auto NavLinks = [&]({ mobile = false }) { return (; };
    <>;
    <Link;
    href="/explore";
    className={clsx(;
    "text-sm font-medium",
    mobile;
    ? "-mx-3 block rounded-lg px-3 py-2 text-base/7 font-semibold text-zinc-900 dark:text-white hover:bg-zinc-50 dark:hover:bg-zinc-900"
    : "text-zinc-600 hover:text-zinc-900 dark:text-zinc-400 dark:hover:text-white",
    )}
    onClick={() => setMobileMenuOpen(false)}
    >;
    Explore;
    </Link>;
    <a;
    href="https://docs.eliza.how/"
    target="_blank";
    rel="noopener noreferrer";
    className={clsx(;
    "text-sm font-medium flex items-center",
    mobile;
    ? "-mx-3 block rounded-lg px-3 py-2 text-base/7 font-semibold text-zinc-900 dark:text-white hover:bg-zinc-50 dark:hover:bg-zinc-900"
    : "text-zinc-500 hover:text-zinc-700 dark:text-zinc-400 dark:hover:text-white",
    )}
    onClick={() => setMobileMenuOpen(false)}
    >;
    Docs;
    </a>;
    <a;
    href="https://twitter.com/elizaos"
    target="_blank";
    rel="noopener noreferrer";
    className={clsx(;
    "text-sm font-medium flex items-center",
    mobile;
    ? "-mx-3 block rounded-lg px-3 py-2 text-base/7 font-semibold text-zinc-900 dark:text-white hover:bg-zinc-50 dark:hover:bg-zinc-900"
    : "text-zinc-500 hover:text-zinc-700 dark:text-zinc-400 dark:hover:text-white",
    )}
    onClick={() => setMobileMenuOpen(false)}
    >;
    <XIcon className="h-4 w-4" />;
    </a>;
    <a;
    href="https://discord.gg/elizaos"
    target="_blank";
    rel="noopener noreferrer";
    className={clsx(;
    "text-sm font-medium flex items-center",
    mobile;
    ? "-mx-3 block rounded-lg px-3 py-2 text-base/7 font-semibold text-zinc-900 dark:text-white hover:bg-zinc-50 dark:hover:bg-zinc-900"
    : "text-zinc-500 hover:text-zinc-700 dark:text-zinc-400 dark:hover:text-white",
    )}
    onClick={() => setMobileMenuOpen(false)}
    >;
    <DiscordIcon className="h-5 w-5" />;
    </a>;
    </>;
    );

    return (;
    <header className="fixed top-0 left-0 right-0 z-10 bg-white dark:bg-black">
    <nav className="px-4 lg:px-6 w-full" aria-label="Global">
    <div className="flex items-center justify-between py-4">;
    <div className="flex">;
    <Link href="/" className="-m-1.5 p-1.5">;
    <Logo width={32} height={32} />;
    </Link>;
    </div>;

    <div className="gap-x-4 flex ml-auto items-center">;
    <NavLinks />;
    </div>;
    </div>;
    </nav>;

    <Dialog;
    open={mobileMenuOpen}
    onClose={setMobileMenuOpen}
    className="lg:hidden"
    variant="slideout";
    >;
    <div className="px-6 py-6 h-full">;
    <div className="flex items-center justify-between">;
    <Link;
    href="/";
    className="-m-1.5 p-1.5";
    onClick={() => setMobileMenuOpen(false)}
    >;
    <Logo width={32} height={32} />;
    </Link>;
    <button;
    type="button";
    className="-m-2.5 rounded-md p-2.5 text-zinc-700 dark:text-zinc-400"
    onClick={() => setMobileMenuOpen(false)}
    >;
    <span className="sr-only">Close menu</span>;
    <XMarkIcon className="h-6 w-6" aria-hidden="true" />;
    </button>;
    </div>;
    <div className="mt-6 flow-root">;
    <div className="space-y-2 py-6">;
    <NavLinks mobile />;
    </div>;
    </div>;
    </div>;
    </Dialog>;
    </header>;
    );

}

} // namespace elizaos
