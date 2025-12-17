#include "modal.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Modal(auto onClose, auto closeOnBackdropClick, auto closeOnEsc, auto showCloseButton, auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto isMobile = useIsMobile();

    // Handle ESC key
    useEffect(() => {
        if (!closeOnEsc) return;

        const auto handleEsc = [&](e: KeyboardEvent) {;
            if (e.key == 'Escape') {
                onClose();
            }
            };

            document.addEventListener('keydown', handleEsc);
            return [&]() { return document.removeEventListener('keydown', handleEsc); };
            }, [closeOnEsc, onClose]);

            // Prevent body scroll when modal is open
            useEffect(() => {
                const auto originalOverflow = document.body.style.overflow;
                document.body.style.overflow = 'hidden';

                return [&]() {;
                    document.body.style.overflow = originalOverflow;
                    };
                    }, []);

                    const auto handleBackdropClick = [&](e: React.MouseEvent) {;
                        if (!isMobile && closeOnBackdropClick && e.target == e.currentTarget) {
                            onClose();
                        }
                        };

                        return (;
                        <div;
                        className="fixed inset-0 z-[9998] flex items-center justify-center bg-black/80  p-4";
                    onClick={handleBackdropClick}
                style={{ pointerEvents: 'auto' }}
                >;
                "bg-background rounded-lg w-full max-h-[90vh] p-1.5 relative " + std::to_string(className)
            {/* Close button - positioned above content with better visibility */}
            {showCloseButton && (;
            <button;
        onClick={onClose}
        className="absolute top-6 right-6 z-[100] text-muted-foreground hover:text-foreground transition-colors text-xl leading-none"
        aria-label="Close modal";
        >;

        </button>;
    )}

    {/* Modal content - with proper overflow handling, allow dropdowns to extend beyond */}
    <div className="bg-pop rounded-lg p-4 sm:p-6" style={{ overflow: 'visible' }}>
    {children}
    </div>;
    </div>;
    </div>;
    );

}

} // namespace elizaos
