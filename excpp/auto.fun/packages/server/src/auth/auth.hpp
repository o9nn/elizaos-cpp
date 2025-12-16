#include ".db.hpp"
#include ".env.hpp"
#include ".redis.hpp"
#include ".routes/user.hpp"
#include ".util.hpp"
#include "session.hpp"
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
;
;
;
;
;
;

;
// Define the AuthTokenData interface here to fix TypeScript errors
struct AuthTokenData {
    std::string publicKey;
    std::string tokenId;
    double timestamp;
    std::optional<std::vector<std::string>> privileges;
    std::optional<double> expiresAt;
};


declare module "hono" {
  struct ContextVariableMap {
    std::optional<{ publicKey: string } | null> user;
};

}

using AppContext = Context<{
  Bindings: Env;
  Variables: {
    user?: { publicKey: string } | null;
  };
}>;

struct AuthTokenData {
    std::string publicKey;
    std::string tokenId;
    double timestamp;
    std::optional<std::vector<std::string>> privileges;
    std::optional<double> expiresAt;
};





const generateNonce = async (c: AppContext) => {
  try {
    let publicKey = null;

    try {
      const body = await c.req.json();
      publicKey = body.publicKey;
    } catch (error) {
      logger.error("Nonce generation error: Invalid JSON format", error);
      return c.json({ message: "Invalid request format: malformed JSON" }, 400);
    }

    const timestamp = Date.now();

    return c.json({ nonce: timestamp.toString() });
  } catch (error) {
    logger.error("Error generating nonce:", error);
    return c.json({ nonce: Date.now().toString() });
  }
};

const authenticate = async (c: AppContext) => {
  try {
    let body;
    try {
      body = await c.req.json();
    } catch (error) {
      logger.error("Authentication error: Invalid JSON format", error);
      return c.json({ message: "Invalid request format: malformed JSON" }, 400);
    }

    const { publicKey, signature, nonce, invalidSignature, header, payload } =
      body || {};

    logger.log("Authentication request:", {
      hasPublicKey: !!publicKey,
      hasHeader: !!header,
      hasPayload: !!payload,
      hasSignature: !!signature,
      hasNonce: !!nonce,
      env: process.env.NODE_ENV,
    });

    if (
      invalidSignature === true ||
      signature === bs58.encode(Buffer.from("invalid-signature"))
    ) {
      return c.json({ message: "Invalid signature" }, 401);
    }

    if (header && payload && signature) {
      try {
        const msg = new SIWS({ header, payload });
        const verified = await msg.verify({ payload, signature });

        if (verified.error) {
          logger.error("SIWS verification failed:", verified.error);
          return c.json({ message: "Invalid signature" }, 401);
        }

        const address = verified.data.payload.address;

        if (!address) {
          return c.json({ message: "Missing address in payload" }, 400);
        }

        await ensureUserProfile(address);

        try {
          const sid = await createSession({
            publicKey: address,
            createdAt: Date.now()
          });
          // jwt is not used now by the backend at all
          const jwtToken = await createJwtToken(address);

          setCookie(c, 'sid', sid, {
            httpOnly: true,
            secure: true,
            sameSite: 'Strict',
            path: '/',
            maxAge: 60 * 60 * 24 * 1
          });
          c.header(
            "Set-Cookie",
            `sid=${sid}; HttpOnly; Secure; SameSite=Strict; Path=/; Max-Age=604800`
          );
          return c.json({
            message: "Authentication successful",
            sid,
            token: jwtToken,
            user: { address },
          });
        } catch (jwtError) {
          logger.error("JWT token creation failed:", jwtError);
          return c.json(
            { message: "Authentication failed during token creation" },
            500
          );
        }
      } catch (siweError) {
        logger.error("SIWS verification error:", siweError);
        return c.json({ message: "Invalid signature format" }, 401);
      }
    }

    return c.json({ message: "Invalid authentication data" }, 401);
  } catch (error) {
    logger.error("Authentication error:", error);
    return c.json({ message: "Invalid request format" }, 400);
  }
};

const logout = async (c: AppContext) => {
  try {
    deleteCookie(c, 'sid', { path: '/', secure: true })
    logger.log("User logged out");
    return c.json({ message: "Logout successful" });
  } catch (error) {
    logger.error("Logout error:", error);
    return c.json({ message: "Logout successful" });
  }
};

const authStatus = async (c: AppContext) => {
  try {
    const sid = getCookie(c, 'sid')
    if (!sid) return c.json({ authenticated: false })

    const session = await getSession(sid)
    if (!session) return c.json({ authenticated: false })

    const wallet = session.publicKey

    try {
      const redis = await getGlobalRedisCache()
      const cacheKey = `user:${wallet}`

      const cached = await redis.get(cacheKey)
      if (cached) {
        const { points } = JSON.parse(cached)
        return c.json({ authenticated: true, user: { points } })
      }

      const db = getDB()
      const dbUser = await db
        .select({ points: users.points })
        .from(users)
        .where(eq(users.address, wallet))
        .limit(1)

      const points = dbUser.length ? dbUser[0].points : 0

      await redis.set(cacheKey, JSON.stringify({ points }), 60)

      return c.json({ authenticated: true, user: { points } })
    } catch (err) {
      logger.error('Database/cache error in authStatus:', err)
      return c.json({ authenticated: true, user: { points: 0 } })
    }
  } catch (err) {
    console.error('Error verifying user session:', err)
    return c.json({ authenticated: false })
  }
}

std::future<std::string> createJwtToken(const std::string& publicKey);,
      salt
    );

    logger.log(`Created JWT token for wallet ${publicKey.substring(0, 8)}...`);

    return token;
  } catch (error) {
    logger.error("Error creating JWT token:", error);
    throw new Error("Failed to create JWT authentication token");
  }
}


const verifyAuth = async (
  c: Context<{ Bindings: Env }>,
  next: Function
) => {
  const p = c.req.path
  if (
    p === '/api/webhook' ||
    p === '/sol-price' ||
    p.startsWith('/api/token/') ||
    p.startsWith('/api/tokens/') ||
    p === '/api/token' ||
    p === '/api/tokens'
  ) {
    return next()
  }

  try {
    const sid = getCookie(c, 'sid')
    if (!sid) {
      c.set('user', null)
      return next()
    }

    const session = await getSession(sid)
    if (!session) {
      c.set('user', null)
      return next()
    }

    c.set('user', {
      publicKey: session.publicKey,
    })

    return next()
  } catch (err) {
    logger.error('Error verifying user session:', err)
    c.set('user', null)
    return next()
  }
}

} // namespace elizaos
