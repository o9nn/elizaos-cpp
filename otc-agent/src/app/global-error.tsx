"use client";

import { useEffect } from "react";

export default function GlobalError({
  error,
  reset,
}: {
  error: Error & { digest?: string };
  reset: () => void;
}) {
  useEffect(() => {
    // Log the error to an error reporting service
    console.error("[Global Error]:", error);
  }, [error]);

  return (
    <html>
      <body>
        <div
          style={{
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
            minHeight: "100vh",
            fontFamily: "system-ui, -apple-system, sans-serif",
          }}
        >
          <div
            style={{ textAlign: "center", padding: "20px", maxWidth: "500px" }}
          >
            <h2
              style={{
                fontSize: "24px",
                marginBottom: "16px",
                color: "#dc2626",
              }}
            >
              Something went wrong!
            </h2>

            {/* Show error details in development */}
            {process.env.NODE_ENV === "development" && (
              <div
                style={{
                  marginBottom: "20px",
                  padding: "12px",
                  background: "#fef2f2",
                  border: "1px solid #fecaca",
                  borderRadius: "6px",
                  textAlign: "left",
                }}
              >
                <p
                  style={{
                    fontSize: "14px",
                    marginBottom: "8px",
                    fontWeight: "bold",
                    color: "#991b1b",
                  }}
                >
                  Error Details:
                </p>
                <p
                  style={{
                    fontSize: "13px",
                    color: "#7f1d1d",
                    wordBreak: "break-word",
                    marginBottom: "4px",
                  }}
                >
                  {error.message}
                </p>
                {error.digest && (
                  <p
                    style={{
                      fontSize: "11px",
                      color: "#991b1b",
                      marginTop: "8px",
                    }}
                  >
                    Error ID: {error.digest}
                  </p>
                )}
              </div>
            )}

            <button
              onClick={() => reset()}
              style={{
                padding: "10px 20px",
                fontSize: "14px",
                borderRadius: "6px",
                border: "1px solid #ccc",
                background: "#fff",
                cursor: "pointer",
                transition: "all 0.2s",
              }}
              onMouseEnter={(e) => {
                e.currentTarget.style.background = "#f3f4f6";
              }}
              onMouseLeave={(e) => {
                e.currentTarget.style.background = "#fff";
              }}
            >
              Try again
            </button>
          </div>
        </div>
      </body>
    </html>
  );
}
