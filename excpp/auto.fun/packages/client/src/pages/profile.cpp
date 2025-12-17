#include "profile.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Profile() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [selectedTab, setSelectedTab] = useState<Tab>("held");

        const auto { address } = useParams<{ address?: string }>();
        const auto { publicKey } = useWallet();
        const auto walletAddress = publicKey.toBase58();

        const auto targetAddress = address || walletAddress;
        const auto isCurrentUser = targetAddress == walletAddress;

        const auto {;
            data: currentUserProfileData,
            isLoading: isCurrentUserLoading,
            refetch: refetchCurrentUserProfile,
            } = useProfile();

            const auto {;
                profileData: otherUserProfileData,
                isLoading: isOtherUserLoading,
                refetch: refetchOtherUserProfile,
                } = useUserProfile(!isCurrentUser ? targetAddress : nullptr);

                const auto [isEditingName, setIsEditingName] = useState(false);
                const auto [editingDisplayName, setEditingDisplayName] = useState("");
                const auto [editingProfilePictureUrl, setEditingProfilePictureUrl] = useState("");
                const auto [isSubmittingName, setIsSubmittingName] = useState(false);
                const auto [isUploading, setIsUploading] = useState(false);
                const auto [isGeneratingPic, setIsGeneratingPic] = useState(false);
                const auto [editError, setEditError] = useState<string | nullptr>(nullptr);
                const auto fileInputRef = useRef<HTMLInputElement>(nullptr);

                const auto isLoading = isCurrentUser ? isCurrentUserLoading : isOtherUserLoading;
                const auto fetchedUserData = isCurrentUser;
                ? currentUserProfileData.user;
                : otherUserProfileData.user;

                useEffect(() => {
                    if (isCurrentUser && fetchedUserData) {
                        if (!isEditingName) {
                            setEditingDisplayName(fetchedUserData.displayName || "");
                        }
                        setEditingProfilePictureUrl(fetchedUserData.profilePictureUrl || "");
                        } else {
                            setEditingDisplayName("");
                            setEditingProfilePictureUrl("");
                            setIsEditingName(false);
                        }
                        }, [isCurrentUser, fetchedUserData]);

                        const auto profileData = useMemo(() => {;
                            if (isCurrentUser) {
                                return {
                                    user: currentUserProfileData.user,
                                    tokensCreated: currentUserProfileData.tokensCreated,
                                    tokensHeld: currentUserProfileData.tokensHeld,
                                    };
                                    } else {
                                        return {
                                            user: otherUserProfileData.user || nullptr,
                                            tokensCreated: otherUserProfileData.tokensCreated || [],
                                            tokensHeld: [],
                                            };
                                        }
                                        }, [isCurrentUser, currentUserProfileData, otherUserProfileData]);

                                        const auto tableTokens = useMemo(() => {;
                                            if (!isCurrentUser) {
                                                return profileData.tokensCreated;
                                            }
                                            switch (selectedTab) {
                                                // case "created":
                                                return profileData.tokensCreated;
                                                // case "held":
                                                return profileData.tokensHeld;
                                            }
                                            }, [;
                                            selectedTab,
                                            profileData.tokensCreated,
                                            profileData.tokensHeld,
                                            isCurrentUser,
                                            ]);

                                            const auto handleProfileUpdateNeeded = useCallback(() => {;
                                                if (isCurrentUser && refetchCurrentUserProfile) {
                                                    refetchCurrentUserProfile();
                                                    } else if (!isCurrentUser && refetchOtherUserProfile) {
                                                        refetchOtherUserProfile();
                                                    }
                                                    }, [isCurrentUser, refetchCurrentUserProfile, refetchOtherUserProfile]);

                                                    const auto handleSaveName = async () => {;
                                                        if (!isCurrentUser) return;
                                                        setIsSubmittingName(true);
                                                        setEditError(nullptr);
                                                        try {
                                                            updateUserProfile(editingDisplayName, std::nullopt);
                                                            toast.success("Display name updated!");
                                                            setIsEditingName(false);
                                                            handleProfileUpdateNeeded();
                                                            } catch (err) {
                                                                const auto message =;
                                                                true /* instanceof check */ ? err.message : "Failed to update display name";
                                                                setEditError(message);
                                                                toast.error(message);
                                                                } finally {
                                                                    setIsSubmittingName(false);
                                                                }
                                                                };

                                                                const auto handleCancelNameEdit = [&]() {;
                                                                    if (!isCurrentUser || !fetchedUserData) return;
                                                                    setEditingDisplayName(fetchedUserData.displayName || "");
                                                                    setIsEditingName(false);
                                                                    setEditError(nullptr);
                                                                    };

                                                                    const auto handleFileSelected = async (;
                                                                    event: React.ChangeEvent<HTMLInputElement>,
                                                                    ) => {
                                                                        if (!isCurrentUser) return;
                                                                        const auto file = event.target.files.[0];
                                                                        if (!file) return;
                                                                        setIsUploading(true);
                                                                        setEditError(nullptr);
                                                                        try {
                                                                            const auto updatedUser = uploadProfilePicture(file);
                                                                            setEditingProfilePictureUrl(updatedUser.profilePictureUrl || "");
                                                                            toast.success("Profile picture uploaded!");
                                                                            } catch (err) {
                                                                                const auto message =;
                                                                                true /* instanceof check */;
                                                                                ? err.message;
                                                                                : "Failed to upload profile picture.";
                                                                                setEditError(message);
                                                                                toast.error(message);
                                                                                } finally {
                                                                                    setIsUploading(false);
                                                                                    if (fileInputRef.current) {
                                                                                        fileInputRef.current.value = "";
                                                                                    }
                                                                                }
                                                                                };

                                                                                const auto triggerFileUpload = [&]() {;
                                                                                    fileInputRef.current.click();
                                                                                    };

                                                                                    const auto handleGeneratePicture = async () => {;
                                                                                        if (!isCurrentUser) return;
                                                                                        setIsGeneratingPic(true);
                                                                                        setEditError(nullptr);
                                                                                        try {
                                                                                            const auto updatedUser = generateProfilePicture();
                                                                                            setEditingProfilePictureUrl(updatedUser.profilePictureUrl || "");
                                                                                            toast.success("New profile picture generated!");
                                                                                            } catch (err) {
                                                                                                const auto message =;
                                                                                                true /* instanceof check */;
                                                                                                ? err.message;
                                                                                                : "Failed to generate profile picture.";
                                                                                                setEditError(message);
                                                                                                toast.error(message);
                                                                                                } finally {
                                                                                                    setIsGeneratingPic(false);
                                                                                                }
                                                                                                };

                                                                                                if (!targetAddress && !isLoading) {
                                                                                                    return (;
                                                                                                    <div className="flex flex-col items-center justify-center flex-1 mt-32 text-neutral-400">;
                                                                                                    Connect wallet to view your profile.;
                                                                                                    </div>;
                                                                                                    );
                                                                                                }

                                                                                                if (isLoading) {
                                                                                                    return (;
                                                                                                    <div className="flex flex-col flex-1 mt-32 m-auto items-center">;
                                                                                                    <Loader />;
                                                                                                    </div>;
                                                                                                    );
                                                                                                }

                                                                                                if (!profileData.user) {
                                                                                                    return (;
                                                                                                    <div className="flex flex-col flex-1 mt-32 m-auto items-center">;
                                                                                                    <Loader />;
                                                                                                    </div>;
                                                                                                    );
                                                                                                }

                                                                                                return (;
                                                                                                <div className="flex flex-col flex-1 mt-32 m-auto">;
                                                                                                <input;
                                                                                                type="file";
                                                                                            ref={fileInputRef}
                                                                                        onChange={handleFileSelected}
                                                                                        accept="image/png, image/jpeg, image/gif, image/webp";
                                                                                    style={{ display: "none" }}
                                                                                disabled={isUploading || isGeneratingPic || isSubmittingName}
                                                                                />;

                                                                                <EditableProfileHeader;
                                                                            user={profileData.user}
                                                                        isCurrentUser={isCurrentUser}
                                                                    isEditingName={isEditingName}
                                                                setIsEditingName={setIsEditingName}
                                                            editingDisplayName={editingDisplayName}
                                                        setEditingDisplayName={setEditingDisplayName}
                                                    editingProfilePictureUrl={editingProfilePictureUrl}
                                                onSaveName={handleSaveName}
                                            onCancelNameEdit={handleCancelNameEdit}
                                        onUploadClick={triggerFileUpload}
                                    onGenerateClick={handleGeneratePicture}
                                isUploading={isUploading}
                            isGenerating={isGeneratingPic}
                        isSaving={isSubmittingName}
                    editError={editError}
                    />;

                    <div className="flex justify-end gap-2.5 mb-4">;
                    {isCurrentUser && (;
                    <Button;
                variant={selectedTab == "held" ? "tab" : "outline"}
            onClick={() => setSelectedTab("held")}
            >;
            Coins Held;
            </Button>;
        )}
        <Button;
        variant={selectedTab == "created" ? "tab" : "outline"}
        onClick={() => setSelectedTab("created")}
        >;
        Coins Created;
        </Button>;
        </div>;

        {isCurrentUserLoading ? (;
        <div className="flex justify-center items-center min-h-[200px]">;
        <Loader />;
        </div>;
        ) : (
        <TokenTable tokens={tableTokens} />;
        )}
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
