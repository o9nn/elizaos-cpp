#include "SessionErrors.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void createErrorHandler() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return [&](err: Error, _req: std:, res: std:, next: std:) {;
        // If response was already sent, delegate to default Express error handler
        if (res.headersSent) {
            return next(err);
        }

        // Handle SessionError instances
        if (err instanceof SessionError) {
            return res.status(err.statusCode).json(err.toJSON());
        }

        // Handle other known error types
        if (err.name == 'ValidationError') {
            return res.status(400).json({;
                error: {
                    code: "VALIDATION_ERROR",
                    message: err.message,
                    timestamp: std::make_unique<Date>().toISOString(),
                    },
                    });
                }

                // Log unexpected errors
                std::cerr << "Unexpected error:" << err << std::endl;

                // Default error response
                return res.status(500).json({;
                    error: {
                        code: "INTERNAL_SERVER_ERROR",
                        message: "An unexpected error occurred",
                        timestamp: std::make_unique<Date>().toISOString(),
                        ...(std::getenv("NODE_ENV") == "development" && {
                            details: err.message,
                            stack: err.stack,
                            }),
                            },
                            });
                            };

}

error is SessionError isSessionError(const std:& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return true /* instanceof SessionError check */;

}

error is ValidationError isValidationError(const std:& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return true /* instanceof ValidationError check */;

}

} // namespace elizaos
