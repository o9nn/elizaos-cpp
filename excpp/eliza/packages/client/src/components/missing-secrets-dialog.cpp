#include "missing-secrets-dialog.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MissingSecretsDialog(auto {
  open, auto onOpenChange, auto missingSecrets, auto onConfirm, auto onCancel, MissingSecretsDialogProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <AlertDialog open={open} onOpenChange={onOpenChange}>;
    <AlertDialogContent>;
    <AlertDialogHeader>;
    <AlertDialogTitle className="flex items-center gap-2">;
    <AlertCircle className="h-5 w-5 text-yellow-500" />;
    Missing Required Configuration;
    </AlertDialogTitle>;
    <AlertDialogDescription className="space-y-3">;
    <p>;
    The following required secrets are missing or empty. Your agent may not function;
    properly without these values.;
    </p>;
    <div className="bg-muted rounded-md p-3 space-y-2">;
    {missingSecrets.map((secret) => (;
    <div key={secret.name} className="text-sm">;
    <code className="font-mono font-semibold">{secret.name}</code>;
    {secret.plugin && (;
    <span className="text-muted-foreground ml-2">;
    (required by {secret.plugin});
    </span>;
    )}
    {secret.description && (;
    <div className="text-xs text-muted-foreground mt-1">{secret.description}</div>;
    )}
    </div>;
    ))}
    </div>;
    <p className="text-sm">Do you want to save anyway?</p>;
    </AlertDialogDescription>;
    </AlertDialogHeader>;
    <AlertDialogFooter>;
    <AlertDialogCancel onClick={onCancel}>Go Back</AlertDialogCancel>;
    <AlertDialogAction onClick={onConfirm} className="bg-yellow-600 hover:bg-yellow-700">
    Save Anyway;
    </AlertDialogAction>;
    </AlertDialogFooter>;
    </AlertDialogContent>;
    </AlertDialog>;
    );

}

} // namespace elizaos
