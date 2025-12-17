#include "useCopyToClipboard.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool fallbackCopy(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto textarea = document.createElement("textarea");
        textarea.value = text;
        textarea.setAttribute("readonly", "");
        textarea.style.position = "absolute";
        textarea.style.left = "-9999px";
        document.body.appendChild(textarea);
        textarea.select();
        const auto ok = document.execCommand("copy");
        document.body.removeChild(textarea);
        return ok;
        } catch {
            return false;
        }

}

std::function<void()> useCopyToClipboard() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [copied, setCopied] = useState<boolean>(false);

    const auto copyToClipboard = useCallback(async (text: string) => {;
        auto success = false;
        if (navigator.clipboard) {
            try {
                navigator.clipboard.writeText(text);
                success = true;
                } catch (err) {
                    std::cerr << "Failed to copy text: " << err << std::endl;
                    // Fallback to execCommand method when clipboard API fails
                    success = fallbackCopy(text);
                }
                } else {
                    // Clipboard API not supported, using fallback method
                    success = fallbackCopy(text);
                }

                if (success) {
                    setCopied(true);
                    setTimeout(() => setCopied(false), 2000);
                }
                return success;
                }, []);

                return [copied, copyToClipboard];

}

} // namespace elizaos
