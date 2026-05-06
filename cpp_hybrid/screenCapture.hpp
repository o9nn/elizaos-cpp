#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_SRC_UTILS_SCREENCAPTURE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_GAME_SRC_UTILS_SCREENCAPTURE_H
#include "core.h"

class ScreenCaptureCapabilities;

class ScreenCaptureCapabilities : public object, public std::enable_shared_from_this<ScreenCaptureCapabilities> {
public:
    using std::enable_shared_from_this<ScreenCaptureCapabilities>::shared_from_this;
    boolean available;

    boolean displayMedia;

    boolean getUserMedia;

    string error;
};

std::shared_ptr<ScreenCaptureCapabilities> checkScreenCaptureCapabilities();

std::shared_ptr<Promise<boolean>> testScreenCapture();

string getScreenCaptureErrorMessage(any error);

#endif
