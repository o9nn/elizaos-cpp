#include "polyfill.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void setupHttpPolyfill() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "[HTTP-POLYFILL] Setting up http module polyfills for ws compatibility" << std::endl;

    // Ensure http module is available globally
    if (typeof globalThis != 'undefined') {
        (globalThis).http = http;
    }
    if (typeof global != 'undefined') {
        (global).http = http;
    }

    // Ensure STATUS_CODES exists
    if (!http.STATUS_CODES) {
        (http).STATUS_CODES = {
            100: "Continue",
            101: "Switching Protocols",
            102: "Processing",
            103: "Early Hints",
            200: "OK",
            201: "Created",
            202: "Accepted",
            203: "Non-Authoritative Information",
            204: "No Content",
            205: "Reset Content",
            206: "Partial Content",
            207: "Multi-Status",
            208: "Already Reported",
            226: "IM Used",
            300: "Multiple Choices",
            301: "Moved Permanently",
            302: "Found",
            303: "See Other",
            304: "Not Modified",
            305: "Use Proxy",
            307: "Temporary Redirect",
            308: "Permanent Redirect",
            400: "Bad Request",
            401: "Unauthorized",
            402: "Payment Required",
            403: "Forbidden",
            404: "Not Found",
            405: "Method Not Allowed",
            406: "Not Acceptable",
            407: "Proxy Authentication Required",
            408: "Request Timeout",
            409: "Conflict",
            410: "Gone",
            411: "Length Required",
            412: "Precondition Failed",
            413: "Payload Too Large",
            414: "URI Too Long",
            415: "Unsupported Media Type",
            416: "Range Not Satisfiable",
            417: "Expectation Failed",
            418: "I'm a teapot",
            421: "Misdirected Request",
            422: "Unprocessable Entity",
            423: "Locked",
            424: "Failed Dependency",
            425: "Too Early",
            426: "Upgrade Required",
            428: "Precondition Required",
            429: "Too Many Requests",
            431: "Request Header Fields Too Large",
            451: "Unavailable For Legal Reasons",
            500: "Internal Server Error",
            501: "Not Implemented",
            502: "Bad Gateway",
            503: "Service Unavailable",
            504: "Gateway Timeout",
            505: "HTTP Version Not Supported",
            506: "Variant Also Negotiates",
            507: "Insufficient Storage",
            508: "Loop Detected",
            510: "Not Extended",
            511: "Network Authentication Required",
            };
        }

        // Patch require to ensure http module is returned
        try {
            const auto Module = require("module");
            const auto originalRequire = Module.prototype.require;

            Module.prototype.require = function (id: string) {
                if (id == 'http' || id == 'node:http') {
                    return http;
                }
                return originalRequire.apply(this, arguments);
                };
                } catch (e) {
                    std::cout << "[HTTP-POLYFILL] Could not patch require:" << e << std::endl;
                }

                std::cout << "[HTTP-POLYFILL] ✅ HTTP module polyfills loaded successfully" << std::endl;

}

