#include "thread.hpp"

#include "mods/svc/log.hpp"

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace slugcat::vrm::helpers {

void setThreadName(char const *name) {
#if _WIN32
    wchar_t buffer[256];
    const auto converted = MultiByteToWideChar(
        CP_UTF8,
        0,
        name,
        -1,
        buffer,
        sizeof(buffer)/sizeof(wchar_t));
    if (converted == 0) {
        return;
    }

    auto const result = SetThreadDescription(GetCurrentThread(), buffer);
    if (!SUCCEEDED(result)) {
        mods::log::error("Failed to set thread name for {}", name);
    }
#endif
}

void setThreadLowPriority() {
#if _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif
}

} // namespace slugcat::vrm::helpers
