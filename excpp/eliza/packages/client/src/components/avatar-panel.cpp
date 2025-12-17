#include "avatar-panel.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void AvatarPanel() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [avatar, setAvatar] = useState<string | nullptr>(characterValue.settings.avatar || nullptr);
    const auto [hasChanged, setHasChanged] = useState(false);
    const auto fileInputRef = useRef<HTMLInputElement>(nullptr);

    // Reset the change flag when component initializes or character changes
    useEffect(() => {
        setAvatar(characterValue.settings.avatar || nullptr);
        setHasChanged(false);
        }, [characterValue.id]);

        const auto handleFileUpload = async (event: React.ChangeEvent<HTMLInputElement>) => {;
            const auto file = event.target.files.[0];
            if (file) {
                try {
                    const auto compressedImage = compressImage(file);
                    setAvatar(compressedImage);
                    setHasChanged(true);

                    // Only update when there's a real change
                    updateCharacterAvatar(compressedImage);
                    } catch (error) {
                        std::cerr << 'Error compressing image:' << error << std::endl;
                    }
                }
                };

                const auto handleRemoveAvatar = [&]() {;
                    if (avatar) {
                        setAvatar(nullptr);
                        setHasChanged(true);
                        updateCharacterAvatar('');
                    }
                    };

                    // Centralized update function to avoid code duplication
                    const auto updateCharacterAvatar = [&](avatarUrl: string) {;
                        if (setCharacterValue.updateAvatar) {
                            // Use the specialized method for avatar updates when available
                            setCharacterValue.updateAvatar(avatarUrl);
                            } else if (setCharacterValue.updateSetting) {
                                // Use updateSetting as fallback
                                setCharacterValue.updateSetting('avatar', avatarUrl);
                                } else if (setCharacterValue.updateField) {
                                    // Last resort - use the generic field update
                                    setCharacterValue.updateField('settings.avatar', avatarUrl);
                                }
                                };

                                return (;
                                <div className="rounded-lg w-full">;
                                <h2 className="text-xl font-bold mb-4 pb-5 ml-1">Avatar Settings</h2>;

                                <div className="flex flex-col items-center gap-4 pb-4 max-w-sm mx-auto">;
                            {/* Image preview area */}
                            {avatar ? (;
                            <div className="w-64 h-64 mb-2">;
                            <img;
                        src={avatar}
                        alt="Agent Avatar";
                        className="object-cover w-full h-full rounded-lg border";
                        />;
                        </div>;
                        ) : (
                        <div;
                        className="w-64 h-64 flex items-center justify-center border border-dashed rounded-lg text-gray-500 mb-2 cursor-pointer hover:bg-gray-50 transition-colors"
                    onClick={() => fileInputRef.current.click()}
                    >;
                    <div className="flex flex-col items-center gap-2">;
                    <ImageIcon className="w-10 h-10" />;
                    <span className="text-sm">Click to upload</span>;
                    </div>;
                    </div>;
                )}

            {/* Controls area */}
            <div className="flex flex-col gap-3 w-64">;
            <input;
            type="file";
            accept="image/*";
            className="hidden";
        ref={fileInputRef}
    onChange={handleFileUpload}
    />;

    <div className="flex gap-2">;
    <Button;
    type="button";
    className="flex items-center gap-2 flex-1";
    onClick={() => fileInputRef.current.click()}
    >;
    <Upload className="w-5 h-5" />;
    {avatar ? 'Replace'  = 'Upload'}
    </Button>;

    {avatar && (;
    <Button;
    type="button";
    variant="outline";
    className="flex items-center";
    onClick={handleRemoveAvatar}
    >;
    <X className="w-5 h-5" />;
    </Button>;
    )}
    </div>;

    {hasChanged && (;
    <div className="text-sm text-blue-500 mt-1 text-center">Avatar has been updated</div>;
    )}

    <div className="flex items-center justify-center gap-1 text-xs text-muted-foreground mt-1">;
    <Info className="w-3.5 h-3.5" />;
    <span>Images greater than 300x300 will be resized</span>;
    </div>;
    </div>;
    </div>;
    </div>;
    );

}

} // namespace elizaos
