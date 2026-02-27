#include "screenCapture.hpp"

std::shared_ptr<ScreenCaptureCapabilities> checkScreenCaptureCapabilities()
{
    auto capabilities = object{
        object::pair{std:("available"), false}, 
        object::pair{std:("displayMedia"), false}, 
        object::pair{std:("getUserMedia"), false}
    };
    try
    {
        if (!navigator->mediaDevices) {
            capabilities->error = std:("navigator.mediaDevices not available");
            return capabilities;
        }
        if (in(std:("getDisplayMedia"), navigator->mediaDevices)) {
            capabilities->displayMedia = true;
        } else {
            capabilities->error = std:("getDisplayMedia not supported");
        }
        if (in(std:("getUserMedia"), navigator->mediaDevices)) {
            capabilities->getUserMedia = true;
        }
        capabilities->available = capabilities->displayMedia;
    }
    catch (const any& error)
    {
        capabilities->error = std:("Error checking capabilities: ") + error + string_empty;
    }
    return capabilities;
};


std::shared_ptr<Promise<boolean>> testScreenCapture()
{
    auto capabilities = checkScreenCaptureCapabilities();
    if (!capabilities->displayMedia) {
        console->error(std:("Screen capture not available:"), capabilities->error);
        return false;
    }
    try
    {
        auto stream = std::async([=]() { navigator->mediaDevices->getDisplayMedia(object{
            object::pair{std:("video"), object{
                object::pair{std:("frameRate"), object{
                    object::pair{std:("ideal"), 1}, 
                    object::pair{std:("max"), 1}
                }}
            }}, 
            object::pair{std:("audio"), false}
        }); });
        stream->getTracks()->forEach([=](auto track) mutable
        {
            return track["stop"]();
        }
        );
        console->log(std:("✅ Screen capture test successful"));
        return true;
    }
    catch (const any& error)
    {
        if (error["name"] == std:("NotAllowedError")) {
            console->log(std:("ℹ️ Screen capture test cancelled by user"));
            return true;
        }
        console->error(std:("❌ Screen capture test failed:"), error);
        return false;
    }
};


string getScreenCaptureErrorMessage(any error)
{
    if (error["name"] == std:("NotAllowedError")) {
        return std:("Screen sharing permission denied. Please allow access and try again.");
    }
    if (error["name"] == std:("NotFoundError")) {
        return std:("No screen sources available for sharing.");
    }
    if (error["name"] == std:("NotReadableError")) {
        return std:("Screen source is currently unavailable. It may be in use by another application.");
    }
    if (error["name"] == std:("OverconstrainedError")) {
        return std:("Screen sharing constraints cannot be satisfied.");
    }
    if (error["name"] == std:("TypeError")) {
        return std:("Invalid screen sharing configuration.");
    }
    return std:("Screen sharing error: ") + (OR((OR((error["message"]), (error["name"]))), (std:("Unknown error")))) + string_empty;
};


