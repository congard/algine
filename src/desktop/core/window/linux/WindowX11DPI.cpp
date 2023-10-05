#ifdef __linux__

#include "WindowX11DPI.h"

#include <algine/core/log/Log.h>

#include <X11/Xlib.h>
#include <X11/Xresource.h>

#include <format>

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define LOG_TAG "linux_x11::WindowDPI"
#define DPI_DEFAULT 96

namespace algine::internal::linux_x11::WindowDPI {
uint32_t getDPI() {
    // prints fallback message and returns the default dpi value
    auto fallbackToDefault = [] {
        Log::debug(LOG_TAG, std::format("Falling back to default DPI value: {}", DPI_DEFAULT));
        return DPI_DEFAULT;
    };

    char *resourceString;

    if (auto display = glfwGetX11Display(); display != nullptr) {
        resourceString = XResourceManagerString(glfwGetX11Display());
    } else {
        Log::error(LOG_TAG, "Error while getting the current X11 display");
        return fallbackToDefault();
    }

    if (resourceString == nullptr) {
        Log::error(LOG_TAG, "Error while retrieving X resource string");
        return fallbackToDefault();
    }

    // we need to initialize the DB before calling Xrm* functions
    XrmInitialize();

    XrmDatabase db = XrmGetStringDatabase(resourceString);
    XrmValue value;

    char *type = nullptr;

    if (XrmGetResource(db, "Xft.dpi", "String", &type, &value) == True && value.addr != nullptr) {
        char *end;

        if (uint32_t dpi = std::strtol(value.addr, &end, 10); *end == '\0') {
            Log::debug(LOG_TAG) << "DPI: " << dpi;
            return dpi;
        } else {
            Log::error(LOG_TAG,
                std::format("Error while parsing Xft.dpi: value: '{}', type: '{}'", value.addr, type));
            return fallbackToDefault();
        }
    } else {
        Log::error(LOG_TAG, "Error while reading Xft.dpi variable from Xresources");
        return fallbackToDefault();
    }
}
} // algine::internal::linux_x11::WindowDPI

#endif //__linux__