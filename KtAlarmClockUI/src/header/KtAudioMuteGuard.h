#ifndef KtAudioMuteGuard_H
#define KtAudioMuteGuard_H

#include <QtGlobal>

class KtAudioMuteGuard {
public:
    KtAudioMuteGuard();
    ~KtAudioMuteGuard();

    void disengage();
    void engage();
    void ensure_muted();

private:
    bool ensure_endpoint();
    void release_endpoint();

private:
    bool active_;
    bool hasPreviousMute_;
    bool previousMute_;

#ifdef Q_OS_WIN
    void* endpointVolume_;
    bool  comInitialized_;
#endif
};

#endif // KtAudioMuteGuard_H
