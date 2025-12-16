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

default </span> does not exist.
            </p>
            <div className="bg-red-900/20 border border-red-800/30 rounded-md p-4 mb-8 text-left">
              <h3 className="text-red-400 font-medium mb-2 flex items-center">
                <AlertCircle className="h-4 w-4 mr-2" />
                Endpoint Not Found
              </h3>
              <p className="text-sm text-red-300/80 mb-2">
                The requested API endpoint does not exist on this server. Please check that:
              </p>
              <ul className="text-sm text-red-300/80 list-disc pl-5 space-y-1">
                <li>The URL is spelled correctly</li>
                <li>You're using the correct version of the API</li>
                <li>The endpoint is available in this version of Eliza</li>
              </ul>
            </div>
          </>
        ) : (
          <p className="text-lg mb-6 text-muted-foreground">
            Sorry, the page you're looking for doesn't exist or has been moved.
          </p>
        )}

        <div className="flex flex-col sm:flex-row gap-4 justify-center">
          <Button asChild variant="default">
            <Link to="/">
              <Home className="h-4 w-4 mr-2" />
              Go to Home
            </Link>
          </Button>

          <Button asChild variant="outline">
            <a onClick={() => window.history.back()}>
              <ArrowLeft className="h-4 w-4 mr-2" />
              Go Back
            </a>
          </Button>
        </div>
      </div>
    </div>
  );
}

} // namespace elizaos
