"use client";

import {
  createContext,
  useCallback,
  useContext,
  useEffect,
  useMemo,
  useRef,
  useState,
} from "react";
import { useChainId, useDisconnect, useConnect, useAccount } from "wagmi";
import { base, baseSepolia, bsc, bscTestnet, localhost } from "wagmi/chains";
import {
  usePrivy,
  useWallets,
  type User as PrivyUser,
} from "@privy-io/react-auth";
import { SUPPORTED_CHAINS, type ChainFamily } from "@/config/chains";
import type { EVMChain } from "@/types";
import { useRenderTracker } from "@/utils/render-tracker";

// Interface compatible with @solana/wallet-adapter-react for downstream components
interface SolanaTransaction {
  serialize(): Uint8Array;
  signatures: Array<{
    publicKey: { toBase58(): string };
    signature: Uint8Array | null;
  }>;
}

type SolanaWalletAdapter = {
  publicKey: { toBase58: () => string } | null;
  signTransaction: <T extends SolanaTransaction>(tx: T) => Promise<T>;
  signAllTransactions: <T extends SolanaTransaction>(txs: T[]) => Promise<T[]>;
};

type MultiWalletContextValue = {
  // Active chain family - derived from connection state + user preference
  // null when no wallet is connected
  activeFamily: ChainFamily | null;
  setActiveFamily: (family: ChainFamily) => void;

  // EVM-specific chain selection (Base, BSC, etc.)
  selectedEVMChain: EVMChain;
  setSelectedEVMChain: (chain: EVMChain) => void;

  // Connection status
  isConnected: boolean;
  hasWallet: boolean; // True if any blockchain wallet connected (not just social auth)
  entityId: string | null;
  networkLabel: string;

  // EVM wallet state
  evmConnected: boolean;
  evmAddress: string | undefined;

  // Solana wallet state
  solanaConnected: boolean;
  solanaPublicKey: string | undefined;
  solanaWallet: SolanaWalletAdapter | null;

  // Privy auth state
  privyAuthenticated: boolean;
  privyReady: boolean;
  privyUser: PrivyUser | null;
  isFarcasterContext: boolean;

  // Helpers
  paymentPairLabel: string;
  isPhantomInstalled: boolean;
  currentChainId: number | null;

  // Auth methods
  login: () => void;
  logout: () => Promise<void>;
  connectWallet: () => void;
  connectSolanaWallet: () => void;
  switchSolanaWallet: () => void;
  disconnect: () => Promise<void>;
};

const MultiWalletContext = createContext<MultiWalletContextValue | undefined>(
  undefined,
);

// Solana provider interface (Privy doesn't export these types)
interface SolanaProvider {
  signTransaction: <T extends SolanaTransaction>(tx: T) => Promise<T>;
  signAllTransactions: <T extends SolanaTransaction>(txs: T[]) => Promise<T[]>;
}

// Extended wallet type for accessing Privy's Solana provider
interface PrivySolanaWallet {
  address: string;
  chainType?: string;
  getProvider?: () => Promise<SolanaProvider>;
}

// Window type extension for Phantom wallet detection
type PhantomWindow = Window & {
  phantom?: { solana?: { isPhantom?: boolean } };
};

