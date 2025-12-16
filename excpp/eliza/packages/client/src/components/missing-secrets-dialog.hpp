#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;

struct MissingSecretsDialogProps {
    bool open;
    (open: boolean) => void onOpenChange;
    Array<{ missingSecrets;
    std::string name;
    std::optional<std::string> plugin;
    std::optional<std::string> description;
    () => void onConfirm;
    () => void onCancel;
};


: MissingSecretsDialogProps) {
  return (
    <AlertDialog open={open} onOpenChange={onOpenChange}>
      <AlertDialogContent>
        <AlertDialogHeader>
          <AlertDialogTitle className="flex items-center gap-2">
            <AlertCircle className="h-5 w-5 text-yellow-500" />
            Missing Required Configuration
          </AlertDialogTitle>
          <AlertDialogDescription className="space-y-3">
            <p>
              The following required secrets are missing or empty. Your agent may not  className="text-sm">
                  <code className="font-mono font-semibold">{secret.name}</code>
                  {secret.plugin && (
                    <span className="text-muted-foreground ml-2">
                      (required by {secret.plugin})
                    </span>
                  )}
                  {secret.description && (
                    <div className="text-xs text-muted-foreground mt-1">{secret.description}</div>
                  )}
                </div>
              ))}
            </div>
            <p className="text-sm">Do you want to save anyway?</p>
          </AlertDialogDescription>
        </AlertDialogHeader>
        <AlertDialogFooter>
          <AlertDialogCancel onClick={onCancel}>Go Back</AlertDialogCancel>
          <AlertDialogAction onClick={onConfirm} className="bg-yellow-600 hover:bg-yellow-700">
            Save Anyway
          </AlertDialogAction>
        </AlertDialogFooter>
      </AlertDialogContent>
    </AlertDialog>
  );
}

} // namespace elizaos
