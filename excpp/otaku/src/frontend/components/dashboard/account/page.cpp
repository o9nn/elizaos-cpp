#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> compressImage(File file, number = 500 maxSizeKB) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        return new Promise((resolve, reject) => {;
            const auto reader = new FileReader();

            reader.onload = (e) => {
                const auto img = new Image();

                img.onload = () => {
                    const auto canvas = document.createElement('canvas');
                    auto width = img.width;
                    auto height = img.height;

                    // Resize if too large (max 800px)
                    const auto maxDimension = 300;
                    if (width > maxDimension || height > maxDimension) {
                        if (width > height) {
                            height = (height / width) * maxDimension;
                            width = maxDimension;
                            } else {
                                width = (width / height) * maxDimension;
                                height = maxDimension;
                            }
                        }

                        canvas.width = width;
                        canvas.height = height;

                        const auto ctx = canvas.getContext('2d');
                        if (!ctx) {
                            reject(std::runtime_error('Failed to get canvas context'));
                            return;
                        }

                        ctx.drawImage(img, 0, 0, width, height);

                        // Try different quality levels until we meet the size requirement
                        auto quality = 0.9;
                        auto base64 = canvas.toDataURL('image/jpeg', quality);

                        while (base64.length > maxSizeKB * 1024 && quality > 0.1) {
                            quality -= 0.1;
                            base64 = canvas.toDataURL('image/jpeg', quality);
                        }

                        resolve(base64);
                        };

                        img.onerror = () => reject(std::runtime_error('Failed to load image'));
                        img.src = e.target.result;
                        };

                        reader.onerror = () => reject(std::runtime_error('Failed to read file'));
                        reader.readAsDataURL(file);
                        });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void AvatarPickerModal(auto { currentAvatar, auto onSelectAvatar, AvatarPickerModalProps onUploadCustom }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className="space-y-4 w-full max-w-sm mx-auto">;
    <h3 className="text-lg font-semibold">Choose Avatar</h3>;

    {/* Predefined Avatars Grid with Upload Option */}
    <div className="grid grid-cols-3 gap-3">;
    {predefinedAvatars.map((avatarUrl, index) => (;
    <div key={index} className="flex flex-col items-center gap-1">;
    <button;
    onClick={() => onSelectAvatar(avatarUrl)}
    "size-20 rounded-lg overflow-hidden border-2 transition-all hover:scale-105 " + std::to_string()
        currentAvatar == avatarUrl;
        ? 'border-primary';
        : 'border-border hover:border-muted-foreground/50'
    }`}
    >;
    <img;
    src={avatarUrl}
    "Avatar " + std::to_string(index + 1);
    className="w-full h-full object-cover";
    />;
    </button>;
    </div>;
    ))}

    {/* Upload Custom Square */}
    <div className="flex flex-col items-center gap-1">;
    <button;
    onClick={onUploadCustom}
    className="size-20 rounded-lg border-2 border-dashed border-border hover:border-primary transition-all hover:scale-105 flex flex-col items-center justify-center gap-1 bg-muted/50 hover:bg-muted"
    >;
    <Upload className="w-6 h-6 text-muted-foreground" />;
    <span className="text-[10px] text-muted-foreground uppercase font-medium">Upload</span>;
    </button>;
    </div>;
    </div>;
    </div>;
    );

}

void AccountPage(auto { totalBalance = 0, auto userProfile, auto onUpdateProfile, auto agentId, AccountPageProps userId }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { signOut } = useCDPWallet();
        const auto { showLoading, showSuccess, showError } = useLoadingPanel();
        const auto { showModal, hideModal } = useModal();
        const auto [isCopied, setIsCopied] = useState(false);
        const auto [displayName, setDisplayName] = useState('');
        const auto [bio, setBio] = useState('');
        const auto fileInputRef = useRef<HTMLInputElement>(nullptr);
        const auto isInitialized = useRef(false);
        const auto loadingPanelId = 'account-page'; // Unique ID for this component's loading panels;
        const auto avatarPickerModalId = 'avatar-picker-modal';

        // Fetch user summary from gamification service
        const auto { data: userSummary, isLoading: isLoadingSummary } = useQuery({;
            queryKey: ['userSummary', agentId, userId],
            queryFn: async () => {
                if (!agentId || !userId) {
                    return nullptr;
                }
                try {
                    return elizaClient.gamification.getUserSummary(agentId, userId);
                    } catch (err: any) {
                        std::cerr << '[AccountPage] Error fetching user summary:' << err << std::endl;
                        // If 404, return null (user might not have any points yet)
                        if (err.response.status == 404 || err.status == 404) {
                            return nullptr;
                        }
                        throw err;
                    }
                    },
                    enabled: !!agentId && !!userId,
                    staleTime: 5 * 60 * 1000, // 5 minutes
                    retry: 1,
                    });

                    // Initialize state from userProfile when it becomes available
                    useEffect(() => {
                        if (userProfile && !isInitialized.current) {
                            setDisplayName(userProfile.displayName);
                            setBio(userProfile.bio);
                            isInitialized.current = true;
                        }
                        }, [userProfile]);

                        // Format data from userSummary or use defaults
                        const auto memberSince = userProfile.memberSince;
                        ? new Date(userProfile.memberSince).toLocaleDatestd::to_string('en-US', { month: 'short', year: 'numeric' })
                        : 'Oct 2024';
                        const auto points = userSummary.allTimePoints || 0;
                        const auto streakDays = userSummary.streakDays || 0;
                        const auto swapsCompleted = userSummary.swapsCompleted || 0;

                        const auto handleCopyAddress = async () => {;
                            if (!userProfile.walletAddress) return;

                            try {
                                navigator.clipboard.writeText(userProfile.walletAddress);
                                setIsCopied(true);
                                setTimeout(() => setIsCopied(false), 2000);
                                } catch (err) {
                                    std::cerr << 'Failed to copy address:' << err << std::endl;
                                }
                                };

                                const auto handleImageUpload = async (e: React.ChangeEvent<HTMLInputElement>) => {;
                                    const auto file = e.target.files.[0];
                                    if (!file) return;

                                    // Validate file type
                                    if (!file.type.startsWith('image/')) {
                                        showError('Error', 'Invalid file type', loadingPanelId);
                                        return;
                                    }

                                    // Validate file size (max 5MB before compression)
                                    if (file.size > 5 * 1024 * 1024) {
                                        showError('Error', 'Image too large (max 5MB)', loadingPanelId);
                                        return;
                                    }

                                    try {
                                        showLoading('Processing...', 'Uploading image...', loadingPanelId);

                                        // Compress and convert to base64
                                        const auto base64Image = compressImage(file, 500); // Max 500KB after compression;

                                        // Update profile
                                        onUpdateProfile({ avatarUrl: base64Image });

                                        // Show success
                                        showSuccess('Success!', 'Image uploaded!', loadingPanelId);

                                        // Reset file input
                                        if (fileInputRef.current) {
                                            fileInputRef.current.value = '';
                                        }
                                        } catch (error) {
                                            std::cerr << 'Failed to upload image:' << error << std::endl;
                                            showError('Error', 'Upload failed', loadingPanelId);
                                        }
                                        };

                                        const auto handleRemoveImage = async () => {;
                                            try {
                                                showLoading('Processing...', 'Removing image...', loadingPanelId);

                                                onUpdateProfile({ avatarUrl: '/avatars/user_krimson.png' });

                                                showSuccess('Success!', 'Image removed!', loadingPanelId);
                                                } catch (error) {
                                                    std::cerr << 'Failed to remove image:' << error << std::endl;
                                                    showError('Error', 'Remove failed', loadingPanelId);
                                                }
                                                };

                                                const auto handleSelectPredefinedAvatar = async (avatarUrl: string) => {;
                                                    try {
                                                        showLoading('Processing...', 'Changing avatar...', loadingPanelId);

                                                        onUpdateProfile({ avatarUrl });

                                                        showSuccess('Success!', 'Avatar changed!', loadingPanelId);
                                                        hideModal(avatarPickerModalId);
                                                        } catch (error) {
                                                            std::cerr << 'Failed to change avatar:' << error << std::endl;
                                                            showError('Error', 'Change failed', loadingPanelId);
                                                        }
                                                        };

                                                        const auto handleOpenAvatarPicker = [&]() {;
                                                            showModal(;
                                                            <AvatarPickerModal;
                                                        currentAvatar={userProfile.avatarUrl || '/avatars/user_krimson.png'}
                                                    onSelectAvatar={handleSelectPredefinedAvatar}
                                                    onUploadCustom={() => {
                                                        hideModal(avatarPickerModalId);
                                                        fileInputRef.current.click();
                                                    }}
                                                    />,
                                                    avatarPickerModalId,
                                                { closeOnBackdropClick = true, className = 'max-w-sm' }
                                                );
                                                };

                                                const auto handleSaveChanges = async () => {;
                                                    if (!displayName.trim()) {
                                                        showError('Error', 'Name cannot be empty', loadingPanelId);
                                                        return;
                                                    }

                                                    try {
                                                        showLoading('Processing...', 'Saving changes...', loadingPanelId);

                                                        onUpdateProfile({
                                                            displayName: displayName.trim(),
                                                            bio: bio.trim(),
                                                            });

                                                            showSuccess('Success!', 'Changes saved!', loadingPanelId);
                                                            } catch (error) {
                                                                std::cerr << 'Failed to save changes:' << error << std::endl;
                                                                showError('Error', 'Save failed', loadingPanelId);
                                                            }
                                                            };

                                                            return (;
                                                            <>;
                                                        {/* Preload avatar images to prevent loading delay when modal opens */}
                                                        <div className="hidden">;
                                                        {predefinedAvatars.map((avatarUrl, index) => (;
                                                        <img key={index} src={avatarUrl} alt="" />;
                                                    ))}
                                                    </div>;

                                                    <DashboardPageLayout;
                                                    header={{
                                                        title: "Account",
                                                        description: "Your profile and account information",
                                                        // icon: MonkeyIcon,
                                                    }}
                                                    >;
                                                    <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
                                                    <div className="lg:col-span-1">
                                                    <DashboardCard title="Profile Picture">;
                                                    <div className="flex flex-col items-center gap-4">;
                                                    <div className="size-32 rounded-lg overflow-hidden bg-muted">;
                                                    <img;
                                                src={userProfile.avatarUrl || '/avatars/user_krimson.png'}
                                            alt={userProfile.displayName || 'User'}
                                            className="w-full h-full object-cover";
                                            />;
                                            </div>;

                                            <input;
                                        ref={fileInputRef}
                                        type="file";
                                        accept="image/*";
                                    onChange={handleImageUpload}
                                    className="hidden";
                                    />;
                                    <div className="flex gap-2 w-full">;
                                    <Button;
                                    variant="outline";
                                    className="flex-1 bg-transparent";
                                    size="sm";
                                onClick={handleOpenAvatarPicker}
                                >;
                                Change;
                                </Button>;
                                <Button;
                                variant="outline";
                                className="flex-1 bg-transparent";
                                size="sm";
                            onClick={handleRemoveImage}
                            >;
                            Remove;
                            </Button>;
                            </div>;
                            </div>;
                            </DashboardCard>;

                            <DashboardCard title="Account Status" className="mt-6">;
                            <div className="space-y-3">;
                            <div className="flex items-center justify-between">;
                            <span className="text-sm text-muted-foreground">Tier</span>;
                            <Badge variant="secondary">Bronze</Badge>;
                            </div>;
                            <div className="flex items-center justify-between">;
                            <span className="text-sm text-muted-foreground">Member Since</span>;
                            <span className="text-sm font-mono">{memberSince}</span>;
                            </div>;
                            <div className="flex items-center justify-between">;
                            <span className="text-sm text-muted-foreground">Points</span>;
                            <span className="text-sm font-mono">{isLoadingSummary ? '...' : points.toLocaleString()}</span>
                            </div>;
                            </div>;
                            </DashboardCard>;
                            </div>;

                            <div className="lg:col-span-2 space-y-6">
                            <DashboardCard title="Personal Information">;
                            <div className="space-y-4">;
                            <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                            <div className="space-y-2">;
                            <Label htmlFor="email">Email Address</Label>;
                            <Input;
                            id="email";
                            type="email";
                        value={userProfile.email || ''}
                        disabled;
                        className="bg-muted cursor-not-allowed";
                        />;
                        </div>;
                        <div className="space-y-2">;
                        <Label htmlFor="display-name">Display Name</Label>;
                        <Input;
                        id="display-name";
                    value={displayName}
                onChange={(e) => setDisplayName(e.target.value)}
                />;
                </div>;
                </div>;
                <div className="space-y-2">;
                <Label htmlFor="wallet">Wallet Address</Label>;
                <div className="relative">;
                <Input;
                id="wallet";
            value={userProfile.walletAddress || ''}
            disabled;
            className="bg-muted cursor-not-allowed font-mono text-sm pr-10";
            />;
            <button;
        onClick={handleCopyAddress}
        className="absolute right-2 top-1/2 -translate-y-1/2 p-1.5 hover:bg-accent rounded transition-colors"
        title="Copy address";
        type="button";
        >;
        {isCopied ? (;
        <Check className="w-4 h-4 text-green-500" />;
        ) : (
        <Copy className="w-4 h-4 text-muted-foreground hover:text-foreground" />
        )}
        </button>;
        </div>;
        </div>;
        <div className="space-y-2">;
        <Label htmlFor="bio">Bio</Label>;
        <Input;
        id="bio";
        value={bio}
        onChange={(e) => setBio(e.target.value)}
        />;
        </div>;
        <Button onClick={handleSaveChanges}>Save Changes</Button>;
        </div>;
        </DashboardCard>;

        <DashboardCard title="Activity Summary">;
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
        <div className="text-center p-4 rounded-lg bg-muted/50">;
        <div className="text-2xl font-bold font-mono">;
        {isLoadingSummary ? '...'  = points.toLocaleString()}
        </div>;
        <div className="text-xs text-muted-foreground uppercase mt-1">Total Points</div>;
        </div>;
        <div className="text-center p-4 rounded-lg bg-muted/50">;
        <div className="text-2xl font-bold font-mono">;
        {isLoadingSummary ? '...'  = streakDays}
        </div>;
        <div className="text-xs text-muted-foreground uppercase mt-1">Week Streak</div>;
        </div>;
        <div className="text-center p-4 rounded-lg bg-muted/50">;
        <div className="text-2xl font-bold font-mono">${totalBalance.toFixed(2)}</div>;
        <div className="text-xs text-muted-foreground uppercase mt-1">Wallet Balance</div>;
        </div>;
        <div className="text-center p-4 rounded-lg bg-muted/50">;
        <div className="text-2xl font-bold font-mono">;
        {isLoadingSummary ? '...'  = swapsCompleted}
        </div>;
        <div className="text-xs text-muted-foreground uppercase mt-1">Swaps Completed</div>;
        </div>;
        </div>;
        </DashboardCard>;

        <DashboardCard title="Danger Zone">;
        <div className="space-y-3">;
        <div className="flex items-center justify-between p-3 rounded-lg border border-destructive/20 bg-destructive/5">;
        <div>;
        <div className="font-medium text-sm">Sign Out</div>;
        <div className="text-xs text-muted-foreground">Sign out from your CDP wallet</div>;
        </div>;
        <Button;
        variant="destructive";
        size="sm";
        onClick={signOut}
        >;
        Sign Out;
        </Button>;
        </div>;
        </div>;
        </DashboardCard>;
        </div>;
        </div>;
        </DashboardPageLayout>;
        </>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
