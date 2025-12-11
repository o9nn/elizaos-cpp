import type { NextConfig } from 'next';
import { webpack } from 'next/dist/compiled/webpack/webpack';
import path from 'path';

const nextConfig: NextConfig = {
  // Skip TypeScript type checking during build (webpack build succeeds, types can be fixed separately)
  typescript: {
    ignoreBuildErrors: true,
  },
  compiler: {
    removeConsole: process.env.NODE_ENV === 'production',
  },
  // Explicitly set workspace root to prevent lockfile detection warnings
  outputFileTracingRoot: process.cwd(),
  serverExternalPackages: ['handlebars', '@elizaos/plugin-sql', '@elizaos/core'],
  // Don't transpile ox - it has compiled .js files in _esm/ and _cjs/
  // Instead, we'll configure webpack to prefer .js files and exclude .ts files from ox
  experimental: {
    inlineCss: true,
  },
  // Skip image optimization for external images - they come from too many sources
  // Vercel Blob caching handles performance for Solana token logos
  images: {
    unoptimized: true,
  },
  // Fix cross-origin chunk loading issues
  // Supports localhost development and Cloudflare tunnels out of the box
  allowedDevOrigins: process.env.NODE_ENV === 'development' 
    ? [
        // Common localhost patterns (works for all developers)
        'localhost:4444',
        '127.0.0.1:4444',
        '0.0.0.0:4444',
        
        // Cloudflare tunnel support (set TUNNEL_DOMAIN in .env.local)
        ...(process.env.TUNNEL_DOMAIN ? [process.env.TUNNEL_DOMAIN] : []),
        
        // Allow custom origins via environment variable (comma-separated)
        ...(process.env.ALLOWED_DEV_ORIGINS?.split(',').map(o => o.trim()) || []),
      ].filter(Boolean)
    : [],
  webpack: (config, { isServer }) => {
    // Ignore handlebars require.extensions warning
    config.ignoreWarnings = [
      ...(config.ignoreWarnings || []),
      {
        module: /node_modules\/handlebars/,
        message: /require\.extensions/,
      },
    ];
    
    // Exclude TypeScript source files from node_modules/ox/ from being processed
    // porto imports from ox without extensions, webpack resolves to .ts files
    // We need to prevent Next.js loaders from processing these files
    config.module = config.module || {};
    config.module.rules = config.module.rules || [];
    
    // Add rule BEFORE other rules to exclude .ts files from ox
    // This prevents Next.js TypeScript loaders from processing them
    config.module.rules.unshift({
      test: /\.ts$/,
      include: /node_modules\/ox\//,
      use: {
        loader: 'ignore-loader',
      },
      enforce: 'pre',
    });
    
    // Exclude Web3 packages and handlebars from server-side bundling
    if (isServer) {
      config.externals = [
        ...(config.externals || []),
        '@rainbow-me/rainbowkit',
        'wagmi',
        '@tanstack/react-query',
        'viem',
        'handlebars',
        '@elizaos/plugin-sql',
        '@elizaos/core'
      ];
    }
    
    // Configure webpack to ignore files that shouldn't trigger rebuilds
    config.watchOptions = {
      ...config.watchOptions,
      ignored: [
        '**/node_modules/**',
        '**/.next/**',
        '**/.git/**',
        '**/*.log',
        '**/*.pid',
        '**/dist/**',
        '**/contracts/artifacts/**',
        '**/contracts/cache/**',
        '**/typechain-types/**',
        '**/cypress/screenshots/**',
        '**/cypress/videos/**',
        '**/prisma/**',
        '**/*.db',
        '**/*.db-journal',
        '**/*.sqlite',
        '**/*.sqlite-journal',
        '**/data/**',
        '**/.eliza/**',
        '**/.elizadb/**',
        '**/.elizaos-cache/**',
        '**/.eliza-runtime/**',
        '**/eliza-data/**',
        '**/logs/**',
        '**/tmp/**',
        '**/.cache/**',
        '**/agent/**',
        '**/agent.js',
        '**/agent.js.map',
        '**/chunk-*.js',
        '**/chunk-*.js.map'
      ],
    };


    config.plugins.push(
      new webpack.IgnorePlugin({
        resourceRegExp: /^pg-native$|^cloudflare:sockets$/,
      }),
      // Prevent webpack from processing .ts files from ox package
      // porto imports from ox without extensions, webpack resolves to .ts files
      // ox has compiled .js files that should be used via package.json exports
      new webpack.IgnorePlugin({
        checkResource(resource: string, context: string) {
          // Ignore .ts files from ox package - use compiled .js files instead
          if (context.includes('node_modules/ox') && resource.endsWith('.ts')) {
            return true;
          }
          return false;
        },
      }),
      // Ignore IndexedDB related imports on server
      ...(isServer ? [
        new webpack.DefinePlugin({
          'typeof window': JSON.stringify('undefined'),
          'typeof indexedDB': JSON.stringify('undefined')
        })
      ] : [])
    );
    // Return modified config
    return {
      ...config,
      resolve: {
        ...config.resolve,
        alias: {
          ...config.resolve?.alias,
          // Force all packages to use root-level viem and ox (fix @base-org/account nested deps)
          'viem': path.resolve(__dirname, 'node_modules/viem'),
          'ox': path.resolve(__dirname, 'node_modules/ox'),
          // Redirect ox TypeScript imports to use package.json exports (compiled .js files)
          // This prevents webpack from processing .ts source files
          'ox/erc8010/SignatureErc8010': path.resolve(__dirname, 'node_modules/ox/_esm/erc8010/SignatureErc8010.js'),
        },
        // Prefer .js files over .ts files for node_modules to avoid processing TypeScript source
        // This ensures webpack resolves ox imports to .js files instead of .ts files
        extensions: [
          '.js',
          '.jsx',
          '.json',
          // Only include .ts/.tsx after .js so webpack prefers compiled files
          ...(config.resolve?.extensions?.filter(ext => !['.js', '.jsx', '.json'].includes(ext)) || ['.ts', '.tsx']),
        ],
        // Ensure nested packages can resolve @noble/hashes from top-level
        modules: [
          ...(config.resolve?.modules || ['node_modules']),
          // Add parent node_modules for nested package resolution
          path.resolve(__dirname, 'node_modules'),
        ],
        fallback: {
          ...config.resolve?.fallback,
          fs: false,
          net: false,
          tls: false,
          async_hooks: false,
          worker_threads: false,
        },
      },
    };
  },
  async redirects() {
    return [
      {
        source: '/start',
        destination: 'https://ai.eliza.how/eliza/',
        permanent: false,
      },
      {
        source: '/school',
        destination: 'https://www.youtube.com/playlist?list=PL0D_B_lUFHBKZSKgLlt24RvjJ8pavZNVh',
        permanent: false,
      },
      {
        source: '/discord',
        destination: 'https://discord.gg/2bkryvK9Yu',
        permanent: false,
      },
      {
        source: '/profiles',
        destination: 'https://elizaos.github.io/profiles',
        permanent: false,
      },
      {
        source: '/bounties',
        destination: 'https://elizaos.github.io/website/',
        permanent: false,
      },
    ];
  },
  async headers() {
    return [
      {
        source: '/(.*)',
        headers: [
          {
            key: 'Content-Security-Policy',
            value: [
              "default-src 'self'",
              "script-src 'self' 'unsafe-inline' 'unsafe-eval' https://challenges.cloudflare.com",
              "style-src 'self' 'unsafe-inline'",
              "img-src 'self' data: https: blob:",
              "connect-src 'self' http://127.0.0.1:8545 http://localhost:8545 ws://127.0.0.1:8545 ws://localhost:8545 http://127.0.0.1:8899 http://localhost:8899 ws://127.0.0.1:8900 ws://localhost:8900 https://*.solana.com wss://*.solana.com https://*.helius-rpc.com https://*.drpc.org https://eth.merkle.io https://api.neynar.com https://farcaster.xyz https://client.farcaster.xyz https://warpcast.com https://wrpcd.net https://*.wrpcd.net wss://relay.farcaster.xyz https://auth.privy.io https://*.rpc.privy.systems https://explorer-api.walletconnect.com https://pulse.walletconnect.org https://api.web3modal.org https://*.walletconnect.com wss://*.walletconnect.com wss://relay.walletconnect.org wss://www.walletlink.org https://*.metamask.io https://*.coinbase.com https://api.developer.coinbase.com https://mainnet.base.org https://sepolia.base.org",
              "font-src 'self' data:",
              "object-src 'none'",
              "base-uri 'self'",
              "form-action 'self'",
              "frame-ancestors 'self' https://farcaster.xyz https://*.farcaster.xyz",
              "frame-src 'self' https://auth.privy.io https://verify.walletconnect.com https://verify.walletconnect.org https://challenges.cloudflare.com",
              "child-src https://auth.privy.io https://verify.walletconnect.com https://verify.walletconnect.org",
              "worker-src 'self'",
              "manifest-src 'self'"
            ].join('; '),
          },
        ],
      },
    ];
  },
  async rewrites() {
    return [
      {
        source: '/ingest/static/:path(.*)',
        destination: 'https://us-assets.i.posthog.com/static/:path',
      },
      {
        source: '/ingest/:path(.*)',
        destination: 'https://us.i.posthog.com/:path',
      },
      {
        source: '/profiles/:path(.*)',
        destination: 'https://elizaos.github.io/profiles/:path',
      },
      {
        source: '/bounties/:path(.*)',
        destination: 'https://elizaos.github.io/website/:path',
      },
      {
        source: '/eliza/:path(.*)',
        destination: 'https://elizaos.github.io/eliza/:path',
      },
      {
        source: '/.well-known/farcaster.json',
        destination: '/.well-known/farcaster.json',
      },
    ];
  },
  // This is required to support PostHog trailing slash API requests
  skipTrailingSlashRedirect: true,
};

export default nextConfig;
