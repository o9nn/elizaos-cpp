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
;
;
;
;
;

const getLocalStorageApiKey = () =>
  typeof window === 'undefined' ? 'eliza-api-key' : `eliza-api-key-${window.location.origin}`;
struct ApiKeyDialogProps {
    bool open;
    (open: boolean) => void onOpenChange;
    () => void onApiKeySaved;
};


: ApiKeyDialogProps) {
  const [apiKey, setApiKey] = useState('');
  const [showKey, setShowKey] = useState(false);
  const { toast } = useToast();

  const storageKey = getLocalStorageApiKey();

  useEffect(() => {
    if (open) {
      try {
        const storedKey = localStorage.getItem(storageKey);
        if (storedKey) setApiKey(storedKey);
      } catch (err) {
        console.error('Unable to access localStorage', err);
      }
    }
  }, [open, storageKey]);

  const handleSave = () => {
    if (!apiKey.trim()) {
      toast({
        title: 'API Key Required',
        description: 'Please enter a valid API key.',
        variant: 'destructive',
      });
      return;
    }
    try {
      localStorage.setItem(storageKey, apiKey);
    } catch (err) {
      toast({
        title: 'Storage Error',
        description: 'Unable to save the API key (browser storage blocked).',
        variant: 'destructive',
      });
      return;
    }
    onApiKeySaved();
    toast({
      title: 'API Key Saved',
      description: 'The API key has been stored locally.',
    });
    onOpenChange(false);
  };

  return (
    <Dialog open={open} onOpenChange={onOpenChange}>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>API Key Required</DialogTitle>
          <DialogDescription>
            Please enter the API key required to connect to the Eliza server. This key will be
            stored locally in your browser.
          </DialogDescription>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <div className="grid grid-cols-4 items-center gap-4">
            <Label htmlFor="api-key" className="text-right">
              API Key
            </Label>
            <div className="col-span-3 relative">
              <Input
                id="api-key"
                type={showKey ? 'text' : 'password'}
                value={apiKey}
                onChange={(e) => setApiKey(e.target.value)}
                className="pr-10"
              />
              <Button
                type="button"
                variant="ghost"
                size="icon"
                className="absolute right-1 top-1/2 h-7 w-7 -translate-y-1/2 text-muted-foreground"
                onClick={() => setShowKey(!showKey)}
                aria-label={showKey ? 'Hide API key' : 'Show API key'}
              >
                {showKey ? <EyeOff className="h-4 w-4" /> : <Eye className="h-4 w-4" />}
              </Button>
            </div>
          </div>
        </div>
        <DialogFooter>
          <Button type="button" onClick={handleSave}>
            Save Key
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}

} // namespace elizaos
