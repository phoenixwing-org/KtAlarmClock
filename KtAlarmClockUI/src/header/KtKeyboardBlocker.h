#ifndef KtKeyboardBlocker_H
#define KtKeyboardBlocker_H

#include <QtGlobal>

class KtKeyboardBlocker {
public:
    KtKeyboardBlocker();
    ~KtKeyboardBlocker();

    bool enabled() const;
    void set_enabled(bool enabled);

private:
    void install();
    void uninstall();

private:
    bool enabled_;

#endif
#ifdef Q_OS_WIN
    void* hook_;
};

#endif // KtKeyboardBlocker_H
