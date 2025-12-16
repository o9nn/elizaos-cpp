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

// Define cookie types locally as CookieProvider will be removed
using CookieType = std::variant<"necessary", "analytics", "preferences">;

default ;

  return (
    <>
      {!tosAccepted && !AllowedRoute ? (
        <div className="h-screen w-full grid place-items-center fixed z-1000 backdrop-blur-xs">
          <div className="max-w-[400px] md:max-w-[496px] w-full bg-autofun-background-card shadow-lg overflow-hidden">
            <div className="p-4 border-b border-autofun-border relative mx-auto">
              <h1 className="mx-auto text-xl text-center font-satoshi font-medium tracking-[-0.018em] text-autofun-text-highlight">
                Welcome to AUTO.FUN [beta]
              </h1>
            </div>
            <div className="p-6 text-center">
              <p className="text-autofun-text-info font-satoshi text-base mb-4">
                Fair launch a meme coin in seconds. When you create a token you
                get to keep a portion of the swap fees to help you grow a real
                community powered by Al content tools!
              </p>
              <p className="text-autofun-text-info font-satoshi text-base mb-6">
                No early dumps — everyone starts at the same line.
              </p>
              <p className="text-autofun-text-info font-satoshi text-base mb-6">
                You earn swap fees — built-in rewards for the creators.
              </p>
              <p className="text-autofun-text-info font-satoshi text-base mb-6">
                Al art + text - instant memes and tweets.
              </p>
              {/* Added Cookie Text */}
              <p className="text-autofun-text-highlight font-satoshi text-base mb-4">
                🍪 A Note About Cookies
              </p>
              <p className="text-autofun-text-info font-satoshi text-base mb-4 px-4">
                We use necessary cookies to make our site work. We'd also like
                to set optional analytics and preference cookies to help us
                improve it and provide the best experience.
              </p>
              <p className="text-autofun-text-info font-satoshi text-base mb-6">
                By clicking below, you're agreeing to our Terms of Service and
                Cookie Policy.
              </p>

              <button
                onClick={acceptTosAndCookies} // Use updated 
              {/* <Link to="/cookie-policy" className="hover:underline">
                Cookie Policy
              </Link> */}
              <Link to="/fees" className="hover:underline">
                Fees
              </Link>
              <Link to="/support" className="hover:underline">
                Support
              </Link>
            </div>
          </div>
        </div>
      ) : null}
    </>
  );
}

} // namespace elizaos
