#include "ThemeToggle.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ThemeToggle() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { theme, setTheme } = useTheme();

    return (;
    <button;
    onClick={() => setTheme(theme == 'dark' ? 'light' : 'dark')}
    className="fixed bottom-4 right-4 p-3 rounded-full bg-gray-100 dark:bg-gray-800 hover:bg-gray-200 dark:hover:bg-gray-700 transition-colors"
    aria-label="Toggle theme";
    >;
    {theme == 'dark' ? <FaSun className="w-5 h-5" />  = <FaMoon className="w-5 h-5" />}
    </button>;
    );

}

} // namespace elizaos