export function MultiWalletProvider({
  children,
}: {
  children: React.ReactNode;
}) {
  useRenderTracker("MultiWalletProvider");

  const {
    ready: privyReady,
    authenticated: privyAuthenticated,
    user: privyUser,
    login,
    logout,
    connectWallet,
  } = usePrivy();

  const { wallets } = useWallets();
  const { disconnect: disconnectWagmi } = useDisconnect();
  const { connect: connectWagmi, connectors } = useConnect();
  const { isConnected: isWagmiConnected, address: wagmiAddress } = useAccount();
  const chainId = useChainId();

  // Track previous state to avoid logging on every render
  const prevStateRef = useRef<string | null>(null);

  // === Derived wallet state ===
  // Check BOTH Privy wallets array AND wagmi direct connection AND Privy user linkedAccounts
  const privyEvmWallet = useMemo(
    () =>
      wallets.find(
        (w) => (w as { chainType?: string }).chainType === "ethereum",
      ),
    [wallets],
  );
  const privySolanaWallet = useMemo(
    () =>
      wallets.find((w) => (w as { chainType?: string }).chainType === "solana"),
    [wallets],
  );

  // Also check Privy user's linkedAccounts for wallet addresses
  const linkedEvmAddress = useMemo(() => {
    if (!privyUser?.linkedAccounts) return undefined;
    const evmAccount = privyUser.linkedAccounts.find(
      (a) =>
        a.type === "wallet" &&
        (a as { chainType?: string }).chainType === "ethereum",
    );
    return (evmAccount as { address?: string })?.address;
  }, [privyUser?.linkedAccounts]);

  const linkedSolanaAddress = useMemo(() => {
    if (!privyUser?.linkedAccounts) return undefined;
    const solanaAccount = privyUser.linkedAccounts.find(
      (a) =>
        a.type === "wallet" &&
        (a as { chainType?: string }).chainType === "solana",
    );
    return (solanaAccount as { address?: string })?.address;
  }, [privyUser?.linkedAccounts]);

  // Track if we have ACTIVE wallets (in the wallets array) vs just linked accounts
  const hasActiveEvmWallet = !!privyEvmWallet || isWagmiConnected;
  const hasActiveSolanaWallet = !!privySolanaWallet;

  // EVM: connected if Privy has wallet OR wagmi is directly connected OR linked account
  const evmConnected = hasActiveEvmWallet || !!linkedEvmAddress;
  const evmAddress =
    privyEvmWallet?.address || wagmiAddress || linkedEvmAddress;

  // Solana: through Privy wallets array OR linked accounts
  const solanaConnected = hasActiveSolanaWallet || !!linkedSolanaAddress;
  const solanaPublicKey = privySolanaWallet?.address || linkedSolanaAddress;

  // For Solana adapter, use the Privy wallet
  const solanaWalletRaw = privySolanaWallet;

  // === User preference state ===
  // Persisted to localStorage to remember user's chain choice across sessions
  // Use ref to track if we've initialized to avoid re-running initialization logic
  const preferenceInitializedRef = useRef(false);

  const [preferredFamily, setPreferredFamily] = useState<ChainFamily | null>(
    () => {
      if (typeof window === "undefined") return null;
      const saved = localStorage.getItem("otc-preferred-chain");
      if (saved === "evm" || saved === "solana") return saved;
      return null;
    },
  );
  const [selectedEVMChain, setSelectedEVMChainState] =
    useState<EVMChain>("base");

  // Combined effect for localStorage persistence and listening
  // Consolidated to prevent cascading state updates
  // Track preferredFamily in a ref to avoid stale closures in event handlers
  const preferredFamilyRef = useRef(preferredFamily);
  preferredFamilyRef.current = preferredFamily;

  // Persist preference to localStorage - separate effect to avoid loop
  useEffect(() => {
    if (preferredFamily && typeof window !== "undefined") {
      localStorage.setItem("otc-preferred-chain", preferredFamily);
    }
  }, [preferredFamily]);

  // Set up event listeners - only once on mount
  useEffect(() => {
    if (typeof window === "undefined") return;

    // Only set up listeners - don't check localStorage here as initial state handles that
    const handleStorageChange = (e: StorageEvent) => {
      // Only respond to actual storage events from other tabs/windows
      if (e.key !== "otc-preferred-chain") return;
      const saved = e.newValue;
      if (saved === "evm" || saved === "solana") {
        setPreferredFamily(saved);
      }
    };

    const handleCustomEvent = () => {
      const saved = localStorage.getItem("otc-preferred-chain");
      // Use ref to compare to avoid triggering unnecessarily
      if (
        (saved === "evm" || saved === "solana") &&
        saved !== preferredFamilyRef.current
      ) {
        setPreferredFamily(saved);
      }
    };

    window.addEventListener("storage", handleStorageChange);
    window.addEventListener("otc-chain-preference-changed", handleCustomEvent);

    return () => {
      window.removeEventListener("storage", handleStorageChange);
      window.removeEventListener(
        "otc-chain-preference-changed",
        handleCustomEvent,
      );
    };
  }, []); // Empty deps - listeners set up once

  // Auto-set preference when user connects a wallet for the first time after login
  // Using a ref to ensure this only runs once per session
  useEffect(() => {
    // Skip if preference already set or not authenticated
    if (preferredFamily || !privyAuthenticated) return;
    // Skip if we've already tried to initialize this session
    if (preferenceInitializedRef.current) return;

    // Mark as initialized to prevent re-running
    preferenceInitializedRef.current = true;

    // Determine preference based on connected wallets
    // Check Solana FIRST since it's explicit (user chose Solana wallet)
    if (privySolanaWallet) {
      setPreferredFamily("solana");
    } else if (privyEvmWallet || isWagmiConnected) {
      setPreferredFamily("evm");
    }
  }, [
    privyAuthenticated,
    preferredFamily,
    isWagmiConnected,
    privyEvmWallet,
    privySolanaWallet,
  ]);

  // === Derived active family ===
  // Single source of truth: derived from connection state + preference
  const activeFamily = useMemo<ChainFamily | null>(() => {
    // If user has a preference AND that wallet is connected, honor it
    if (preferredFamily === "solana" && solanaConnected) return "solana";
    if (preferredFamily === "evm" && evmConnected) return "evm";

    // No explicit preference - prioritize ACTIVE wallets (in wallets array) over linked accounts
    // If user has an active wallet, use that chain
    if (hasActiveSolanaWallet) return "solana";
    if (hasActiveEvmWallet) return "evm";

    // No active wallets - only linked accounts exist
    // If only one chain is linked, use that
    if (solanaConnected && !evmConnected) return "solana";
    if (evmConnected && !solanaConnected) return "evm";

    // Both linked but no active wallet and no preference
    // Return "evm" as default but log that user should choose
    // The UI should show chain switcher buttons in this case
    if (evmConnected && solanaConnected) {
      console.log(
        "[MultiWallet] Both chains linked, no active wallet - defaulting to EVM. Use chain switcher to change.",
      );
      return "evm";
    }

    if (evmConnected) return "evm";
    if (solanaConnected) return "solana";

    // No wallet connected
    return null;
  }, [
    preferredFamily,
    evmConnected,
    solanaConnected,
    hasActiveEvmWallet,
    hasActiveSolanaWallet,
  ]);

  // === Environment detection ===
  // Use refs to ensure these only run once
  const envDetectionRef = useRef(false);
  const farcasterAutoConnectRef = useRef(false);

  const [isFarcasterContext, setIsFarcasterContext] = useState(false);
  const [isPhantomInstalled, setIsPhantomInstalled] = useState(false);

  useEffect(() => {
    if (typeof window === "undefined" || envDetectionRef.current) return;
    envDetectionRef.current = true;

    // Detect Phantom - check immediately and once after delay
    const checkPhantom = () => {
      const phantomWindow = window as PhantomWindow;
      const hasPhantom = !!phantomWindow.phantom?.solana?.isPhantom;
      setIsPhantomInstalled((prev) =>
        prev !== hasPhantom ? hasPhantom : prev,
      );
    };
    checkPhantom();
    const timer = setTimeout(checkPhantom, 1000);

    // Detect Farcaster
    import("@farcaster/miniapp-sdk")
      .then(({ default: miniappSdk }) => {
        miniappSdk.context
          .then((context) => {
            if (context) {
              setIsFarcasterContext(true);
              miniappSdk.actions.ready();
            }
          })
          .catch(() => {
            /* Not in Farcaster context */
          });
      })
      .catch(() => {
        /* SDK not available */
      });

    return () => clearTimeout(timer);
  }, []);

  // === Farcaster auto-connect ===
  useEffect(() => {
    // Guard against multiple executions
    if (farcasterAutoConnectRef.current) return;
    if (!isFarcasterContext || isWagmiConnected || !connectors?.length) return;

    const farcasterConnector = connectors.find(
      (c) => c.id === "farcasterMiniApp" || c.id === "farcasterFrame",
    );
    if (farcasterConnector) {
      farcasterAutoConnectRef.current = true;
      connectWagmi({ connector: farcasterConnector });
    }
  }, [isFarcasterContext, isWagmiConnected, connectors, connectWagmi]);

  // === Solana wallet adapter ===
  // Track current wallet address to avoid recreating adapter unnecessarily
  const solanaWalletAddressRef = useRef<string | null>(null);
  const [solanaWalletAdapter, setSolanaWalletAdapter] =
    useState<SolanaWalletAdapter | null>(null);

  useEffect(() => {
    let mounted = true;
    const currentAddress = privySolanaWallet?.address ?? null;

    // Skip if wallet address hasn't changed
    if (solanaWalletAddressRef.current === currentAddress) return;
    solanaWalletAddressRef.current = currentAddress;

    async function createAdapter() {
      if (!solanaWalletRaw) {
        if (mounted) setSolanaWalletAdapter(null);
        return;
      }

      try {
        const typedWallet = solanaWalletRaw as PrivySolanaWallet;
        const provider = await typedWallet.getProvider?.();
        if (mounted && provider) {
          setSolanaWalletAdapter({
            publicKey: { toBase58: () => typedWallet.address },
            signTransaction: <T extends SolanaTransaction>(tx: T) =>
              provider.signTransaction(tx),
            signAllTransactions: <T extends SolanaTransaction>(txs: T[]) =>
              provider.signAllTransactions(txs),
          });
        }
      } catch (error) {
        console.error("Failed to create Solana adapter:", error);
        if (mounted) setSolanaWalletAdapter(null);
      }
    }

    createAdapter();
    return () => {
      mounted = false;
    };
  }, [solanaWalletRaw, privySolanaWallet?.address]);

  // === Action handlers ===
  const setActiveFamily = useCallback((family: ChainFamily) => {
    console.log("[MultiWallet] setActiveFamily called with:", family);
    setPreferredFamily(family);
    // Immediately persist to localStorage
    if (typeof window !== "undefined") {
      localStorage.setItem("otc-preferred-chain", family);
    }
  }, []);

  const setSelectedEVMChain = useCallback(
    async (chain: EVMChain) => {
      setSelectedEVMChainState(chain);

      // Only switch chain if we have a Privy-managed wallet (has switchChain method)
      if (privyEvmWallet && evmConnected) {
        const targetChainId = SUPPORTED_CHAINS[chain]?.chainId;
        if (targetChainId) {
          try {
            const currentChainId = parseInt(
              privyEvmWallet.chainId.split(":")[1] || privyEvmWallet.chainId,
            );
            if (currentChainId !== targetChainId) {
              await privyEvmWallet.switchChain(targetChainId);
            }
          } catch {
            // Chain switch failed - not critical
          }
        }
      }
    },
    [privyEvmWallet, evmConnected],
  );

  const connectSolanaWallet = useCallback(
    () => connectWallet(),
    [connectWallet],
  );
  const switchSolanaWallet = useCallback(
    () => connectWallet(),
    [connectWallet],
  );

  const disconnect = useCallback(async () => {
    if (evmConnected) disconnectWagmi();
    await logout();

    if (typeof window !== "undefined") {
      localStorage.removeItem("wagmi.store");
      localStorage.removeItem("wagmi.cache");
      localStorage.removeItem("wagmi.recentConnectorId");
      localStorage.removeItem("privy:token");
      localStorage.removeItem("privy:refresh_token");
      localStorage.removeItem("otc-preferred-chain"); // Clear chain preference on logout
    }

    setPreferredFamily(null);
  }, [evmConnected, disconnectWagmi, logout]);

  // === Derived values ===
  // hasWallet: true if any blockchain wallet is available (active or linked)
  const hasWallet = evmConnected || solanaConnected;
  const isConnected = hasWallet || privyAuthenticated;

  // Debug logging in development - only log when state actually changes
  useEffect(() => {
    if (process.env.NODE_ENV !== "development") return;

    const stateKey = JSON.stringify({
      evmConnected,
      solanaConnected,
      activeFamily,
      hasWallet,
      evmAddress,
      solanaPublicKey,
      preferredFamily,
    });

    // Only log if state actually changed
    if (prevStateRef.current === stateKey) return;
    prevStateRef.current = stateKey;

    console.log("[MultiWallet] State changed:", {
      privyAuthenticated,
      privyReady,
      walletsCount: wallets.length,
      hasActiveEvmWallet,
      hasActiveSolanaWallet,
      linkedEvmAddress,
      linkedSolanaAddress,
      isWagmiConnected,
      evmConnected,
      solanaConnected,
      evmAddress,
      solanaPublicKey,
      preferredFamily,
      activeFamily,
      hasWallet,
    });
  }, [
    evmConnected,
    solanaConnected,
    activeFamily,
    hasWallet,
    evmAddress,
    solanaPublicKey,
    preferredFamily,
    hasActiveEvmWallet,
    hasActiveSolanaWallet,
    privyAuthenticated,
    privyReady,
    wallets.length,
    linkedEvmAddress,
    linkedSolanaAddress,
    isWagmiConnected,
  ]);

  const evmNetworkName = useMemo(() => {
    if (!chainId) return "Unknown";
    const chainNames: Record<number, string> = {
      [localhost.id]: "Anvil",
      [base.id]: "Base",
      [baseSepolia.id]: "Base Sepolia",
      [bsc.id]: "BSC",
      [bscTestnet.id]: "BSC Testnet",
    };
    return chainNames[chainId] ?? `Chain ${chainId}`;
  }, [chainId]);

  const solanaNetworkName =
    process.env.NODE_ENV === "development" ? "Devnet" : "Mainnet";

  const networkLabel = useMemo(() => {
    if (activeFamily === "evm") {
      const chainNames: Record<string, string> = { base: "Base", bsc: "BSC" };
      return chainNames[selectedEVMChain] || evmNetworkName;
    }
    if (activeFamily === "solana") {
      return `Solana ${solanaNetworkName}`;
    }
    // No wallet connected - show auth status
    if (privyAuthenticated) {
      return isFarcasterContext ? "Farcaster" : "Signed In";
    }
    return "Not connected";
  }, [
    activeFamily,
    selectedEVMChain,
    evmNetworkName,
    solanaNetworkName,
    privyAuthenticated,
    isFarcasterContext,
  ]);

  const entityId = useMemo(() => {
    if (activeFamily === "evm" && evmAddress) return evmAddress.toLowerCase();
    if (activeFamily === "solana" && solanaPublicKey) return solanaPublicKey;
    // Fallback for social-only auth
    if (privyAuthenticated && privyUser?.id) return privyUser.id;
    return null;
  }, [
    activeFamily,
    evmAddress,
    solanaPublicKey,
    privyAuthenticated,
    privyUser,
  ]);

  const paymentPairLabel = activeFamily === "solana" ? "USDC/SOL" : "USDC/ETH";

  // === Context value - memoized to prevent unnecessary child re-renders ===
  const value = useMemo<MultiWalletContextValue>(
    () => ({
      activeFamily,
      setActiveFamily,
      selectedEVMChain,
      setSelectedEVMChain,
      isConnected,
      hasWallet,
      entityId,
      networkLabel,
      evmConnected,
      evmAddress,
      solanaConnected,
      solanaPublicKey,
      solanaWallet: solanaWalletAdapter,
      privyAuthenticated,
      privyReady,
      privyUser,
      isFarcasterContext,
      paymentPairLabel,
      isPhantomInstalled,
      currentChainId: chainId ?? null,
      login,
      logout,
      connectWallet,
      connectSolanaWallet,
      switchSolanaWallet,
      disconnect,
    }),
    [
      activeFamily,
      setActiveFamily,
      selectedEVMChain,
      setSelectedEVMChain,
      isConnected,
      hasWallet,
      entityId,
      networkLabel,
      evmConnected,
      evmAddress,
      solanaConnected,
      solanaPublicKey,
      solanaWalletAdapter,
      privyAuthenticated,
      privyReady,
      privyUser,
      isFarcasterContext,
      paymentPairLabel,
      isPhantomInstalled,
      chainId,
      login,
      logout,
      connectWallet,
      connectSolanaWallet,
      switchSolanaWallet,
      disconnect,
    ],
  );

  return (
    <MultiWalletContext.Provider value={value}>
      {children}
    </MultiWalletContext.Provider>
  );
}

// Default values for SSR/prerendering
const defaultContextValue: MultiWalletContextValue = {
  activeFamily: null,
  setActiveFamily: () => {},
  selectedEVMChain: "base",
  setSelectedEVMChain: () => {},
  isConnected: false,
  hasWallet: false,
  entityId: null,
  networkLabel: "",
  evmConnected: false,
  evmAddress: undefined,
  solanaConnected: false,
  solanaPublicKey: undefined,
  solanaWallet: null,
  privyAuthenticated: false,
  privyReady: false,
  privyUser: null,
  isFarcasterContext: false,
  paymentPairLabel: "",
  isPhantomInstalled: false,
  currentChainId: null,
  login: () => {},
  logout: async () => {},
  connectWallet: () => {},
  connectSolanaWallet: () => {},
  switchSolanaWallet: () => {},
  disconnect: async () => {},
};

export function useMultiWallet(): MultiWalletContextValue {
  const ctx = useContext(MultiWalletContext);
  return ctx ?? defaultContextValue;
}
