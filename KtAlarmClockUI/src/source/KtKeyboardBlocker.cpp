#include "KtKeyboardBlocker.h"

#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>

namespace {
KtKeyboardBlocker* g_blocker = nullptr;

bool is_key_down(int virtualKey) {
    return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

bool should_block_key(const KBDLLHOOKSTRUCT* key) {
    if (!key)
        return false;

    const DWORD vk = key->vkCode;
    const bool altDown = (key->flags & LLKHF_ALTDOWN) != 0;
    const bool ctrlDown = is_key_down(VK_CONTROL) || is_key_down(VK_LCONTROL) ||
                          is_key_down(VK_RCONTROL);
    const bool shiftDown = is_key_down(VK_SHIFT) || is_key_down(VK_LSHIFT) ||
                           is_key_down(VK_RSHIFT);

    if (vk == VK_LWIN || vk == VK_RWIN)
        return true;
    if (altDown && (vk == VK_TAB || vk == VK_ESCAPE || vk == VK_F4 || vk == VK_SPACE))
        return true;
    if (ctrlDown && vk == VK_ESCAPE)
        return true;
    if (ctrlDown && shiftDown && vk == VK_ESCAPE)
        return true;

    return false;
}

LRESULT CALLBACK keyboard_hook_proc(int code, WPARAM wParam, LPARAM lParam) {
    Q_UNUSED(wParam);

    if (code == HC_ACTION && g_blocker && g_blocker->enabled()) {
        const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if (should_block_key(key))
            return 1;
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}
} // namespace
#endif

KtKeyboardBlocker::KtKeyboardBlocker()
    : enabled_(false)
#ifdef Q_OS_WIN
    , hook_(nullptr)
#endif
{
}

KtKeyboardBlocker::~KtKeyboardBlocker() {
    set_enabled(false);
}

bool KtKeyboardBlocker::enabled() const {
    return enabled_;
}

void KtKeyboardBlocker::set_enabled(bool enabled) {
    if (enabled_ == enabled)
        return;

    enabled_ = enabled;
    if (enabled_)
        install();
    else
        uninstall();
}

void KtKeyboardBlocker::install() {
#ifdef Q_OS_WIN
    if (hook_)
        return;

    g_blocker = this;
    hook_ = SetWindowsHookExW(WH_KEYBOARD_LL, keyboard_hook_proc, GetModuleHandleW(nullptr), 0);
    if (!hook_) {
        qWarning() << "[KeyboardBlocker] failed to install hook" << GetLastError();
        enabled_ = false;
        if (g_blocker == this)
            g_blocker = nullptr;
    }
#endif
}

void KtKeyboardBlocker::uninstall() {
#ifdef Q_OS_WIN
    if (hook_) {
        UnhookWindowsHookEx(static_cast<HHOOK>(hook_));
        hook_ = nullptr;
    }

    if (g_blocker == this)
        g_blocker = nullptr;
#endif
}
