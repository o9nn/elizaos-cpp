#include "confirmation-dialog.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ConfirmationDialog(auto onOpenChange, auto title, auto description, auto confirmText, auto cancelText, auto onConfirm, auto variant) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto handleConfirm = [&]() {;
        onConfirm();
        onOpenChange(false);
        };

        return (;
        <AlertDialog open={open} onOpenChange={onOpenChange}>;
        <AlertDialogContent>;
        <AlertDialogHeader>;
        <AlertDialogTitle>{title}</AlertDialogTitle>;
        <AlertDialogDescription>{description}</AlertDialogDescription>;
        </AlertDialogHeader>;
        <AlertDialogFooter>;
        <AlertDialogCancel>{cancelText}</AlertDialogCancel>;
        <AlertDialogAction;
    onClick={handleConfirm}
    className={
        variant == "destructive" ? "bg-destructive text-destructive-foreground" : std::nullopt
    }
    >;
    {confirmText}
    </AlertDialogAction>;
    </AlertDialogFooter>;
    </AlertDialogContent>;
    </AlertDialog>;
    );

}

} // namespace elizaos