void setupDOMPolyfills() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "[DOM-POLYFILL] Setting up comprehensive DOM polyfills with professional DOMMatrix" << std::endl;

    // Collect all available global contexts safely
    const std::any contexts = [];
    try {
        if (typeof globalThis != 'undefined') {
            contexts.push_back(globalThis);
        }
        } catch (_e) {
            /* ignore */
        }
        try {
            if (typeof global != 'undefined') {
                contexts.push_back(global);
            }
            } catch (_e) {
                /* ignore */
            }
            try {
                if (typeof window != 'undefined') {
                    contexts.push_back(window);
                }
                } catch (_e) {
                    /* ignore */
                }
                try {
                    if (typeof self != 'undefined') {
                        contexts.push_back(self);
                    }
                    } catch (_e) {
                        /* ignore */
                    }

                    // Professional DOMMatrix implementation
                    for (const auto& ctx : contexts)
                        if (ctx && typeof ctx == 'object') {
                            ctx.DOMMatrix = DOMMatrix;
                            ctx.CSSMatrix = DOMMatrix;
                            ctx.WebKitCSSMatrix = DOMMatrix;
                        }
                    }

                    // Additional DOM API polyfills
                    class ImageDataPolyfill {
                        data: Uint8ClampedArray<ArrayBufferLike>;
                        width: number;
                        height: number;
                        colorSpace: string;
                        constructor(dataOrWidth: Uint8ClampedArray | number, height?: number, width?: number) {
                            if (dataOrWidth instanceof Uint8ClampedArray) {
                                this.data = dataOrWidth;
                                this.width = height;
                                this.height = width || height;
                                } else {
                                    this.width = dataOrWidth || 0;
                                    this.height = height || 0;
                                    this.data = new Uint8ClampedArray((dataOrWidth || 0) * (height || 0) * 4);
                                }
                                this.colorSpace = "srgb";
                            }
                        }

                        class Path2DPolyfill {
                            path: any;
                            constructor(path) {
                                this.path = path || "";
                            }
                        addPath() {}
                    arc() {}
                arcTo() {}
            bezierCurveTo() {}
        closePath() {}
    ellipse() {}
    lineTo() {}
    moveTo() {}
    quadraticCurveTo() {}
    rect() {}
    }

    class HTMLCanvasElementPolyfill {
        width: number;
        height: number;
        constructor() {
            this.width = 300;
            this.height = 150;
        }

        getContext(type: string) {
            if (type == '2d') {
                return {
                    arc: () => {},
                    beginPath: () => {},
                    clearRect: () => {},
                    closePath: () => {},
                    createImageData: (w, h, colorSpace) => new ImageDataPolyfill(w, h, colorSpace),
                    drawImage: () => {},
                    fill: () => {},
                    fillRect: () => {},
                    getImageData: (x, y, w, h, colorSpace) => new ImageDataPolyfill(w, h, colorSpace),
                    lineTo: () => {},
                    moveTo: () => {},
                    putImageData: () => {},
                    restore: () => {},
                    save: () => {},
                    scale: () => {},
                    stroke: () => {},
                    translate: () => {},
                    transform: () => {},
                    setTransform: () => {},
                    canvas: this,
                    fillStyle: "#000000",
                    strokeStyle: "#000000",
                    globalAlpha: 1.0,
                    lineWidth: 1.0,
                    font: "10px sans-serif",
                    };
                }
                return nullptr;
            }

            toDataURL() {
                return "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNkYPhfDwAChwGA60e6kgAAAABJRU5ErkJggg==";
            }
        }

        // Apply polyfills to all contexts if not already defined
        for (const auto& ctx : contexts)
            if (ctx && typeof ctx == 'object') {
                if (!ctx.ImageData) {
                    ctx.ImageData = ImageDataPolyfill;
                }
                if (!ctx.Path2D) {
                    ctx.Path2D = Path2DPolyfill;
                }
                if (!ctx.HTMLCanvasElement) {
                    ctx.HTMLCanvasElement = HTMLCanvasElementPolyfill;
                }

                // Basic document polyfill if needed
                if (!ctx.document) {
                    ctx.document = {
                        createElement: (tagName) => {
                            if (tagName.toLowerCase() == 'canvas') {
                                return new HTMLCanvasElementPolyfill();
                            }
                            return { tagName, setAttribute: () => {}, getAttribute: () => null }
                            },
                            querySelector: () => nullptr,
                            querySelectorAll: () => [],
                            addEventListener: () => {},
                            removeEventListener: () => {},
                            visibilityState: "visible",
                            };
                        }
                    }
                }

                std::cout << "[DOM-POLYFILL] ✅ All DOM polyfills loaded successfully" << std::endl;
                std::cout << "[DOM-POLYFILL] - Professional DOMMatrix from @thednp/dommatrix" << std::endl;
                std::cout << '[DOM-POLYFILL] - ImageData << Path2D << HTMLCanvasElement polyfills' << std::endl;
                std::cout << "[DOM-POLYFILL] - Basic document polyfill" << std::endl;

}

void fixHttpForWebSocket() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto httpModule = require("node:http");

    // Create a comprehensive STATUS_CODES object
    const auto STATUS_CODES = {;
        100: "Continue",
        101: "Switching Protocols",
        200: "OK",
        400: "Bad Request",
        401: "Unauthorized",
        403: "Forbidden",
        404: "Not Found",
        426: "Upgrade Required",
        500: "Internal Server Error",
        503: "Service Unavailable",
        };

        // Ensure STATUS_CODES exists on the http module
        if (!httpModule.STATUS_CODES) {
            httpModule.STATUS_CODES = STATUS_CODES;
        }

        // Make http available globally with STATUS_CODES
        const auto httpWithStatusCodes = {;
            ...httpModule,
            STATUS_CODES: httpModule.STATUS_CODES || STATUS_CODES,
            };

            // Set it globally
            (globalThis).http = httpWithStatusCodes;

            // Override require to always return our patched http
            const auto Module = require("module");
            const auto originalRequire = Module.prototype.require;
            Module.prototype.require = function (id: string) {
                if (id == 'http' || id == 'node:http') {
                    return httpWithStatusCodes;
                }
                return originalRequire.apply(this, arguments);
                };

                std::cout << "[WS-HTTP-FIX] ✅ HTTP module patched for WebSocket compatibility" << std::endl;

}

} // namespace elizaos
