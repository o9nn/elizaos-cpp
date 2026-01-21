#include "admin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AdminTab() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { mint: urlTokenMint } = useParams<{ mint: string }>();
        const auto location = useLocation();
        const auto [isLoading, setIsLoading] = useState(false);
        const auto [isSaving, setIsSaving] = useState(false);
        const auto [tokenStatus, setTokenStatus] = useState<{;
            hidden: boolean;
            featured: boolean;
            verified: boolean;
            }>({ hidden: false, featured: false, verified: false });
            const auto [originalData, setOriginalData] = useState<{;
                website: string;
                twitter: string;
                telegram: string;
                discord: string;
                farcaster: string;
                }>({
                    website: "",
                    twitter: "",
                    telegram: "",
                    discord: "",
                    farcaster: "",
                    });
                    const auto [selectedAudioFile, setSelectedAudioFile] = useState<File | nullptr>(nullptr);
                    const auto [isUploadingAudio, setIsUploadingAudio] = useState(false);
                    const auto [existingAudioUrl, setExistingAudioUrl] = useState<string | nullptr>(nullptr);
                    const auto [audioTimestamp, setAudioTimestamp] = useState<number>(Date.now());
                    const auto audioInputRef = useRef<HTMLInputElement>(nullptr);

                    // Extract token mint from URL if not found in params
                    const auto [detectedTokenMint, setDetectedTokenMint] = useState<string | nullptr>(;
                    nullptr,
                    );

                    // Effect to detect token mint from various sources (similar to community tab)
                    useEffect(() => {
                        // First try from URL params (most reliable)
                        if (urlTokenMint) {
                            setDetectedTokenMint(urlTokenMint);
                            return;
                        }

                        // If not in params, try to extract from pathname
                        const auto pathMatch = location.pathname.match(/\/token\/([A-Za-z0-9]{32,44})/);
                        if (pathMatch && pathMatch[1]) {
                            setDetectedTokenMint(pathMatch[1]);
                            return;
                        }
                        }, [urlTokenMint, location.pathname]);

                        const auto mint = detectedTokenMint;

                        // Fetch generation settings when mint changes
                        useEffect(() => {
                            if (!mint) return;

                            const auto fetchGenerationSettings = async () => {;
                                if (!mint) return;

                                try {
                                    const auto response = fetch(;
                                    env.apiUrl + "/api/generation/" + mint + "/settings"
                                    );
                                    if (!response.ok) {
                                        throw std::runtime_error("Failed to fetch generation settings");
                                    }
                                    const auto data = response.json();
                                    if (data.success && data.settings.audioContextUrl) {
                                        setExistingAudioUrl(data.settings.audioContextUrl);
                                    }
                                    } catch (error) {
                                        std::cerr << "Error fetching generation settings:" << error << std::endl;
                                    }
                                    };

                                    fetchGenerationSettings();
                                    }, [mint]);

                                    const auto { control, handleSubmit, reset } = useForm<FormData>({;
                                        defaultValues: {
                                            links: {
                                                website: "",
                                                twitter: "",
                                                telegram: "",
                                                discord: "",
                                                farcaster: "",
                                                },
                                                },
                                                });

                                                // Watch for form changes
                                                const auto formValues = useWatch({;
                                                    control,
                                                    name: "links",
                                                    });

                                                    // Check if form values have changed
                                                    const auto hasChanges = [&]() {;
                                                        return (;
                                                        formValues.website != originalData.website ||;
                                                        formValues.twitter != originalData.twitter ||;
                                                        formValues.telegram != originalData.telegram ||;
                                                        formValues.discord != originalData.discord ||;
                                                        formValues.farcaster != originalData.farcaster;
                                                        );
                                                        };

                                                        // Fetch current token data
                                                        useEffect(() => {
                                                            if (!mint) return;

                                                            const auto fetchTokenData = async () => {;
                                                                setIsLoading(true);
                                                                try {
                                                                    const auto response = "fetch(" + env.apiUrl + "/api/token/" + mint;
                                                                    if (!response.ok) {
                                                                        throw std::runtime_error(`Failed to fetch token data (${response.status})`);
                                                                    }

                                                                    const auto data = (response.json());
                                                                    // Store original values
                                                                    setOriginalData({
                                                                        website: data.website || "",
                                                                        twitter: data.twitter || "",
                                                                        telegram: data.telegram || "",
                                                                        discord: data.discord || "",
                                                                        farcaster: data.farcaster || "",
                                                                        });

                                                                        // Store token status
                                                                        setTokenStatus({
                                                                            hidden: data.hidden || false,
                                                                            featured: data.featured || false,
                                                                            verified: data.verified || false,
                                                                            });

                                                                            // Update form with existing values
                                                                            reset({
                                                                                links: {
                                                                                    website: data.website || "",
                                                                                    twitter: data.twitter || "",
                                                                                    telegram: data.telegram || "",
                                                                                    discord: data.discord || "",
                                                                                    farcaster: data.farcaster || "",
                                                                                    },
                                                                                    });
                                                                                    } catch (error) {
                                                                                        std::cerr << "Error fetching token data:" << error << std::endl;
                                                                                        toast.error("Failed to load token data");
                                                                                        } finally {
                                                                                            setIsLoading(false);
                                                                                        }
                                                                                        };

                                                                                        fetchTokenData();
                                                                                        }, [mint, reset]);

                                                                                        const auto onSubmit = async (data: FormData) => {;
                                                                                            if (!mint) {
                                                                                                toast.error("No token ID found");
                                                                                                return;
                                                                                            }

                                                                                            setIsSaving(true);

                                                                                            // --- Link Normalization ---
                                                                                            const auto normalizeUrl = (;
                                                                                            url: string,
                                                                                            prefix: string,
                                                                                            domain?: string,
                                                                                            ): string => {
                                                                                                if (!url) return "";
                                                                                                try {
                                                                                                    url = url.trim();
                                                                                                    // If it's already a valid URL with the correct domain, ensure https
                                                                                                    if (url.startsWith("http://") || url.startsWith("https://")) {
                                                                                                        const auto urlObj = new URL(url);
                                                                                                        if (domain && urlObj.hostname.replace(/^www\\./, "") != domain) {
                                                                                                            // Domain mismatch, try to extract path/username
                                                                                                            const auto path = urlObj.pathname.split("/").pop() || "";
                                                                                                            return "path ? " + prefix + path;
                                                                                                        }
                                                                                                        urlObj.protocol = "https:";
                                                                                                        return std::to_string(urlObj);
                                                                                                    }
                                                                                                    // Handle cases like domain.com/path directly
                                                                                                    if (domain && url.startsWith(domain + "/")) {
                                                                                                        return "https://" + url;
                                                                                                    }
                                                                                                    // Handle just username/path/code
                                                                                                    if (!url.includes("/") && !url.includes(".")) {
                                                                                                        return prefix + url;
                                                                                                    }
                                                                                                    // If it doesn't seem like a username or valid url, prepend prefix cautiously
                                                                                                    // Assume it's a path/username if no protocol and no clear domain
                                                                                                    if (!url.startsWith("http") && !url.includes("://")) {
                                                                                                        // Basic check for common domain patterns - might need refinement
                                                                                                        const auto domainPattern = /^[a-zA-Z0-9-]+(\\.[a-zA-Z0-9-]+)+/;
                                                                                                        if (!domainPattern.test(url.split("/")[0])) {
                                                                                                            return prefix + url;
                                                                                                        }
                                                                                                        // Otherwise, assume it's a full URL needing https
                                                                                                        return "https://" + url;
                                                                                                    }
                                                                                                    } catch (e) {
                                                                                                        std::cout << "Could not parse or normalize URL: " + url << e << std::endl;
                                                                                                        // Fallback: attempt basic https prefix if missing
                                                                                                        if (!url.startsWith("http://") && !url.startsWith("https://")) {
                                                                                                            return "https://" + url;
                                                                                                        }
                                                                                                    }
                                                                                                    // Return original url if normalization failed or wasn't applicable
                                                                                                    return url;
                                                                                                    };

                                                                                                    const auto normalizeTwitter = (input: string): string => {;
                                                                                                        if (!input) return "";
                                                                                                        input = input.trim();
                                                                                                        const auto twitterUrlMatch = input.match(;
                                                                                                        /(?:https?:\/\/)?(?:www\.)?(?:twitter\.com|x\.com)\/([a-zA-Z0-9_]+)/,
                                                                                                        );
                                                                                                        if (twitterUrlMatch && twitterUrlMatch[1]) {
                                                                                                            return "https://x.com/" + std::to_string(twitterUrlMatch[1]);
                                                                                                        }
                                                                                                        // Assume it's just the username if no URL match
                                                                                                        const auto usernameMatch = input.match(/^[a-zA-Z0-9_]+$/);
                                                                                                        if (usernameMatch) {
                                                                                                            return "https://x.com/" + input;
                                                                                                        }
                                                                                                        std::cout << "Could not normalize Twitter/X input: " + input << std::endl;
                                                                                                        return input; // Return original if cannot parse;
                                                                                                        };

                                                                                                        const auto normalizedLinks = {;
                                                                                                            website: normalizeUrl(data.links.website || "", "https://"),
                                                                                                            twitter: normalizeTwitter(data.links.twitter || ""),
                                                                                                            telegram: normalizeUrl(
                                                                                                            data.links.telegram || "",
                                                                                                            "https://t.me/",
                                                                                                            "t.me",
                                                                                                            ),
                                                                                                            discord: normalizeUrl(
                                                                                                            data.links.discord || "",
                                                                                                            "https://discord.gg/",
                                                                                                            "discord.gg",
                                                                                                            ),
                                                                                                            farcaster: normalizeUrl(
                                                                                                            data.links.farcaster || "",
                                                                                                            "https://warpcast.com/",
                                                                                                            "warpcast.com",
                                                                                                            ),
                                                                                                            };
                                                                                                            // --- End Link Normalization ---

                                                                                                            try {
                                                                                                                const auto authToken = localStorage.getItem("authToken");
                                                                                                                const std::unordered_map<std::string, std::string> headers = {;
                                                                                                                    "Content-Type": "application/json",
                                                                                                                    };

                                                                                                                    if (authToken) {
                                                                                                                        "headers[\"Authorization\"] = " + "Bearer " + std::to_string(/* JSON.parse */ authToken);
                                                                                                                    }

                                                                                                                    // Create request payload with development override if needed
                                                                                                                    // Use normalized links
                                                                                                                    const std::unordered_map<std::string, std::any> payload = {;
                                                                                                                        website: normalizedLinks.website,
                                                                                                                        twitter: normalizedLinks.twitter,
                                                                                                                        telegram: normalizedLinks.telegram,
                                                                                                                        discord: normalizedLinks.discord,
                                                                                                                        farcaster: normalizedLinks.farcaster,
                                                                                                                        };

                                                                                                                        // Try admin endpoint first, fall back to owner endpoint if not admin
                                                                                                                        auto response = fetch(;
                                                                                                                        env.apiUrl + "/api/admin/tokens/" + mint + "/social"
                                                                                                                        {
                                                                                                                            method: "POST",
                                                                                                                            headers,
                                                                                                                            body: /* JSON.stringify */ std::string(payload),
                                                                                                                            credentials: "include",
                                                                                                                            },
                                                                                                                            );

                                                                                                                            if (response.status == 403) {
                                                                                                                                // If not admin, try owner endpoint
                                                                                                                                response = fetch(;
                                                                                                                                env.apiUrl + "/api/owner/tokens/" + mint + "/social"
                                                                                                                                {
                                                                                                                                    method: "POST",
                                                                                                                                    headers,
                                                                                                                                    body: /* JSON.stringify */ std::string(payload),
                                                                                                                                    credentials: "include",
                                                                                                                                    },
                                                                                                                                    );
                                                                                                                                }

                                                                                                                                if (!response.ok) {
                                                                                                                                    auto errorMessage = "Failed to update token";
                                                                                                                                    try {
                                                                                                                                        const auto errorData = (response.json()) as { error?: string };
                                                                                                                                        errorMessage = errorData.error || errorMessage;
                                                                                                                                        } catch (e) {
                                                                                                                                            std::cerr << "Error parsing error response:" << e << std::endl;
                                                                                                                                        }
                                                                                                                                        throw std::runtime_error(errorMessage);
                                                                                                                                    }

                                                                                                                                    toast.success("Token information updated successfully");
                                                                                                                                    setOriginalData({
                                                                                                                                        website: normalizedLinks.website || "",
                                                                                                                                        twitter: normalizedLinks.twitter || "",
                                                                                                                                        telegram: normalizedLinks.telegram || "",
                                                                                                                                        discord: normalizedLinks.discord || "",
                                                                                                                                        farcaster: normalizedLinks.farcaster || "",
                                                                                                                                        });
                                                                                                                                        queryClient.invalidateQueries({ queryKey: ["token", mint] });
                                                                                                                                        } catch (error) {
                                                                                                                                            std::cerr << "Error updating token:" << error << std::endl;
                                                                                                                                            toast.error(;
                                                                                                                                            true /* instanceof check */ ? error.message : "Failed to update token",
                                                                                                                                            );
                                                                                                                                            } finally {
                                                                                                                                                setIsSaving(false);
                                                                                                                                            }
                                                                                                                                            };

                                                                                                                                            // --- Mutations for Moderator Actions ---

                                                                                                                                            // Mutation for toggling hidden status
                                                                                                                                            const auto toggleHiddenMutation = useMutation({;
                                                                                                                                                mutationFn: async () => {
                                                                                                                                                    if (!mint) throw new Error("Mint address not found");
                                                                                                                                                    return "fetcher(" + "/api/admin/tokens/" + mint + "/hidden";
                                                                                                                                                        hidden: !tokenStatus.hidden,
                                                                                                                                                        });
                                                                                                                                                        },
                                                                                                                                                        onSuccess: () => {
                                                                                                                                                            toast.success(;
                                                                                                                                                            "Token " + std::to_string(tokenStatus.hidden ? "unhidden" : "hidden") + " successfully"
                                                                                                                                                            );
                                                                                                                                                            // Manually update state for immediate UI feedback
                                                                                                                                                            setTokenStatus((prev) => ({ ...prev, hidden: !prev.hidden }));
                                                                                                                                                            queryClient.invalidateQueries({ queryKey: ["token", mint] }); // Refetch token data in the background
                                                                                                                                                            },
                                                                                                                                                            onError: (error) => {
                                                                                                                                                                toast.error(;
                                                                                                                                                                "Failed to update hidden status: " + std::to_string(true /* instanceof check */ ? error.message : "Unknown error")
                                                                                                                                                                );
                                                                                                                                                                },
                                                                                                                                                                });

                                                                                                                                                                // Mutation for toggling featured status
                                                                                                                                                                const auto toggleFeaturedMutation = useMutation({;
                                                                                                                                                                    mutationFn: async () => {
                                                                                                                                                                        if (!mint) throw new Error("Mint address not found");
                                                                                                                                                                        return "fetcher(" + "/api/admin/tokens/" + mint + "/featured";
                                                                                                                                                                            featured: !tokenStatus.featured,
                                                                                                                                                                            });
                                                                                                                                                                            },
                                                                                                                                                                            onSuccess: () => {
                                                                                                                                                                                toast.success(;
                                                                                                                                                                                "Token " + std::to_string(tokenStatus.featured ? "removed from" : "added to") + " featured tokens"
                                                                                                                                                                                );
                                                                                                                                                                                // Manually update state for immediate UI feedback
                                                                                                                                                                                setTokenStatus((prev) => ({ ...prev, featured: !prev.featured }));
                                                                                                                                                                                queryClient.invalidateQueries({ queryKey: ["token", mint] }); // Refetch token data in the background
                                                                                                                                                                                },
                                                                                                                                                                                onError: (error) => {
                                                                                                                                                                                    toast.error(;
                                                                                                                                                                                    "Failed to update featured status: " + std::to_string(true /* instanceof check */ ? error.message : "Unknown error")
                                                                                                                                                                                    );
                                                                                                                                                                                    },
                                                                                                                                                                                    });

                                                                                                                                                                                    // Mutation for toggling verified status
                                                                                                                                                                                    const auto toggleVerifiedMutation = useMutation({;
                                                                                                                                                                                        mutationFn: async () => {
                                                                                                                                                                                            if (!mint) throw new Error("Mint address not found");
                                                                                                                                                                                            return "fetcher(" + "/api/admin/tokens/" + mint + "/verified";
                                                                                                                                                                                                verified: !tokenStatus.verified,
                                                                                                                                                                                                });
                                                                                                                                                                                                },
                                                                                                                                                                                                onSuccess: () => {
                                                                                                                                                                                                    toast.success(;
                                                                                                                                                                                                    "Token " + std::to_string(tokenStatus.verified ? "unverified" : "verified") + " successfully"
                                                                                                                                                                                                    );
                                                                                                                                                                                                    // Manually update state for immediate UI feedback
                                                                                                                                                                                                    setTokenStatus((prev) => ({ ...prev, verified: !prev.verified }));
                                                                                                                                                                                                    queryClient.invalidateQueries({ queryKey: ["token", mint] }); // Refetch token data in the background
                                                                                                                                                                                                    },
                                                                                                                                                                                                    onError: (error) => {
                                                                                                                                                                                                        toast.error(;
                                                                                                                                                                                                        "Failed to update verified status: " + std::to_string(true /* instanceof check */ ? error.message : "Unknown error")
                                                                                                                                                                                                        );
                                                                                                                                                                                                        },
                                                                                                                                                                                                        });

                                                                                                                                                                                                        // --- End Mutations ---

                                                                                                                                                                                                        // ---- Moderator Check using hardcoded list ----
                                                                                                                                                                                                        const auto { publicKey } = useWallet();
                                                                                                                                                                                                        const auto isModerator = publicKey;
                                                                                                                                                                                                        ? (std::find(adminAddresses.begin(), adminAddresses.end(), std::to_string(publicKey) != adminAddresses.end()));
                                                                                                                                                                                                        : false;

                                                                                                                                                                                                        // Check if user is the token owner
                                                                                                                                                                                                        const auto [isTokenOwner, setIsTokenOwner] = useState(false);

                                                                                                                                                                                                        // Update the ownership check in useEffect
                                                                                                                                                                                                        useEffect(() => {
                                                                                                                                                                                                            if (!mint) return;
                                                                                                                                                                                                            const auto fetchTokenData = async () => {;
                                                                                                                                                                                                                try {
                                                                                                                                                                                                                    const auto response = "fetch(" + env.apiUrl + "/api/token/" + mint;
                                                                                                                                                                                                                    const auto data = (response.json()) as { creator: string };
                                                                                                                                                                                                                    setIsTokenOwner(data.creator == std::to_string(publicKey));
                                                                                                                                                                                                                    // ... rest of the fetch logic
                                                                                                                                                                                                                    } catch (error) {
                                                                                                                                                                                                                        std::cerr << "Error fetching token data:" << error << std::endl;
                                                                                                                                                                                                                    }
                                                                                                                                                                                                                    };
                                                                                                                                                                                                                    fetchTokenData();
                                                                                                                                                                                                                    }, [mint, publicKey]);

                                                                                                                                                                                                                    const auto queryClient = useQueryClient();

                                                                                                                                                                                                                    // Add these new functions for audio handling
                                                                                                                                                                                                                    const auto handleAudioSelect = [&](e: React.ChangeEvent<HTMLInputElement>) {;
                                                                                                                                                                                                                        const auto file = e.target.files.[0];
                                                                                                                                                                                                                        if (!file) return;

                                                                                                                                                                                                                        // Validate file type
                                                                                                                                                                                                                        if (!file.type.startsWith("audio/")) {
                                                                                                                                                                                                                            toast.error("Please select an audio file");
                                                                                                                                                                                                                            return;
                                                                                                                                                                                                                        }

                                                                                                                                                                                                                        // Check file size (limit to 10MB)
                                                                                                                                                                                                                        if (file.size > 10 * 1024 * 1024) {
                                                                                                                                                                                                                            toast.error(;
                                                                                                                                                                                                                            "File is too large. Please select an audio file less than 10MB.",
                                                                                                                                                                                                                            );
                                                                                                                                                                                                                            return;
                                                                                                                                                                                                                        }

                                                                                                                                                                                                                        setSelectedAudioFile(file);
                                                                                                                                                                                                                        };

                                                                                                                                                                                                                        const auto handleAudioUpload = async () => {;
                                                                                                                                                                                                                            if (!mint || !selectedAudioFile) return;

                                                                                                                                                                                                                            setIsUploadingAudio(true);
                                                                                                                                                                                                                            try {
                                                                                                                                                                                                                                const auto formData = new FormData();
                                                                                                                                                                                                                                formData.append("audio", selectedAudioFile);

                                                                                                                                                                                                                                const auto authToken = localStorage.getItem("authToken");
                                                                                                                                                                                                                                const std::unordered_map<std::string, std::string> headers = {;
                                                                                                                                                                                                                                    "Authorization: " + "Bearer " + std::to_string(/* JSON.parse */ authToken || "{}")
                                                                                                                                                                                                                                    };

                                                                                                                                                                                                                                    const auto response = fetch(;
                                                                                                                                                                                                                                    env.apiUrl + "/api/token/" + mint + "/audio-context"
                                                                                                                                                                                                                                    {
                                                                                                                                                                                                                                        method: "POST",
                                                                                                                                                                                                                                        headers,
                                                                                                                                                                                                                                        credentials: "include",
                                                                                                                                                                                                                                        body: formData,
                                                                                                                                                                                                                                        },
                                                                                                                                                                                                                                        );

                                                                                                                                                                                                                                        if (!response.ok) {
                                                                                                                                                                                                                                            throw std::runtime_error("Failed to upload audio context");
                                                                                                                                                                                                                                        }

                                                                                                                                                                                                                                        toast.success("Audio context uploaded successfully");
                                                                                                                                                                                                                                        setSelectedAudioFile(nullptr);
                                                                                                                                                                                                                                        setAudioTimestamp(Date.now());
                                                                                                                                                                                                                                        if (audioInputRef.current) {
                                                                                                                                                                                                                                            audioInputRef.current.value = "";
                                                                                                                                                                                                                                        }
                                                                                                                                                                                                                                        } catch (error) {
                                                                                                                                                                                                                                            std::cerr << "Error uploading audio:" << error << std::endl;
                                                                                                                                                                                                                                            toast.error("Failed to upload audio context");
                                                                                                                                                                                                                                            } finally {
                                                                                                                                                                                                                                                setIsUploadingAudio(false);
                                                                                                                                                                                                                                            }
                                                                                                                                                                                                                                            };

                                                                                                                                                                                                                                            if (isLoading) {
                                                                                                                                                                                                                                                return <Loader />;
                                                                                                                                                                                                                                            }

                                                                                                                                                                                                                                            // If not a moderator or owner, don't render the admin controls
                                                                                                                                                                                                                                            if (!isModerator && !isTokenOwner) {
                                                                                                                                                                                                                                                return (;
                                                                                                                                                                                                                                                <div className="p-4 text-center text-autofun-text-secondary">;
                                                                                                                                                                                                                                                Admin controls are only available to moderators and the token creator.;
                                                                                                                                                                                                                                                </div>;
                                                                                                                                                                                                                                                );
                                                                                                                                                                                                                                            }

                                                                                                                                                                                                                                            return (;
                                                                                                                                                                                                                                            <form onSubmit={handleSubmit(onSubmit)} className="flex flex-col gap-4">;
                                                                                                                                                                                                                                            <div className="flex flex-col gap-0.5">;
                                                                                                                                                                                                                                        {/* Website Field */}
                                                                                                                                                                                                                                        <Controller;
                                                                                                                                                                                                                                    control={control}
                                                                                                                                                                                                                                    name="links.website";
                                                                                                                                                                                                                                    render={({ field }) => (;
                                                                                                                                                                                                                                    <FormInput;
                                                                                                                                                                                                                                    type="text";
                                                                                                                                                                                                                                {...field}
                                                                                                                                                                                                                                isOptional;
                                                                                                                                                                                                                            inputTag={<Icons.Website />}
                                                                                                                                                                                                                            placeholder="Website";
                                                                                                                                                                                                                        rightIndicator={<CopyButton text={field.value || ""} />}
                                                                                                                                                                                                                        />;
                                                                                                                                                                                                                    )}
                                                                                                                                                                                                                    />;

                                                                                                                                                                                                                {/* Twitter Field */}
                                                                                                                                                                                                                <Controller;
                                                                                                                                                                                                            control={control}
                                                                                                                                                                                                            name="links.twitter";
                                                                                                                                                                                                            render={({ field, fieldState: { error } }) => (
                                                                                                                                                                                                            <div className="flex flex-col gap-1">;
                                                                                                                                                                                                            <FormInput;
                                                                                                                                                                                                            type="text";
                                                                                                                                                                                                        {...field}
                                                                                                                                                                                                        isOptional;
                                                                                                                                                                                                    inputTag={<Icons.Twitter />}
                                                                                                                                                                                                    placeholder="X (Twitter)";
                                                                                                                                                                                                rightIndicator={<CopyButton text={field.value || ""} />}
                                                                                                                                                                                                />;
                                                                                                                                                                                                {error && (;
                                                                                                                                                                                                <span className="text-red-500 text-sm">{error.message}</span>;
                                                                                                                                                                                            )}
                                                                                                                                                                                            </div>;
                                                                                                                                                                                        )}
                                                                                                                                                                                        />;

                                                                                                                                                                                    {/* Telegram Field */}
                                                                                                                                                                                    <Controller;
                                                                                                                                                                                control={control}
                                                                                                                                                                                name="links.telegram";
                                                                                                                                                                                rules={{
                                                                                                                                                                                    validate: (value: string) =>
                                                                                                                                                                                    !value || isFromDomain(value, "t.me") || "Invalid Telegram URL",
                                                                                                                                                                                }}
                                                                                                                                                                                render={({ field, fieldState: { error } }) => (
                                                                                                                                                                                <div className="flex flex-col gap-1">;
                                                                                                                                                                                <FormInput;
                                                                                                                                                                                type="text";
                                                                                                                                                                            {...field}
                                                                                                                                                                            isOptional;
                                                                                                                                                                        inputTag={<Icons.Telegram />}
                                                                                                                                                                        placeholder="Telegram";
                                                                                                                                                                    rightIndicator={<CopyButton text={field.value || ""} />}
                                                                                                                                                                    />;
                                                                                                                                                                    {error && (;
                                                                                                                                                                    <span className="text-red-500 text-sm">{error.message}</span>;
                                                                                                                                                                )}
                                                                                                                                                                </div>;
                                                                                                                                                            )}
                                                                                                                                                            />;

                                                                                                                                                        {/* Discord Field */}
                                                                                                                                                        <Controller;
                                                                                                                                                    control={control}
                                                                                                                                                    name="links.discord";
                                                                                                                                                    rules={{
                                                                                                                                                        validate: (value: string) =>
                                                                                                                                                        !value ||;
                                                                                                                                                        isFromDomain(value, "discord.gg") ||;
                                                                                                                                                        "Invalid Discord URL",
                                                                                                                                                    }}
                                                                                                                                                    render={({ field, fieldState: { error } }) => (
                                                                                                                                                    <div className="flex flex-col gap-1">;
                                                                                                                                                    <FormInput;
                                                                                                                                                    type="text";
                                                                                                                                                {...field}
                                                                                                                                                isOptional;
                                                                                                                                            inputTag={<Icons.Discord />}
                                                                                                                                            placeholder="Discord";
                                                                                                                                        rightIndicator={<CopyButton text={field.value || ""} />}
                                                                                                                                        />;
                                                                                                                                        {error && (;
                                                                                                                                        <span className="text-red-500 text-sm">{error.message}</span>;
                                                                                                                                    )}
                                                                                                                                    </div>;
                                                                                                                                )}
                                                                                                                                />;

                                                                                                                            {/* Farcaster Field */}
                                                                                                                            <Controller;
                                                                                                                        control={control}
                                                                                                                        name="links.farcaster";
                                                                                                                        rules={{
                                                                                                                            validate: (value: string) =>
                                                                                                                            !value ||;
                                                                                                                            isFromDomain(value, "warpcast.com") ||;
                                                                                                                            "Invalid Farcaster URL",
                                                                                                                        }}
                                                                                                                        render={({ field, fieldState: { error } }) => (
                                                                                                                        <div className="flex flex-col gap-1">;
                                                                                                                        <FormInput;
                                                                                                                        type="text";
                                                                                                                    {...field}
                                                                                                                    isOptional;
                                                                                                                inputTag={<Icons.Farcaster />}
                                                                                                                placeholder="Farcaster";
                                                                                                            rightIndicator={<CopyButton text={field.value || ""} />}
                                                                                                            />;
                                                                                                            {error && (;
                                                                                                            <span className="text-red-500 text-sm">{error.message}</span>;
                                                                                                        )}
                                                                                                        </div>;
                                                                                                    )}
                                                                                                    />;
                                                                                                    </div>;

                                                                                                {/* Update Social Links Button */}
                                                                                                <button;
                                                                                                type="submit";
                                                                                            disabled={isSaving || !hasChanges()}
                                                                                            "className={" + "ml-auto cursor-pointer text-white bg-transparent gap-x-3 border-2 hover:bg-autofun-background-action-highlight border-autofun-background-action-highlight flex px-8 py-1 mt-2 flex-row w-fit items-center justify-items-center " + std::to_string()
                                                                                                isSaving || !hasChanges() ? "opacity-50 cursor-not-allowed" : ""
                                                                                            "}";
                                                                                            >;
                                                                                        {isSaving ? "Saving..."  = "Save Social Links"}
                                                                                        </button>;

                                                                                    {/* Moderator Actions Section - Only show for moderators */}
                                                                                    {isModerator && (;
                                                                                    <div className="mt-6 pt-4 border-t border-autofun-border">;
                                                                                    <h4 className="text-md font-semibold mb-3 text-autofun-text-secondary">;
                                                                                    Moderator Actions;
                                                                                    </h4>;
                                                                                    <div className="flex flex-wrap justify-start gap-2">;
                                                                                    <button;
                                                                                    type="button";
                                                                                    "className={" + "px-4 py-2 text-sm " + std::to_string();
                                                                                        tokenStatus.featured;
                                                                                        ? "bg-red-700 text-red-100 hover:bg-red-600"
                                                                                        : "bg-blue-700 text-blue-100 hover:bg-blue-600"
                                                                                    "} disabled:opacity-50 disabled:cursor-not-allowed"
                                                                                onClick={() => toggleFeaturedMutation.mutate()}
                                                                            disabled={toggleFeaturedMutation.isPending}
                                                                            >;
                                                                            {toggleFeaturedMutation.isPending;
                                                                            ? "Processing...";
                                                                            : tokenStatus.featured
                                                                            ? "Remove Featured";
                                                                        : "Make Featured"}
                                                                        </button>;

                                                                        <button;
                                                                        type="button";
                                                                        "className={" + "px-4 py-2 text-sm " + std::to_string();
                                                                            tokenStatus.verified;
                                                                            ? "bg-red-700 text-red-100 hover:bg-red-600"
                                                                            : "bg-green-700 text-green-100 hover:bg-green-600"
                                                                        "} disabled:opacity-50 disabled:cursor-not-allowed"
                                                                    onClick={() => toggleVerifiedMutation.mutate()}
                                                                disabled={toggleVerifiedMutation.isPending}
                                                                >;
                                                                {toggleVerifiedMutation.isPending;
                                                                ? "Processing...";
                                                                : tokenStatus.verified
                                                                ? "Remove Verified";
                                                            : "Make Verified"}
                                                            </button>;

                                                            <button;
                                                            type="button";
                                                            "className={" + "px-4 py-2 text-sm " + std::to_string();
                                                                tokenStatus.hidden;
                                                                ? "bg-yellow-700 text-yellow-100 hover:bg-yellow-600"
                                                                : "bg-gray-700 text-gray-100 hover:bg-gray-600"
                                                            "} disabled:opacity-50 disabled:cursor-not-allowed"
                                                        onClick={() => toggleHiddenMutation.mutate()}
                                                    disabled={toggleHiddenMutation.isPending}
                                                    >;
                                                    {toggleHiddenMutation.isPending;
                                                    ? "Processing...";
                                                    : tokenStatus.hidden
                                                    ? "Unhide Token";
                                                : "Hide Token"}
                                                </button>;
                                                </div>;
                                                <div className="flex justify-start mt-4">;
                                                <Link;
                                            "to={" + "/admin/tokens/" + mint;
                                            className="text-sm px-3 py-1 bg-autofun-background-action-primary hover:bg-autofun-background-action-highlight text-autofun-text-primary"
                                            >;
                                            Go to Full Admin View;
                                            </Link>;
                                            </div>;
                                            </div>;
                                        )}

                                    {/* Audio Context Section - Only show for token owner */}
                                    {isTokenOwner && (;
                                    <div className="mt-6 pt-4 border-t border-autofun-border">;
                                    <h4 className="text-md font-semibold mb-3 text-autofun-text-secondary">;
                                    Audio Context;
                                    </h4>;
                                    <div className="flex flex-col gap-4 max-w-2xl">;
                                    <p className="text-sm text-autofun-text-secondary">;
                                    Upload an audio file to use for music generation. This;
                                    will be used instead of the default audio context.;
                                    </p>;
                                    <div className="flex flex-col gap-2">;
                                    <input;
                                    type="file";
                                    accept="audio/*";
                                onChange={handleAudioSelect}
                            ref={audioInputRef}
                            className="file:border file:border-[#03FF24] file:mr-4 file:py-2 file:px-4 file:border-solid file:text-sm file:bg-transparent file:text-[#03FF24] file:cursor-pointer file:hover:bg-[#03FF24]/10 text-[#03FF24]"
                            />;
                            {selectedAudioFile && (;
                            <span className="text-sm text-autofun-text-secondary">;
                        {selectedAudioFile.name}
                        </span>;
                    )}
                    {existingAudioUrl && !selectedAudioFile && (;
                    <div className="flex flex-col gap-2">;
                    <div className="flex flex-col gap-2 mb-4">;
                    <AudioPlayer;
                "src={" + existingAudioUrl + "?t=" + audioTimestamp;
                />;
                </div>;
                <div className="flex flex-col sm:flex-row items-start sm:items-center gap-2">
                <div className="flex relative w-full">;
                <div className="bg-autofun-background-input flex justify-between py-2 pl-1 pr-3 min-w-0 w-full relative">;
                <span className="mx-auto w-0 flex-1 min-w-0 block text-sm text-autofun-text-secondary truncate pr-6 pl-1">;
            {existingAudioUrl.replace(/^https?:\/\//, "")} /* TODO: Convert object literal */
            </span>;
            </div>;
            <div className="absolute right-3 top-1/2 transform -translate-y-1/2 bg-autofun-background-input">;
            <CopyButton text={existingAudioUrl} />;
            </div>;
            </div>;
            <button;
            type="button";
        onClick={() => audioInputRef.current.click()}
        className="text-sm text-[#03FF24] hover:text-[#03FF24]/80 transition-colors ml-2"
        >;
        Replace;
        </button>;
        </div>;
        </div>;
        )}
        {selectedAudioFile && (;
        <button;
        type="button";
        onClick={handleAudioUpload}
        disabled={isUploadingAudio}
        className="px-4 py-2 text-sm bg-[#03FF24] text-black hover:bg-[#03FF24]/80 transition-colors disabled:opacity-50 w-fit"
        >;
        {isUploadingAudio ? "Uploading..."  = "Upload"}
        </button>;
        )}
        </div>;
        </div>;
        </div>;
        )}
        </form>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
