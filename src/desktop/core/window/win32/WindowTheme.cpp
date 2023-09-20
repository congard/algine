/**
 * This file uses undocumented Windows features.
 * Based on Windows Terminal sources:
 * https://github.com/microsoft/terminal/blob/v0.8.10261.0/src/interactivity/win32/windowtheme.cpp
 * Copyright (c) Microsoft Corporation, congard.
 */

#ifdef _WIN32

#include "WindowTheme.h"

#include <algine/core/log/Log.h>
#include <memory>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <atlbase.h>
#include <comdef.h>
#include <dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "UxTheme.lib")

#define RETURN_IF_FAILED(hr) { if (FAILED(hr)) return hr; }

#define DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 19

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#define DARK_MODE_STRING_NAME L"DarkMode_Explorer"
#define UXTHEME_DLL_NAME L"uxtheme.dll"
#define UXTHEME_SHOULDAPPSUSEDARKMODE_ORDINAL 132

#define LOG_TAG "win32::WindowTheme"

namespace algine::internal::win32::WindowTheme {
// Routine Description:
// - Attempts to set the dark mode on the given HWND.
// - Will check the system for user preferences and high contrast to see if it's a good idea
//   before setting it.
// Arguments:
// - hwnd - Window to apply dark mode to
// Return Value:
// - S_OK or suitable HRESULT from theming or DWM engines.
static HRESULT tryApplyTheme(HWND hwnd) noexcept;

// Routine Description:
// - Logical determination of if we should use the dark mode or not.
// - Combines user preferences and high contrast accessibility settings.
// Arguments:
// - <none>
// Return Value:
// - TRUE if dark mode is allowed. FALSE if it is not.
static bool isDarkMode() noexcept;

// Routine Description:
// - Looks up the high contrast state of the system.
// Arguments:
// - <none>
// Return Value:
// - True if the system is in high contrast (shouldn't change theme further.) False otherwise.
static bool isHighContrast() noexcept;

// Routine Description:
// - Looks up the user preference for dark mode.
// Arguments:
// - <none>
// Return Value:
// - True if the user chose dark mode in settings. False otherwise.
static bool shouldAppsUseDarkMode() noexcept;

bool applyTheme(GLFWwindow *window) {
    auto handle = glfwGetWin32Window(window);
    auto result = tryApplyTheme(handle);

    if (result == S_OK) {
        Log::debug(LOG_TAG) << "Color scheme has been successfully applied";
    } else {
        _com_error err(result);
        LPCTSTR errMsg = err.ErrorMessage();
        std::string errMsgStr {ATL::CT2A(errMsg)};
        Log::debug(LOG_TAG) << "Error: " << errMsgStr;
    }

    return result == S_OK;
}

HRESULT tryApplyTheme(HWND hwnd) noexcept {
    // I have to be a big B BOOL or DwnSetWindowAttribute will be upset (E_INVALIDARG) when I am passed in.
    const BOOL darkMode = isDarkMode();

    Log::debug(LOG_TAG) << "Current color scheme: " << (darkMode ? "dark" : "light");

    if (darkMode) {
        RETURN_IF_FAILED(SetWindowTheme(hwnd, DARK_MODE_STRING_NAME, nullptr))
    } else {
        RETURN_IF_FAILED(SetWindowTheme(hwnd, L"", nullptr))
    }

    if (FAILED(DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode)))) {
        RETURN_IF_FAILED(DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, &darkMode, sizeof(darkMode)))
    }

    return S_OK;
}

bool isDarkMode() noexcept {
    return shouldAppsUseDarkMode() && !isHighContrast();
}

bool isHighContrast() noexcept {
    BOOL fHighContrast = FALSE;
    HIGHCONTRAST hc = { sizeof(hc) };

    if (SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0))
        fHighContrast = static_cast<BOOL>(HCF_HIGHCONTRASTON & hc.dwFlags);

    return fHighContrast;
}

bool shouldAppsUseDarkMode() noexcept {
    using unique_hmodule = std::unique_ptr<std::remove_pointer_t<HMODULE>, BOOL(*)(HMODULE)>;

    // NOTE: Use LoadLibraryExW with LOAD_LIBRARY_SEARCH_SYSTEM32 flag below to avoid unneeded directory traversal.
    //       This has triggered CPG boot IO warnings in the past.
    static unique_hmodule module {
        LoadLibraryExW(UXTHEME_DLL_NAME, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32),
        [](HMODULE hmodule) { return FreeLibrary(hmodule); }
    };

    if (module != nullptr) {
        typedef bool(WINAPI * PfnShouldAppsUseDarkMode)();

        static bool tried = false;
        static PfnShouldAppsUseDarkMode pfn = nullptr;

        if (!tried) {
            pfn = (PfnShouldAppsUseDarkMode) GetProcAddress(module.get(), MAKEINTRESOURCEA(UXTHEME_SHOULDAPPSUSEDARKMODE_ORDINAL));
        }

        tried = true;

        if (pfn != nullptr) {
            return pfn();
        }
    }

    return false;
}
}

#endif