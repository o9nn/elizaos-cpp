#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/game/src/utils/screenCapture.h"

std::shared_ptr<ScreenCaptureCapabilities> checkScreenCaptureCapabilities()
{
    auto capabilities = object{
        object::pair{std::string("available"), false}, 
        object::pair{std::string("displayMedia"), false}, 
        object::pair{std::string("getUserMedia"), false}
    };
    try
    {
        if (!navigator->mediaDevices) {
            capabilities->error = std::string("navigator.mediaDevices not available");
            return capabilities;
        }
        if (in(std::string("getDisplayMedia"), navigator->mediaDevices)) {
            capabilities->displayMedia = true;
        } else {
            capabilities->error = std::string("getDisplayMedia not supported");
        }
        if (in(std::string("getUserMedia"), navigator->mediaDevices)) {
            capabilities->getUserMedia = true;
        }
        capabilities->available = capabilities->displayMedia;
    }
    catch (const any& error)
    {
        capabilities->error = std::string("Error checking capabilities: ") + error + string_empty;
    }
    return capabilities;
};


std::shared_ptr<Promise<boolean>> testScreenCapture()
{
    auto capabilities = checkScreenCaptureCapabilities();
    if (!capabilities->displayMedia) {
        console->error(std::string("Screen capture not available:"), capabilities->error);
        return false;
    }
    try
    {
        auto stream = std::async([=]() { navigator->mediaDevices->getDisplayMedia(object{
            object::pair{std::string("video"), object{
                object::pair{std::string("frameRate"), object{
                    object::pair{std::string("ideal"), 1}, 
                    object::pair{std::string("max"), 1}
                }}
            }}, 
            object::pair{std::string("audio"), false}
        }); });
        stream->getTracks()->forEach([=](auto track) mutable
        {
            return track["stop"]();
        }
        );
        console->log(std::string("✅ Screen capture test successful"));
        return true;
    }
    catch (const any& error)
    {
        if (error["name"] == std::string("NotAllowedError")) {
            console->log(std::string("ℹ️ Screen capture test cancelled by user"));
            return true;
        }
        console->error(std::string("❌ Screen capture test failed:"), error);
        return false;
    }
};


string getScreenCaptureErrorMessage(any error)
{
    if (error["name"] == std::string("NotAllowedError")) {
        return std::string("Screen sharing permission denied. Please allow access and try again.");
    }
    if (error["name"] == std::string("NotFoundError")) {
        return std::string("No screen sources available for sharing.");
    }
    if (error["name"] == std::string("NotReadableError")) {
        return std::string("Screen source is currently unavailable. It may be in use by another application.");
    }
    if (error["name"] == std::string("OverconstrainedError")) {
        return std::string("Screen sharing constraints cannot be satisfied.");
    }
    if (error["name"] == std::string("TypeError")) {
        return std::string("Invalid screen sharing configuration.");
    }
    return std::string("Screen sharing error: ") + (OR((OR((error["message"]), (error["name"]))), (std::string("Unknown error")))) + string_empty;
};


