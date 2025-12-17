#include "errors.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void createErrorResponse(unknown error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof ElizaError) {
        return {
            error: {
                code: error.code,
                message: error.message,
                details: error.details,
                },
                statusCode: error.statusCode,
                };
            }

            if (error instanceof Error) {
                return {
                    error: {
                        code: ErrorCode.INTERNAL_ERROR,
                        message: error.message,
                        },
                        statusCode: 500,
                        };
                    }

                    return {
                        error: {
                            code: ErrorCode.INTERNAL_ERROR,
                            message: 'An unexpected error occurred',
                            },
                            statusCode: 500,
                            };

}

} // namespace elizaos
