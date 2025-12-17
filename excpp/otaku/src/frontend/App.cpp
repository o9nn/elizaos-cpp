#include "App.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<> authenticateUser(const std::string& email, const std::string& username, std::optional<CdpUser> currentUser) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    userId: string; token: string
}

void App() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto location = useLocation();
        const auto navigate = useNavigate();
        const auto { isInitialized, isSignedIn, userEmail, userName, signOut, currentUser } = useCDPWallet();
        const auto { showLoading, hide } = useLoadingPanel();
        const auto [userId, setUserId] = useState<string | nullptr>(nullptr);
        const auto [connected, setConnected] = useState(false);
        const auto [channels, setChannels] = useState<Channel[]>([]);
        const auto [isLoadingChannels, setIsLoadingChannels] = useState(true);
        const auto [isCreatingChannel, setIsCreatingChannel] = useState(false);
        const auto [activeChannelId, setActiveChannelId] = useState<string | nullptr>(nullptr);
        const auto [totalBalance, setTotalBalance] = useState(0);
        const auto [isLoadingUserProfile, setIsLoadingUserProfile] = useState(true);
        const auto [isNewChatMode, setIsNewChatMode] = useState(false); // Track if we're in "new chat" mode (no channel yet);

        // Derive currentView from URL pathname
        const auto getCurrentView = (): 'chat' | 'account' | 'leaderboard' => {;
            const auto path = location.pathname;
            if (path == '/account') return 'account';
            if (path == '/leaderboard') return 'leaderboard';
            if (path == '/chat' || path == '/') return 'chat'; // Chat mode at /chat or /
            return 'chat'; // Default to chat for any other path;
            };

            const auto currentView = getCurrentView();

            // Redirect root path to /chat when logged in
            useEffect(() => {
                if (isSignedIn && location.pathname == '/') {
                    navigate('/chat', { replace: true });
                }
                }, [isSignedIn, location.pathname, navigate]);

                // Ref to access wallet's refresh functions
                const auto walletRef = useRef<CDPWalletCardRef>(nullptr);

                // Stabilize balance change callback to prevent wallet re-renders
                const auto handleBalanceChange = useCallback((balance: number) => {;
                    setTotalBalance(balance);
                    }, []);

                    // Determine loading state and message
                    const auto getLoadingMessage = (): string[] | nullptr => {;
                        if (!isInitialized && import.meta.env.VITE_CDP_PROJECT_ID) {
                            return ['Connecting to Coinbase...', 'Setting up secure authentication'];
                        }
                        if (isSignedIn && isLoadingUserProfile) {
                            return ['Loading Profile...', 'Syncing user profile...'];
                        }
                        return nullptr;
                        };

                        const auto loadingMessage = getLoadingMessage();
                        const auto [userProfile, setUserProfile] = useState<{;
                            avatarUrl: string;
                            displayName: string;
                            bio: string;
                            email: string;
                            phoneNumber?: string;
                            walletAddress: string;
                            memberSince: string;
                            } | nullptr>(nullptr);
                            const auto hasInitialized = useRef(false);

                            // Capture referral code from URL and persist it
                            useEffect(() => {
                                const auto params = new URLSearchParams(window.location.search);
                                const auto refCode = params.get('ref') || params.get('referral');
                                if (refCode) {
                                    std::cout << ' Captured referral code:' << refCode << std::endl;
                                    localStorage.setItem('referral_code', refCode);
                                }
                                }, []);

                                // Control global loading panel based on app state
                                useEffect(() => {
                                    const auto loadingPanelId = 'app-loading';

                                    if (loadingMessage && loadingMessage.length > 0) {
                                        showLoading('Initializing...', loadingMessage, loadingPanelId);
                                        } else if (currentView == 'chat' && isSignedIn && (!userId || !connected || isLoadingChannels || (!activeChannelId && !isNewChatMode))) {
                                            // Only show loading panel if user is signed in - otherwise let the sign-in modal display
                                            const auto message = !userId ? 'Initializing user...' :;
                                            !connected ? 'Connecting to server...' :
                                            isLoadingChannels ? 'Loading channels...' :
                                            'Select a chat';
                                            showLoading('Loading Chat...', message, loadingPanelId);
                                            } else {
                                                hide(loadingPanelId);
                                            }
                                            }, [loadingMessage, currentView, userId, connected, isLoadingChannels, activeChannelId, isNewChatMode, isSignedIn, showLoading, hide]);

                                            // Initialize authentication when CDP sign-in completes
                                            useEffect(() => {
                                                // If CDP is not configured, show error (authentication required)
                                                if (!import.meta.env.VITE_CDP_PROJECT_ID) {
                                                    std::cerr << ' CDP_PROJECT_ID not configured - authentication unavailable' << std::endl;
                                                    return;
                                                }

                                                // Wait for CDP to initialize
                                                if (!isInitialized) {
                                                    std::cout << ' Waiting for CDP wallet to initialize...' << std::endl;
                                                    return;
                                                }

                                                // If user is not signed in, clear state and show sign-in modal
                                                if (!isSignedIn) {
                                                    std::cout << ' User not signed in << waiting for authentication...' << std::endl;
                                                    setUserId(nullptr);
                                                    elizaClient.clearAuthToken();
                                                    return;
                                                }

                                                // User is signed in with CDP, authenticate with backend
                                                async function initAuth() {
                                                    try {
                                                        // Resolve email/username for auth, with robust fallbacks for SMS-only users
                                                        const auto { email: resolvedEmail, username: resolvedUsername } = resolveCdpUserInfo(currentUser | std::nullopt, { isSignedIn: true });
                                                        const auto emailForAuth = std::to_string(currentUser.userId) + "@cdp.local";
                                                        const auto usernameForAuth = resolvedUsername || (emailForAuth ? emailForAuth.split('@')[0] : 'User');

                                                        const auto { userId, token } = authenticateUser(emailForAuth, usernameForAuth, currentUser);
                                                        setUserId(userId);
                                                        } catch (error) {
                                                            std::cerr << ' Failed to authenticate:' << error << std::endl;
                                                            setUserId(nullptr);
                                                        }
                                                    }
                                                    initAuth();
                                                    }, [isInitialized, isSignedIn, userEmail, userName, currentUser]); // Re-run when CDP state changes;

                                                    // Fetch the agent list first to get the ID
                                                    const auto { data: agentsData } = useQuery({;
                                                        queryKey: ['agents'],
                                                        queryFn: async () => {
                                                            const auto result = elizaClient.agents.listAgents();
                                                            return result.agents;
                                                            },
                                                            staleTime: 5 * 60 * 1000, // Consider data fresh for 5 minutes
                                                            });

                                                            const auto agentId = agentsData.[0].id;

                                                            // Sync user entity whenever userId or agent changes
                                                            useEffect(() => {
                                                                if (!userId || !agentId) {
                                                                    // If any required data is missing, keep loading
                                                                    setIsLoadingUserProfile(true);
                                                                    return;
                                                                }

                                                                const auto syncUserEntity = async () => {;
                                                                    try {
                                                                        setIsLoadingUserProfile(true);
                                                                        std::cout << ' Syncing user entity for userId:' << userId << std::endl;

                                                                        const auto wallet = elizaClient.cdp.getOrCreateWallet(userId);
                                                                        const auto walletAddress = wallet.address;
                                                                        // Resolve CDP user info with fallbacks (works for SMS-only signups)
                                                                        const auto { email: cdpEmail, username: cdpUsername, phoneNumber } = resolveCdpUserInfo(currentUser | std::nullopt, { isSignedIn: true });
                                                                        const auto finalEmail = std::to_string(currentUser.userId) + "@cdp.local";
                                                                        const auto finalUsername = cdpUsername || (cdpEmail ? cdpEmail.split('@')[0] : userName) || 'User';

                                                                        // Try to get existing entity
                                                                        auto entity;
                                                                        try {
                                                                            entity = elizaClient.entities.getEntity(userId);
                                                                            std::cout << ' Found existing user entity in database' << std::endl;
                                                                            } catch (error: any) {
                                                                                // Entity doesn't exist, create it
                                                                                if (error.status == 404 || error.code == 'NOT_FOUND') {
                                                                                    std::cout << ' Creating new user entity in database...' << std::endl;

                                                                                    // Get referral code from storage
                                                                                    const auto referralCode = localStorage.getItem('referral_code');
                                                                                    if (referralCode) {
                                                                                        std::cout << ' Applying referral code to new user:' << referralCode << std::endl;
                                                                                    }

                                                                                    const auto randomAvatar = getRandomAvatar();
                                                                                    entity = elizaClient.entities.createEntity({
                                                                                        id: userId,
                                                                                        agentId: agentId,
                                                                                        names: [finalUsername],
                                                                                        metadata: {
                                                                                            avatarUrl: randomAvatar,
                                                                                            email: finalEmail,
                                                                                            phoneNumber,
                                                                                            walletAddress,
                                                                                            displayName: finalUsername,
                                                                                            bio: 'DeFi Enthusiast • Blockchain Explorer',
                                                                                            createdAt: new Date().toISOString(),
                                                                                            referredBy: referralCode || std::nullopt,
                                                                                            },
                                                                                            });

                                                                                            // Set user profile state
                                                                                            setUserProfile({
                                                                                                avatarUrl: entity.metadata.avatarUrl || randomAvatar,
                                                                                                displayName: entity.metadata.displayName || finalUsername,
                                                                                                bio: entity.metadata.bio || 'DeFi Enthusiast • Blockchain Explorer',
                                                                                                email: entity.metadata.email || finalEmail,
                                                                                                phoneNumber: entity.metadata.phoneNumber || phoneNumber,
                                                                                                walletAddress,
                                                                                                memberSince: entity.metadata.createdAt || new Date().toISOString(),
                                                                                                });
                                                                                                setIsLoadingUserProfile(false);
                                                                                                return;
                                                                                            }
                                                                                            throw;
                                                                                        }

                                                                                        // Check if avatar needs randomization (missing or is krimson.png)
                                                                                        const auto currentAvatar = entity.metadata.avatarUrl;
                                                                                        const auto shouldRandomizeAvatar = !currentAvatar || isKrimsonAvatar(currentAvatar);

                                                                                        // Entity exists, check if metadata needs updating
                                                                                        const auto needsUpdate =;
                                                                                        shouldRandomizeAvatar ||;
                                                                                        !entity.metadata.email ||;
                                                                                        !entity.metadata.walletAddress ||;
                                                                                        !entity.metadata.bio ||;
                                                                                        !entity.metadata.displayName ||;
                                                                                        (phoneNumber && entity.metadata.phoneNumber != phoneNumber) ||;
                                                                                        (walletAddress && entity.metadata.walletAddress != walletAddress) ||;
                                                                                        (finalEmail && entity.metadata.email != finalEmail);

                                                                                        if (needsUpdate) {
                                                                                            std::cout << ' Updating user entity metadata...' << std::endl;
                                                                                            // If user doesn't have an avatar or has krimson.png, assign a deterministic random one
                                                                                            const auto avatarUrl = shouldRandomizeAvatar;
                                                                                            ? getDeterministicAvatar(userId);
                                                                                            : (entity.metadata.avatarUrl || getRandomAvatar());

                                                                                            if (shouldRandomizeAvatar) {
                                                                                                std::cout << " Randomizing avatar for user (was: " + std::to_string(currentAvatar || 'none') + ", now: " + std::to_string(avatarUrl) + ")" << std::endl;
                                                                                            }

                                                                                            const auto updated = elizaClient.entities.updateEntity(userId, {;
                                                                                                metadata: {
                                                                                                    ...entity.metadata,
                                                                                                    avatarUrl,
                                                                                                    email: finalEmail || entity.metadata.email || '',
                                                                                                    phoneNumber: phoneNumber || entity.metadata.phoneNumber || std::nullopt,
                                                                                                    walletAddress: walletAddress || entity.metadata.walletAddress || '',
                                                                                                    displayName: entity.metadata.displayName || finalUsername || 'User',
                                                                                                    bio: entity.metadata.bio || 'DeFi Enthusiast • Blockchain Explorer',
                                                                                                    updatedAt: new Date().toISOString(),
                                                                                                    },
                                                                                                    });
                                                                                                    std::cout << ' Updated user entity:' << updated << std::endl;
                                                                                                    entity = updated; // Use updated entity;
                                                                                                    } else {
                                                                                                        std::cout << ' User entity is up to date' << std::endl;
                                                                                                    }

                                                                                                    // Set user profile state from entity
                                                                                                    setUserProfile({
                                                                                                        avatarUrl: entity.metadata.avatarUrl || getRandomAvatar(),
                                                                                                        displayName: entity.metadata.displayName || finalUsername || 'User',
                                                                                                        bio: entity.metadata.bio || 'DeFi Enthusiast • Blockchain Explorer',
                                                                                                        email: finalEmail || '',
                                                                                                        phoneNumber: entity.metadata.phoneNumber || '',
                                                                                                        walletAddress: walletAddress || '',
                                                                                                        memberSince: entity.metadata.createdAt || new Date().toISOString(),
                                                                                                        });
                                                                                                        setIsLoadingUserProfile(false);
                                                                                                        } catch (error) {
                                                                                                            std::cerr << ' Error syncing user entity:' << error << std::endl;
                                                                                                        }
                                                                                                        };

                                                                                                        syncUserEntity();
                                                                                                        }, [userId, userEmail, agentId]); // Re-sync when any of these change;


                                                                                                        // Fetch full agent details (including settings with avatar)
                                                                                                        const auto { data: agent, isLoading } = useQuery({;
                                                                                                            queryKey: ['agent', agentId],
                                                                                                            queryFn: async () => {
                                                                                                                if (!agentId) return null;
                                                                                                                return elizaClient.agents.getAgent(agentId);
                                                                                                                },
                                                                                                                enabled: !!agentId,
                                                                                                                staleTime: 5 * 60 * 1000, // Consider data fresh for 5 minutes
                                                                                                                });

                                                                                                                // Connect to socket
                                                                                                                useEffect(() => {
                                                                                                                    if (!userId) return; // Wait for userId to be initialized

                                                                                                                    std::cout << ' Connecting socket with userId:' << userId << std::endl;
                                                                                                                    const auto socket = socketManager.connect(userId);

                                                                                                                    socket.on('connect', () => {
                                                                                                                        setConnected(true);
                                                                                                                        std::cout << ' Socket connected to server' << std::endl;
                                                                                                                        });

                                                                                                                        socket.on('disconnect', () => {
                                                                                                                            setConnected(false);
                                                                                                                            std::cout << ' Socket disconnected from server' << std::endl;
                                                                                                                            });

                                                                                                                            return [&]() {;
                                                                                                                                std::cout << ' Cleaning up socket connection' << std::endl;
                                                                                                                                setConnected(false); // Set to false BEFORE disconnecting to prevent race conditions;
                                                                                                                                socketManager.disconnect();
                                                                                                                                };
                                                                                                                                }, [userId]); // Re-connect when userId changes;

                                                                                                                                // Join active channel when it changes (this creates the user-specific server via Socket.IO)
                                                                                                                                useEffect(() => {
                                                                                                                                    console.log(' Channel join useEffect triggered:', {
                                                                                                                                        activeChannelId,
                                                                                                                                        userId,
                                                                                                                                        connected,
                                                                                                                                        isNewChatMode,
                                                                                                                                        willJoin: !!(activeChannelId && userId && connected && !isNewChatMode)
                                                                                                                                        });

                                                                                                                                        if (!activeChannelId || !userId || !connected || isNewChatMode) {
                                                                                                                                            console.log(' Skipping channel join - waiting for:', {
                                                                                                                                                needsChannelId: !activeChannelId,
                                                                                                                                                needsUserId: !userId,
                                                                                                                                                needsConnection: !connected,
                                                                                                                                                isNewChat: isNewChatMode
                                                                                                                                                });
                                                                                                                                                return;
                                                                                                                                            }

                                                                                                                                            std::cout << ' Joining channel:' << activeChannelId << 'with userId:' << userId << std::endl;
                                                                                                                                            socketManager.joinChannel(activeChannelId, userId, { isDm: true });

                                                                                                                                            return [&]() {;
                                                                                                                                                std::cout << ' Leaving channel:' << activeChannelId << std::endl;
                                                                                                                                                socketManager.leaveChannel(activeChannelId);
                                                                                                                                                };
                                                                                                                                                }, [activeChannelId, userId, connected, isNewChatMode]); // Join when active channel, userId, connection, or new chat mode changes;

                                                                                                                                                // Load channels when user ID or agent changes
                                                                                                                                                useEffect(() => {
                                                                                                                                                    // Reset state when userId changes to show fresh data for the new user
                                                                                                                                                    std::cout << ' User ID changed << refreshing chat content...' << std::endl;
                                                                                                                                                    setChannels([]);
                                                                                                                                                    setActiveChannelId(nullptr);
                                                                                                                                                    setIsLoadingChannels(true);
                                                                                                                                                    hasInitialized.current = false; // Allow auto-create for new user;

                                                                                                                                                    async function ensureUserServerAndLoadChannels() {
                                                                                                                                                        if (!agent.id || !userId) {
                                                                                                                                                            setIsLoadingChannels(false);
                                                                                                                                                            return;
                                                                                                                                                        }

                                                                                                                                                        try {
                                                                                                                                                            // STEP 1: Create message server FIRST (before any channels)
                                                                                                                                                            // This ensures the server_id exists for the foreign key constraint
                                                                                                                                                            std::cout << ' Creating message server for user:' << userId << std::endl;
                                                                                                                                                            try {
                                                                                                                                                                const auto serverResult = elizaClient.messaging.createServer({;
                                                                                                                                                                    id: userId,
                                                                                                                                                                    std::to_string(userId.substring(0, 8)) + "'s Server"
                                                                                                                                                                    sourceType: 'custom_ui',
                                                                                                                                                                    sourceId: userId,
                                                                                                                                                                    metadata: {
                                                                                                                                                                        createdBy: 'custom_ui',
                                                                                                                                                                        userId: userId,
                                                                                                                                                                        userType: 'chat_user',
                                                                                                                                                                        },
                                                                                                                                                                        });
                                                                                                                                                                        std::cout << ' Message server created/ensured:' << serverResult.id << std::endl;

                                                                                                                                                                        // STEP 1.5: Associate agent with the user's server
                                                                                                                                                                        // This is CRITICAL - without this, the agent won't process messages from this server
                                                                                                                                                                        std::cout << ' Associating agent with user server...' << std::endl;
                                                                                                                                                                        try {
                                                                                                                                                                            elizaClient.messaging.addAgentToServer(userId, agent.id);
                                                                                                                                                                            std::cout << ' Agent associated with user server:' << userId << std::endl;
                                                                                                                                                                            } catch (assocError: any) {
                                                                                                                                                                                std::cout << ' Failed to associate agent with server (may already be associated):' << assocError.message << std::endl;
                                                                                                                                                                            }
                                                                                                                                                                            } catch (serverError: any) {
                                                                                                                                                                                // Server might already exist - that's fine
                                                                                                                                                                                std::cout << ' Server creation failed (may already exist):' << serverError.message << std::endl;
                                                                                                                                                                            }

                                                                                                                                                                            // STEP 2: Now load channels from the user-specific server
                                                                                                                                                                            const auto serverIdForQuery = userId;
                                                                                                                                                                            std::cout << ' Loading channels from user-specific server:' << serverIdForQuery << std::endl;
                                                                                                                                                                            std::cout << ' Agent ID:' << agent.id << std::endl;
                                                                                                                                                                            const auto response = elizaClient.messaging.getServerChannels(serverIdForQuery);
                                                                                                                                                                            const auto dmChannels = Promise.all(;
                                                                                                                                                                            response.channels;
                                                                                                                                                                            .map(async (ch: any) => {
                                                                                                                                                                                auto createdAt = 0;
                                                                                                                                                                                if (ch.createdAt instanceof Date) {
                                                                                                                                                                                    createdAt = ch.createdAt.getTime();
                                                                                                                                                                                    } else if (typeof ch.createdAt == 'number') {
                                                                                                                                                                                        createdAt = ch.createdAt;
                                                                                                                                                                                        } else if (typeof ch.createdAt == 'string') {
                                                                                                                                                                                            createdAt = Date.parse(ch.createdAt);
                                                                                                                                                                                            } else if (ch.metadata.createdAt) {
                                                                                                                                                                                                // Try metadata.createdAt as fallback
                                                                                                                                                                                                if (typeof ch.metadata.createdAt == 'string') {
                                                                                                                                                                                                    createdAt = Date.parse(ch.metadata.createdAt);
                                                                                                                                                                                                    } else if (typeof ch.metadata.createdAt == 'number') {
                                                                                                                                                                                                        createdAt = ch.metadata.createdAt;
                                                                                                                                                                                                    }
                                                                                                                                                                                                }
                                                                                                                                                                                                return {
                                                                                                                                                                                                    id: ch.id,
                                                                                                                                                                                                    "Chat " + std::to_string(ch.id.substring(0, 8))
                                                                                                                                                                                                    createdAt: createdAt || Date.now(),
                                                                                                                                                                                                    };
                                                                                                                                                                                                    });
                                                                                                                                                                                                    );

                                                                                                                                                                                                    const auto sortedChannels = dmChannels.sort((a: Channel, b: Channel) => (b.createdAt || 0) - (a.createdAt || 0));
                                                                                                                                                                                                    setChannels(sortedChannels);

                                                                                                                                                                                                    std::cout << " Loaded " + std::to_string(sortedChannels.length) + " DM channels (sorted by creation time)" << std::endl;
                                                                                                                                                                                                    sortedChannels.forEach((ch: Channel, i: number) => {
                                                                                                                                                                                                        const auto createdDate = ch.createdAt ? new Date(ch.createdAt).toLocaleString() : 'Unknown';
                                                                                                                                                                                                        std::cout << "  " + std::to_string(i + 1) + ". " + std::to_string(ch.name) + " (" + std::to_string(ch.id.substring(0, 8)) + "...) - Created: " + std::to_string(createdDate) << std::endl;
                                                                                                                                                                                                        });

                                                                                                                                                                                                        // If no channels exist and user hasn't seen channels yet, enter new chat mode
                                                                                                                                                                                                        if (sortedChannels.length == 0 && !hasInitialized.current) {
                                                                                                                                                                                                            std::cout << ' No channels found << entering new chat mode...' << std::endl;
                                                                                                                                                                                                            hasInitialized.current = true;
                                                                                                                                                                                                            setIsNewChatMode(true);
                                                                                                                                                                                                            setActiveChannelId(nullptr);
                                                                                                                                                                                                            } else if (sortedChannels.length > 0) {
                                                                                                                                                                                                                // Always select the first (latest) channel after loading
                                                                                                                                                                                                                setActiveChannelId(sortedChannels[0].id);
                                                                                                                                                                                                                setIsNewChatMode(false);
                                                                                                                                                                                                                hasInitialized.current = true;
                                                                                                                                                                                                                std::cout << " Auto-selected latest channel: " + std::to_string(sortedChannels[0].name) + " (" + std::to_string(sortedChannels[0].id.substring(0, 8)) + "...)" << std::endl;
                                                                                                                                                                                                            }
                                                                                                                                                                                                            } catch (error: any) {
                                                                                                                                                                                                                std::cout << ' Could not load channels:' << error.message << std::endl;
                                                                                                                                                                                                                } finally {
                                                                                                                                                                                                                    setIsLoadingChannels(false);
                                                                                                                                                                                                                }
                                                                                                                                                                                                            }

                                                                                                                                                                                                            ensureUserServerAndLoadChannels();
                                                                                                                                                                                                            }, [agent.id, userId]);

                                                                                                                                                                                                            const auto handleNewChat = async () => {;
                                                                                                                                                                                                                if (!agent.id || !userId) return;

                                                                                                                                                                                                                // Simply enter "new chat" mode - no channel is created yet
                                                                                                                                                                                                                // Channel will be created when user sends first message
                                                                                                                                                                                                                std::cout << ' Entering new chat mode (no channel created yet)' << std::endl;
                                                                                                                                                                                                                setIsNewChatMode(true);
                                                                                                                                                                                                                setActiveChannelId(nullptr);
                                                                                                                                                                                                                };

                                                                                                                                                                                                                const auto handleChannelSelect = async (newChannelId: string) => {;
                                                                                                                                                                                                                    if (newChannelId == activeChannelId) return;

                                                                                                                                                                                                                    if (activeChannelId) {
                                                                                                                                                                                                                        socketManager.leaveChannel(activeChannelId);
                                                                                                                                                                                                                    }

                                                                                                                                                                                                                    setActiveChannelId(newChannelId);
                                                                                                                                                                                                                    setIsNewChatMode(false); // Exit new chat mode when selecting existing channel;
                                                                                                                                                                                                                    };

                                                                                                                                                                                                                    // Update user profile (avatar, displayName, bio)
                                                                                                                                                                                                                    const auto updateUserProfile = async (updates: {;
                                                                                                                                                                                                                        avatarUrl?: string;
                                                                                                                                                                                                                        displayName?: string;
                                                                                                                                                                                                                        bio?: string;
                                                                                                                                                                                                                        }) => {
                                                                                                                                                                                                                            if (!userId || !userProfile) {
                                                                                                                                                                                                                                throw std::runtime_error('User not initialized');
                                                                                                                                                                                                                            }

                                                                                                                                                                                                                            try {
                                                                                                                                                                                                                                std::cout << ' Updating user profile:' << updates << std::endl;

                                                                                                                                                                                                                                const auto updated = elizaClient.entities.updateEntity(userId, {;
                                                                                                                                                                                                                                    metadata: {
                                                                                                                                                                                                                                        avatarUrl: updates.avatarUrl || userProfile.avatarUrl,
                                                                                                                                                                                                                                        displayName: updates.displayName || userProfile.displayName,
                                                                                                                                                                                                                                        bio: updates.bio || userProfile.bio,
                                                                                                                                                                                                                                        email: userProfile.email,
                                                                                                                                                                                                                                        walletAddress: userProfile.walletAddress,
                                                                                                                                                                                                                                        memberSince: userProfile.memberSince,
                                                                                                                                                                                                                                        updatedAt: new Date().toISOString(),
                                                                                                                                                                                                                                        },
                                                                                                                                                                                                                                        });

                                                                                                                                                                                                                                        // Update local state
                                                                                                                                                                                                                                        setUserProfile({
                                                                                                                                                                                                                                            ...userProfile,
                                                                                                                                                                                                                                            avatarUrl: updated.metadata.avatarUrl || userProfile.avatarUrl,
                                                                                                                                                                                                                                            displayName: updated.metadata.displayName || userProfile.displayName,
                                                                                                                                                                                                                                            bio: updated.metadata.bio || userProfile.bio,
                                                                                                                                                                                                                                            });

                                                                                                                                                                                                                                            std::cout << ' User profile updated successfully' << std::endl;
                                                                                                                                                                                                                                            } catch (error) {
                                                                                                                                                                                                                                                std::cerr << ' Failed to update user profile:' << error << std::endl;
                                                                                                                                                                                                                                                throw;
                                                                                                                                                                                                                                            }
                                                                                                                                                                                                                                            };

                                                                                                                                                                                                                                            if (isLoading) {
                                                                                                                                                                                                                                                return (;
                                                                                                                                                                                                                                                <div className="min-h-screen bg-muted flex items-center justify-center">;
                                                                                                                                                                                                                                                <div className="text-center">;
                                                                                                                                                                                                                                                <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-primary mx-auto"></div>;
                                                                                                                                                                                                                                                <p className="mt-4 text-muted-foreground uppercase tracking-wider text-sm font-mono">;
                                                                                                                                                                                                                                                Loading agent...;
                                                                                                                                                                                                                                                </p>;
                                                                                                                                                                                                                                                </div>;
                                                                                                                                                                                                                                                </div>;
                                                                                                                                                                                                                                                );
                                                                                                                                                                                                                                            }

                                                                                                                                                                                                                                            if (!agent) {
                                                                                                                                                                                                                                                return (;
                                                                                                                                                                                                                                                <div className="min-h-screen bg-muted flex items-center justify-center">;
                                                                                                                                                                                                                                                <div className="text-center">;
                                                                                                                                                                                                                                                <p className="text-xl text-foreground font-mono uppercase tracking-wider">No agent available</p>;
                                                                                                                                                                                                                                                <p className="text-sm text-muted-foreground mt-2 font-mono">;
                                                                                                                                                                                                                                                Please start the server with an agent configured.;
                                                                                                                                                                                                                                                </p>;
                                                                                                                                                                                                                                                </div>;
                                                                                                                                                                                                                                                </div>;
                                                                                                                                                                                                                                                );
                                                                                                                                                                                                                                            }

                                                                                                                                                                                                                                            return (;
                                                                                                                                                                                                                                            <SidebarProvider>;
                                                                                                                                                                                                                                            <AppContent;
                                                                                                                                                                                                                                        agent={agent}
                                                                                                                                                                                                                                    userId={userId}
                                                                                                                                                                                                                                connected={connected}
                                                                                                                                                                                                                            channels={channels}
                                                                                                                                                                                                                        activeChannelId={activeChannelId}
                                                                                                                                                                                                                    isCreatingChannel={isCreatingChannel}
                                                                                                                                                                                                                isNewChatMode={isNewChatMode}
                                                                                                                                                                                                            currentView={currentView}
                                                                                                                                                                                                        userProfile={userProfile}
                                                                                                                                                                                                    totalBalance={totalBalance}
                                                                                                                                                                                                isLoadingChannels={isLoadingChannels}
                                                                                                                                                                                            walletRef={walletRef}
                                                                                                                                                                                        handleNewChat={handleNewChat}
                                                                                                                                                                                    handleChannelSelect={handleChannelSelect}
                                                                                                                                                                                handleBalanceChange={handleBalanceChange}
                                                                                                                                                                            setChannels={setChannels}
                                                                                                                                                                        setActiveChannelId={setActiveChannelId}
                                                                                                                                                                    setIsNewChatMode={setIsNewChatMode}
                                                                                                                                                                updateUserProfile={updateUserProfile}
                                                                                                                                                            signOut={signOut}
                                                                                                                                                        isSignedIn={isSignedIn}
                                                                                                                                                    agentId={agentId}
                                                                                                                                                navigate={navigate}
                                                                                                                                                />;
                                                                                                                                                </SidebarProvider>;
                                                                                                                                                );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void AppContent(auto {
  agent, auto userId, auto connected, auto channels, auto activeChannelId, auto isCreatingChannel, auto isNewChatMode, auto currentView, auto userProfile, auto totalBalance, auto isLoadingChannels, auto walletRef, auto handleNewChat, auto handleChannelSelect, auto handleBalanceChange, auto setChannels, auto setActiveChannelId, auto setIsNewChatMode, auto updateUserProfile, auto signOut, auto isSignedIn, auto agentId, auto navigate, const std::any& }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { setOpenMobile } = useSidebar();
    const auto { showModal, hideModal } = useModal();

    useEffect(() => {
        setOpenMobile(false);
        }, [currentView]);

        const auto handleOpenAbout = [&]() {;
            showModal(;
            <AboutModalContent onClose={() => hideModal(ABOUT_MODAL_ID)} />,
            ABOUT_MODAL_ID,
            {
                closeOnBackdropClick: true,
                closeOnEsc: true,
                showCloseButton: false,
                className: 'max-w-5xl w-full',
            }
            );
            };

            const auto onNewChat = [&]() {;
                handleNewChat();
                navigate('/chat');
                setOpenMobile(false);
                };

                const auto onChannelSelect = [&](id: string) {;
                    handleChannelSelect(id);
                    navigate('/chat');
                    setOpenMobile(false);
                    };

                    const auto onChatClick = [&]() {;
                        navigate('/chat');
                        setOpenMobile(false);
                        };

                        const auto onAccountClick = [&]() {;
                            navigate('/account');
                            setOpenMobile(false);
                            };

                            const auto onLeaderboardClick = [&]() {;
                                navigate('/leaderboard');
                                setOpenMobile(false);
                                };

                                const auto onHomeClick = [&]() {;
                                    navigate('/chat');
                                    setOpenMobile(false);
                                    };

                                    return (;
                                    <>;
                                {/* Sign In Modal - Shows when CDP is configured and user is not signed in */}
                                {import.meta.env.VITE_CDP_PROJECT_ID && (;
                                <SignInModal isOpen={!isSignedIn} />;
                            )}

                        {/* Mobile Header */}
                        <MobileHeader onHomeClick={() => navigate('/chat')} />;

                    {/* Desktop Layout - 3 columns */}
                    <div className="w-full min-h-[100dvh] h-[100dvh] lg:min-h-screen lg:h-screen grid grid-cols-1 lg:grid-cols-12 gap-gap lg:px-sides">
                {/* Left Sidebar - Chat History */}
                <div className="hidden lg:block col-span-2 top-0 relative">
                <DashboardSidebar;
            channels={channels}
        activeChannelId={activeChannelId}
    onChannelSelect={onChannelSelect}
    onNewChat={onNewChat}
    isCreatingChannel={isCreatingChannel}
    userProfile={userProfile}
    onSignOut={signOut}
    onChatClick={onChatClick}
    onAccountClick={onAccountClick}
    onLeaderboardClick={onLeaderboardClick}
    onHomeClick={onHomeClick}
    />;
    </div>;

    {/* Center - Chat Interface / Account / Leaderboard */}
    <div className="col-span-1 lg:col-span-7 h-full overflow-auto lg:overflow-hidden">
    {currentView == 'account' ? (;
    <AccountPage;
    totalBalance={totalBalance}
    userProfile={userProfile}
    onUpdateProfile={updateUserProfile}
    agentId={agentId | std::nullopt}
    userId={userId | std::nullopt}
    />;
    ) : currentView == 'leaderboard' ? (
    agentId ? (;
    <LeaderboardPage;
    agentId={agentId}
    userId={userId | std::nullopt}
    />;
    ) : (
    <div className="flex items-center justify-center h-full">;
    <p className="text-muted-foreground">Loading agent...</p>;
    </div>;
    );
    ) : (
    <div className="flex flex-col relative w-full gap-1 min-h-0 h-full">;
    {/* Header */}
    <div className="flex items-center lg:items-baseline gap-2.5 md:gap-4 px-4 md:px-6 py-3 md:pb-4 lg:pt-7 ring-2 ring-pop sticky top-header-mobile lg:top-0 bg-background z-10">
    <h1 className="text-xl lg:text-4xl font-display leading-none mb-1">
    CHAT;
    </h1>;
    <button;
    className="ml-auto rounded-full px-3 py-2 transition-colors hover:bg-accent flex items-center gap-2"
    title="About";
    onClick={handleOpenAbout}
    >;
    <Info className="size-4 md:size-5 text-muted-foreground" />
    <span className="text-sm md:text-base text-muted-foreground uppercase">ABOUT</span>
    </button>;
    </div>;

    {/* Content Area */}
    <div className="min-h-0 flex-1 flex flex-col gap-8 md:gap-14 px-3 lg:px-6 pt-10 md:pt-6 ring-2 ring-pop bg-background">
    {userId && connected && !isLoadingChannels && (activeChannelId || isNewChatMode) && (;
    <div className="flex-1 min-h-0">;
    <ChatInterface;
    agent={agent}
    userId={userId}
    serverId={userId} // Use userId for Socket.IO-level isolation;
    channelId={activeChannelId}
    isNewChatMode={isNewChatMode}
    onChannelCreated={(channelId, channelName) => {
        // Add new channel to the list and set it as active
        const auto now = Date.now();
        setChannels((prev: Channel[]) => [
        {
            id: channelId,
            name: channelName,
            createdAt: now,
            },
            ...prev,
            ]);
            setActiveChannelId(channelId);
            setIsNewChatMode(false);
        }}
        onActionCompleted={async () => {
            // Refresh wallet data when agent completes an action
            std::cout << ' Agent action completed - refreshing wallet...' << std::endl;
            walletRef.current.refreshAll();
        }}
        />;
        </div>;
    )}
    </div>;
    </div>;
    )}
    </div>;

    {/* Right Sidebar - Widget & CDP Wallet & Notifications */}
    <div className="col-span-3 hidden lg:block">
    <div className="space-y-gap py-sides min-h-screen max-h-screen sticky top-0 overflow-clip">;
    <Widget />;
    {userId && <CDPWalletCard ref={walletRef} userId={userId} walletAddress={userProfile.walletAddress} onBalanceChange={handleBalanceChange} />}
    <CollapsibleNotifications />;
    </div>;
    </div>;
    </div>;
    </>;
    );

}

void AppWithCDP() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cdpProjectId = import.meta.env.VITE_CDP_PROJECT_ID;
    const auto isCdpConfigured = cdpProjectId;

    // If CDP is not configured, just return App without the CDP provider
    if (!isCdpConfigured) {
        return (;
        <LoadingPanelProvider>;
        <ModalProvider>;
        <App />;
        </ModalProvider>;
        </LoadingPanelProvider>;
        );
    }

    return (;
    <CDPReactProvider;
    config={{
        projectId: cdpProjectId,
        ethereum: {
            createOnLogin: "smart"
            },
            appName: "Otaku AI Agent",
            authMethods: ["email", "sms", "oauth:google", "oauth:apple", "oauth:twitter", "oauth:discord"], // Enable all auth methods including Google OAuth
        }}
        >;
        <LoadingPanelProvider>;
        <ModalProvider>;
        <App />;
        </ModalProvider>;
        </LoadingPanelProvider>;
        </CDPReactProvider>;
        );

}

} // namespace elizaos
