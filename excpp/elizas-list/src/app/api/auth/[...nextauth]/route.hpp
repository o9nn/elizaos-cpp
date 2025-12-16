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

import NextAuth, { AuthOptions } from 'next-auth';
;
;
;

// Define auth options separately with proper typing
const authOptions: AuthOptions = {
  adapter: PrismaAdapter(prisma),
  providers: [
    GithubProvider({
      clientId: process.env.GITHUB_ID || '',
      clientSecret: process.env.GITHUB_SECRET || '',
    }),
  ],
  secret: process.env.NEXTAUTH_SECRET,
  session: {
    strategy: 'jwt'
  },
  pages: {
    signIn: '/auth/signin',
  }
};

// Create and the handler
const handler = NextAuth(authOptions);

// Export the handler functions
{ handler as GET, handler as POST };

// Remove edge runtime configuration to use Node.js runtime instead 
} // namespace elizaos
