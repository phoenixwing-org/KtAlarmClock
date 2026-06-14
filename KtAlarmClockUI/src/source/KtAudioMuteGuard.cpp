#include "KtAudioMuteGuard.h"

#include <QDebug>

#ifdef Q_OS_WIN
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <objbase.h>
#endif

KtAudioMuteGuard::KtAudioMuteGuard()
    : active_(false)
    , hasPreviousMute_(false)
    , previousMute_(false)
#ifdef Q_OS_WIN
    , endpointVolume_(nullptr)
    , comInitialized_(false)
#endif
{
}

KtAudioMuteGuard::~KtAudioMuteGuard() {
    disengage();
}

void KtAudioMuteGuard::disengage() {
#ifdef Q_OS_WIN
    auto* volume = static_cast<IAudioEndpointVolume*>(endpointVolume_);
    if (volume && hasPreviousMute_)
        volume->SetMute(previousMute_ ? TRUE : FALSE, nullptr);

    release_endpoint();
#endif

    active_ = false;
    hasPreviousMute_ = false;
    previousMute_ = false;
}

void KtAudioMuteGuard::engage() {
    if (active_)
        return;

    if (!ensure_endpoint())
        return;

#ifdef Q_OS_WIN
    auto* volume = static_cast<IAudioEndpointVolume*>(endpointVolume_);
    BOOL muted = FALSE;
    if (FAILED(volume->GetMute(&muted))) {
        release_endpoint();
        return;
    }

    previousMute_ = muted == TRUE;
    hasPreviousMute_ = true;
    if (FAILED(volume->SetMute(TRUE, nullptr))) {
        release_endpoint();
        hasPreviousMute_ = false;
        previousMute_ = false;
        return;
    }
#endif

    active_ = true;
}

void KtAudioMuteGuard::ensure_muted() {
    if (!active_)
        return;

#ifdef Q_OS_WIN
    if (!ensure_endpoint())
        return;

    auto* volume = static_cast<IAudioEndpointVolume*>(endpointVolume_);
    volume->SetMute(TRUE, nullptr);
#endif
}

bool KtAudioMuteGuard::ensure_endpoint() {
#ifdef Q_OS_WIN
    if (endpointVolume_)
        return true;

    const HRESULT initResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(initResult)) {
        comInitialized_ = true;
    }
    else if (initResult == RPC_E_CHANGED_MODE) {
        comInitialized_ = false;
    }
    else {
        qWarning() << "[AudioMuteGuard] failed to initialize COM" << Qt::hex << initResult;
        return false;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT result = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(&enumerator));
    if (FAILED(result) || !enumerator) {
        qWarning() << "[AudioMuteGuard] failed to create device enumerator" << Qt::hex << result;
        release_endpoint();
        return false;
    }

    IMMDevice* device = nullptr;
    result = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    enumerator->Release();
    if (FAILED(result) || !device) {
        qWarning() << "[AudioMuteGuard] failed to get default render endpoint" << Qt::hex << result;
        release_endpoint();
        return false;
    }

    IAudioEndpointVolume* volume = nullptr;
    result = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr,
                              reinterpret_cast<void**>(&volume));
    device->Release();
    if (FAILED(result) || !volume) {
        qWarning() << "[AudioMuteGuard] failed to activate endpoint volume" << Qt::hex << result;
        release_endpoint();
        return false;
    }

    endpointVolume_ = volume;
    return true;
#else
    return false;
#endif
}

void KtAudioMuteGuard::release_endpoint() {
#ifdef Q_OS_WIN
    if (endpointVolume_) {
        static_cast<IAudioEndpointVolume*>(endpointVolume_)->Release();
        endpointVolume_ = nullptr;
    }

    if (comInitialized_) {
        CoUninitialize();
        comInitialized_ = false;
    }
#endif
}
