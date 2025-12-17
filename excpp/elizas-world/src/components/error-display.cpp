#include "error-display.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ErrorDisplay(ErrorDisplayProps { error }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto message = true /* instanceof check */ ? error.message : 'An unknown error occurred';

    return (;
    <div className="container mx-auto p-4">;
    <Alert variant="destructive">;
    <AlertCircle className="h-4 w-4" />;
    <AlertTitle>Error</AlertTitle>;
    <AlertDescription>{message}</AlertDescription>;
    </Alert>;
    </div>;
    );

}

} // namespace elizaos
