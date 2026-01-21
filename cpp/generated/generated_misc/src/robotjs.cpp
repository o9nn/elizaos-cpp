#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/tests/mocks/robotjs.h"

object mockScreenCapture = object{
    object::pair{std::string("image"), Buffer::from(std::string("mock-screenshot-data"))}, 
    object::pair{std::string("width"), 1920}, 
    object::pair{std::string("height"), 1080}, 
    object::pair{std::string("byteWidth"), 7680}, 
    object::pair{std::string("bitsPerPixel"), 32}, 
    object::pair{std::string("bytesPerPixel"), 4}
};
std::any getScreenSize = vi->fn([=]() mutable
{
    return (object{
        object::pair{std::string("width"), 1920}, 
        object::pair{std::string("height"), 1080}
    });
}
);
object screen = object{
    object::pair{std::string("capture"), vi->fn([=]() mutable
    {
        return mockScreenCapture;
    }
    )}
};
std::any moveMouse = vi->fn();
std::any mouseClick = vi->fn();
std::any typeString = vi->fn();
std::any keyTap = vi->fn();
std::any keyToggle = vi->fn();
std::any getMousePos = vi->fn([=]() mutable
{
    return (object{
        object::pair{std::string("x"), 100}, 
        object::pair{std::string("y"), 100}
    });
}
);
std::any getPixelColor = vi->fn([=]() mutable
{
    return std::string("ffffff");
}
);
std::any setMouseDelay = vi->fn();
std::any setKeyboardDelay = vi->fn();

void Main(void)
{
}

MAIN
