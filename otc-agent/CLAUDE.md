# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a **multi-chain AI-powered OTC (Over-The-Counter) trading desk** that integrates with the Eliza AI agent framework. It enables users to trade discounted tokens with lockup periods through AI-driven negotiations.

## Development Environment

**Package Manager**: Bun (required - do not use npm)
**Port**: 4444
**Database**: PostgreSQL with pgvector (auto-starts on port 5439)

### Essential Commands

```bash
# Install dependencies
bun install
cd contracts && forge install OpenZeppelin/openzeppelin-contracts && cd ..

# Start full development environment (Anvil + Solana + Next.js + PostgreSQL)
bun run dev

# Run tests (all tests use real blockchain data - NO MOCKS)
bun run test                    # Runtime E2E tests
bun run test:integration       # Full-stack integration tests
bun run test:contracts         # Forge contract tests
bun run test:solana            # Solana program tests
bun run test:complete-flow     # Full system test (auto-starts services)
bun run test:e2e              # Playwright E2E tests (237 tests, 99% coverage)

# Linting
bun run lint                   # Format with Prettier
```

### Running Single Tests
```bash
# Vitest
bun run test -- tests/runtime-e2e.test.ts

# Playwright single test
bun run test:e2e:single e2e/01-pages.spec.ts

# Contract tests
cd contracts && forge test --match-test "testSpecificFunctionName" -vvv
```

## High-Level Architecture

### Core Flow
1. **User Authentication** → Multi-wallet support (EVM via Privy, Solana, Farcaster)
2. **Marketplace Discovery** → Browse consignments with AI-powered filtering
3. **AI Negotiation** → Chat with Eliza agent for custom quotes
4. **Quote Approval** → AI-generated quotes with cryptographic signatures
5. **Deal Execution** → On-chain offer creation → approval → payment → fulfillment
6. **Token Lockup** → Programmable unlock scheduling

### Key Components

**Frontend (src/)**:
- `app/` - Next.js App Router with SSR disabled for wallet-dependent components
- `components/` - Multi-chain UI components with unified wallet abstraction
- `services/` - Backend services using Eliza runtime cache (no traditional DB)
- `lib/` - Eliza agent runtime and OTC desk plugin

**Smart Contracts (contracts/)**:
- `OTC.sol` - Core multi-token consignment system (31k lines)
- Consignment types: Negotiable, Fixed, Private, Fractionalized
- Multi-token registry with per-token price oracles
- Emergency refund mechanisms for failed transactions

**AI Integration (src/lib/)**:
- Eliza Runtime singleton with PostgreSQL backend
- XML-based message parsing with regex quote extraction
- Cryptographic quote signatures for integrity verification

### Critical Design Decisions

1. **No Traditional Database**: Uses Eliza runtime cache system as primary storage
2. **Real Blockchain Testing**: All tests run against actual blockchains - NO MOCKS ALLOWED
3. **Multi-Chain First**: Native support for Base, BSC, Solana
4. **TDD Enforcement**: Tests must pass with real data, never mock data

### Development Principles

**From Cursor Rules**:
- **Runtime E2E Testing Only**: No unit tests, no mocks, no shallow checks
- **Fail-Fast Programming**: Use guard clauses and early returns, catch errors at boundaries only
- **Fix Problems, Never Remove Functionality**: Debug and fix root causes, don't disable features

**TypeScript Standards**:
- Avoid `any` and `unknown`, use precise types
- Minimal nesting (≤2 levels), early returns
- Shared types in `src/types/` or central packages
- Strong type assumptions instead of runtime checks

### Environment Setup

Create `.env.local`:
```bash
NEXT_PUBLIC_PRIVY_APP_ID=your-privy-app-id
NEXT_PUBLIC_RPC_URL=http://127.0.0.1:8545
POSTGRES_URL=postgres://eliza:password@localhost:5439/eliza
GROQ_API_KEY=your-groq-key
COINGECKO_API_KEY=optional-for-token-prices
BIRDEYE_API_KEY=optional-for-solana-prices
```

### Multi-Chain Support

**EVM Chains**: Base, BSC, Sepolia testnets
**Solana**: Devnet/Mainnet with Anchor protocol
**Pricing**: Unified USD pricing across all chains
**Authentication**: Privy for EVM + native Solana wallets + Farcaster social login

### Token Registration

Tokens must be registered before consignment creation:
- Support for any ERC20/SPL token (not elizaOS-specific)
- Price oracles configured per token
- Multi-provider price feeds (CoinGecko, Birdeye)

### Security Features

- Multi-approver system prevents unauthorized transactions
- ReentrancyGuard and Pausable patterns in contracts
- Cryptographic quote verification prevents manipulation
- Emergency refund mechanisms protect users
- Prepaid gas deposits for consignment creation

This architecture solves the trust problem in decentralized OTC trading through cryptographic verification, AI-driven fair pricing, and atomic execution across all settlement steps.