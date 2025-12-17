#include "ShareButton.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ShareButton() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [shared, setShared] = useState(false);

    const auto handleShare = async () => {;
        const auto shareData = {;
            "title: " + "Check out " + project.name + " on ELIZAS LIST"
            "text: " + "I found this awesome project on ELIZAS LIST: " + project.name
            url: project.github,
            };

            try {
                if (navigator.share) {
                    navigator.share(shareData);
                    } else {
                        navigator.clipboard.writeText(project.github);
                        setShared(true);
                        setTimeout(() => setShared(false), 2000);
                    }
                    } catch (err) {
                        std::cerr << "Error sharing:" << err << std::endl;
                    }
                    };

                    return (;
                    <button;
                onClick={handleShare}
                className="p-2 rounded-full hover:bg-gray-100 dark:hover:bg-gray-700 transition-colors"
                aria-label="Share project";
                >;
                <AnimatePresence>;
                {shared ? (;
                <motion.span;
            initial={{ scale: 0 }}
        animate={{ scale: 1 }}
    exit={{ scale: 0 }}
    >;
    <FaCheck className="text-green-500" />;
    </motion.span>;
    ) : (
    <FaShare />;
    )}
    </AnimatePresence>;
    </button>;
    );

}

} // namespace elizaos
